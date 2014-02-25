#pragma once

void init_mono(const char* monoDllPath, const char* monoAssemblyDir, const char* monoConfigDir, bool debug);
void exec_mono(const char* assemblyPath);
void exit_mono();
void install_thread_test_function();
