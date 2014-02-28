// Cuckoo's Egg

#include <iostream>
#include <Windows.h>

typedef void (*thread_test_callback_t)();

static thread_test_callback_t threadTestCallback = 0;

static DWORD WINAPI delayed_callback(LPVOID lpThreadParameter)
{
	Sleep(5000);
	threadTestCallback();
	return 0;
}

#if defined(__cplusplus)
extern "C"
{
#endif

void __declspec(dllexport) RegisterThreadTestCallbackAsDelegate(thread_test_callback_t callback)
{
	std::cout << "RegisterThreadTestCallbackAsDelegate invoked." << std::endl;
	threadTestCallback = callback;
}

void __declspec(dllexport) InvokeDelegateThreaded()
{
	std::cout << "InvokeDelegateThreaded invoked." << std::endl;

	HANDLE hthread = CreateThread(NULL, 0, delayed_callback, NULL, 0, NULL);
	CloseHandle(hthread);
}

#if defined(__cplusplus)
}
#endif
