#pragma once

void init_mono(const char* monoDllPath, const char* monoAssemblyDir, const char* monoConfigDir);
void exec_mono(const char* assemblyPath, int argc, char** argv);
