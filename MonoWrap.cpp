// Mono.cpp

#include "Windows.h"
#include <stdexcept>
#include <stdint.h>

#define MONO_CALL

struct MonoDomain;
struct MonoAssembly;
struct MonoImage;
struct MonoMethod;
typedef uint32_t guint32;

typedef enum {
	MONO_DEBUG_FORMAT_NONE,
	MONO_DEBUG_FORMAT_MONO,
	MONO_DEBUG_FORMAT_DEBUGGER
} MonoDebugFormat;

// typedefs for function pointers
typedef void (MONO_CALL *mono_set_dirs_t)(const char *assembly_dir, const char *config_dir);
typedef void (MONO_CALL *mono_config_parse_t)(const char* filename);
typedef MonoDomain* (MONO_CALL *mono_jit_init_version_t)(const char *domain_name, const char *runtime_version);
typedef MonoAssembly* (MONO_CALL *mono_domain_assembly_open_t)(MonoDomain *domain, const char *name);
typedef int (MONO_CALL *mono_jit_exec_t)(MonoDomain *domain, MonoAssembly *assembly, int argc, char *argv[]);
//typedef MonoImage* (MONO_CALL *mono_assembly_get_image_t)(MonoAssembly *assembly);
//typedef guint32 (MONO_CALL *mono_image_get_entry_point_t)(MonoImage *image);
typedef void (MONO_CALL *mono_debug_init_t)(MonoDebugFormat format);
//typedef void (MONO_CALL *mono_debug_domain_create_t)(MonoDomain *domain);
typedef void (MONO_CALL *mono_jit_parse_options_t)(int argc, char *argv[]);

// function pointers
static mono_set_dirs_t mono_set_dirs_func;
static mono_config_parse_t mono_config_parse_func;
static mono_jit_init_version_t mono_jit_init_version_func;
static mono_domain_assembly_open_t mono_domain_assembly_open_func;
static mono_jit_exec_t mono_jit_exec_func;
//static mono_assembly_get_image_t mono_assembly_get_image_func;
//static mono_image_get_entry_point_t mono_image_get_entry_point_func;
static mono_debug_init_t mono_debug_init_func;
//static mono_debug_domain_create_t mono_debug_domain_create_func;
static mono_jit_parse_options_t mono_jit_parse_options_func;

static HINSTANCE hinstance = 0;
static MonoDomain *domain = 0;

static void map_mono_functions()
{
	mono_set_dirs_func = (mono_set_dirs_t)GetProcAddress(hinstance, "mono_set_dirs");
    mono_config_parse_func = (mono_config_parse_t)GetProcAddress(hinstance, "mono_config_parse");
	mono_jit_init_version_func = (mono_jit_init_version_t)GetProcAddress(hinstance, "mono_jit_init_version");
	mono_domain_assembly_open_func = (mono_domain_assembly_open_t)GetProcAddress(hinstance, "mono_domain_assembly_open");
	mono_jit_exec_func = (mono_jit_exec_t)GetProcAddress(hinstance, "mono_jit_exec");
	//mono_assembly_get_image_func = (mono_assembly_get_image_t)GetProcAddress(hinstance, "mono_assembly_get_image");
	//mono_image_get_entry_point_func = (mono_image_get_entry_point_t)GetProcAddress(hinstance, "mono_image_get_entry_point");
	mono_debug_init_func = (mono_debug_init_t)GetProcAddress(hinstance, "mono_debug_init");
	//mono_debug_domain_create_func = (mono_debug_domain_create_t)GetProcAddress(hinstance, "mono_debug_domain_create");
	mono_jit_parse_options_func = (mono_jit_parse_options_t)GetProcAddress(hinstance, "mono_jit_parse_options");
}

void init_mono(const char* monoDllPath, const char* monoAssemblyDir, const char* monoConfigDir)
{
	hinstance = LoadLibrary(monoDllPath);
    if (!hinstance)
    {
		throw std::runtime_error("Dll loading failed.");
    } 
    
    map_mono_functions();

	// see http://docs.go-mono.com/?link=xhtml%3adeploy%2fmono-api-embedding.html

	static char* options[] = {
//          "--soft-breakpoints",
          "--debugger-agent=transport=dt_socket,address=127.0.0.1:10000"
        };

	mono_jit_parse_options_func(sizeof(options)/sizeof(char*), (char**)options);

	mono_set_dirs_func(monoAssemblyDir, monoConfigDir);
	mono_config_parse_func(NULL);

	mono_debug_init_func(MONO_DEBUG_FORMAT_MONO);

	domain = mono_jit_init_version_func("rootdomain", "v2.0.50727");
	//mono_add_internal_call ("Sample::GetMessage", getMessage);

	//mono_debug_domain_create_func(domain); // NOTE not exported in the Unity mono.dll
}

void exec_mono(const char* assemblyPath)
{
	MonoAssembly *assembly = mono_domain_assembly_open_func(domain, assemblyPath);
	if (!assembly)
	{
		throw std::runtime_error("Assembly loading failed.");
	}

	static char* options[] = { "MonoWrap.exe", NULL };
	mono_jit_exec_func(domain, assembly, 1, options);
}
