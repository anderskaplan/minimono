// Main.cpp : Defines the entry point for the console application.

#include "MonoWrap.h"

const char* monoDllPath = "C:/Program Files (x86)/Unity/Editor/Data/Mono/EmbedRuntime/mono.dll";
const char* monoAssemblyDir = "C:/Program Files (x86)/Unity/Editor/Data/Mono/lib";
const char* monoConfigDir = "C:/Program Files (x86)/Unity/Editor/Data/Mono/etc";
const char* assemblyPath = "C:/git/minimono/Sleeper/bin/Debug/Sleeper.exe";

int main(int argc, char* argv[])
{
	init_mono(monoDllPath, monoAssemblyDir, monoConfigDir);
	exec_mono(assemblyPath, argc, argv);

	return 0;
}
