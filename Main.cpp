// Main.cpp : Defines the entry point for the console application.

#include "MonoWrap.h"
#include <Windows.h>
#include <iostream>

//#define MONO_PATH "C:/Program Files (x86)/Unity/Editor/Data/Mono"
#define MONO_PATH "E:/Unity/Editor/Data/Mono"

const char* monoDllPath = MONO_PATH "/EmbedRuntime/mono.dll";
const char* monoAssemblyDir = MONO_PATH "/lib";
const char* monoConfigDir = MONO_PATH "/etc";
const char* assemblyPath = "Sleeper/bin/Debug/Sleeper.exe";

int main(int argc, char* argv[])
{
	std::cout << "MiniMono" << std::endl;

	init_mono(monoDllPath, monoAssemblyDir, monoConfigDir, true);
	install_thread_test_function();

	char fullPath[_MAX_PATH];
	_fullpath(fullPath, assemblyPath, _MAX_PATH);
	exec_mono(fullPath);
	
	exit_mono();

	return 0;
}
