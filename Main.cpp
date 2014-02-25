// Main.cpp : Defines the entry point for the console application.

#include "MonoWrap.h"
#include <iostream>

const char* monoDllPath = "C:/Program Files (x86)/Unity/Editor/Data/Mono/EmbedRuntime/mono.dll";
const char* monoAssemblyDir = "C:/Program Files (x86)/Unity/Editor/Data/Mono/lib";
const char* monoConfigDir = "C:/Program Files (x86)/Unity/Editor/Data/Mono/etc";
const char* assemblyPath = "C:/git/minimono/Sleeper/bin/Debug/Sleeper.exe";

int main(int argc, char* argv[])
{
	std::cout << "MiniMono" << std::endl;

	init_mono(monoDllPath, monoAssemblyDir, monoConfigDir, true);
	install_thread_test_function();
	exec_mono(assemblyPath);
	exit_mono();

	return 0;
}
