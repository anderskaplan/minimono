// Mono.cpp

#include "Windows.h"
#include <stdexcept>
#include <stdint.h>
#include <iostream>

#define MONO_CALL

struct MonoDomain;
struct MonoAssembly;
struct MonoImage;
struct MonoMethod;
typedef uint32_t guint32;
struct MonoThread;

typedef enum {
	MONO_DEBUG_FORMAT_NONE,
	MONO_DEBUG_FORMAT_MONO,
	MONO_DEBUG_FORMAT_DEBUGGER
} MonoDebugFormat;

typedef void *gconstpointer;

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
typedef void (MONO_CALL *mono_jit_cleanup_t)(MonoDomain *domain);
typedef void (MONO_CALL *mono_add_internal_call_t)(const char *name, gconstpointer method);
typedef MonoThread *(MONO_CALL *mono_thread_attach_t)(MonoDomain *domain);
typedef void (MONO_CALL *mono_thread_detach_t)(MonoThread *thread);

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
static mono_jit_cleanup_t mono_jit_cleanup_func;
static mono_add_internal_call_t mono_add_internal_call_func;
static mono_thread_attach_t mono_thread_attach_func;
static mono_thread_detach_t mono_thread_detach_func;

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
	mono_jit_cleanup_func = (mono_jit_cleanup_t)GetProcAddress(hinstance, "mono_jit_cleanup");
	mono_add_internal_call_func = (mono_add_internal_call_t)GetProcAddress(hinstance, "mono_add_internal_call");
	mono_thread_attach_func = (mono_thread_attach_t)GetProcAddress(hinstance, "mono_thread_attach");
	mono_thread_detach_func = (mono_thread_detach_t)GetProcAddress(hinstance, "mono_thread_detach");
}

void init_mono(const char* monoDllPath, const char* monoAssemblyDir, const char* monoConfigDir, bool debug)
{
	hinstance = LoadLibrary(monoDllPath);
    if (!hinstance)
    {
		throw std::runtime_error("Dll loading failed.");
    } 
    
    map_mono_functions();

	// see http://docs.go-mono.com/?link=xhtml%3adeploy%2fmono-api-embedding.html

	if (debug)
	{
		static char* options[] = {
			  //"--soft-breakpoints",
			  "--debugger-agent=transport=dt_socket,address=127.0.0.1:10000"
			};

		mono_jit_parse_options_func(sizeof(options)/sizeof(char*), (char**)options);
	}

	mono_set_dirs_func(monoAssemblyDir, monoConfigDir);
	mono_config_parse_func(NULL);

	if (debug)
	{
		mono_debug_init_func(MONO_DEBUG_FORMAT_MONO);
	}

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

void exit_mono()
{
	mono_jit_cleanup_func(domain);
}

typedef void (*thread_test_callback_t)();

static thread_test_callback_t threadTestCallback = 0;

static void register_thread_test_callback(thread_test_callback_t callback)
{
	std::cout << "thread test callback" << std::endl;
	threadTestCallback = callback;
}

static void invoke_thread_test_callback()
{
	threadTestCallback();
}

static DWORD WINAPI delayed_callback(LPVOID lpThreadParameter)
{
	auto handle = mono_thread_attach_func(domain);

	Sleep(5000);
	invoke_thread_test_callback();

	mono_thread_detach_func(handle);

	return 0;
}

static void invoke_thread_test_callback_threaded()
{
	HANDLE hthread = CreateThread(NULL, 0, delayed_callback, NULL, 0, NULL);
	CloseHandle(hthread);
}

static void invoke_thread_test_callback_from_managed_thread()
{
	Sleep(5000);
	invoke_thread_test_callback();
}

void install_thread_test_function()
{
	mono_add_internal_call_func("Sleeper.Program::RegisterThreadTestCallback", register_thread_test_callback);
	mono_add_internal_call_func("Sleeper.Program::InvokeThreadTestCallback", invoke_thread_test_callback);
	mono_add_internal_call_func("Sleeper.Program::InvokeThreadTestCallbackThreaded", invoke_thread_test_callback_threaded);
	mono_add_internal_call_func("Sleeper.Program::InvokeThreadTestCallbackFromManagedThread", invoke_thread_test_callback_from_managed_thread);
}
