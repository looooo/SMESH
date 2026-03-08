// Crash handler for Windows: prints stack trace to stderr on unhandled exception.
// Used in CI to debug crashes without a local Windows machine.
#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0600  /* CaptureStackBackTrace requires Vista+ */
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma comment(lib, "dbghelp.lib")
#endif

namespace {

void write_stderr(const char* msg) {
    HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    if (h && h != INVALID_HANDLE_VALUE) {
        DWORD n = (DWORD)strlen(msg);
        WriteFile(h, msg, n, &n, NULL);
    }
}

LONG WINAPI crash_filter(EXCEPTION_POINTERS* p) {
    write_stderr("\n[CRASH] Unhandled exception 0x");
    char buf[32];
    sprintf(buf, "%08lX", (unsigned long)p->ExceptionRecord->ExceptionCode);
    write_stderr(buf);
    write_stderr(" at ");
    sprintf(buf, "%p\n", (void*)p->ExceptionRecord->ExceptionAddress);
    write_stderr(buf);

    void* stack[64];
    DWORD n = CaptureStackBackTrace(0, 64, stack, NULL);
    write_stderr("[CRASH] Stack trace:\n");

    HANDLE proc = GetCurrentProcess();
    SymInitialize(proc, NULL, TRUE);
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

    for (DWORD i = 0; i < n; i++) {
        DWORD64 addr = (DWORD64)stack[i];
        char sym_buf[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO* sym = (SYMBOL_INFO*)sym_buf;
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = 255;

        if (SymFromAddr(proc, addr, NULL, sym)) {
            sprintf(buf, "  #%lu 0x%p %s\n", (unsigned long)i, (void*)addr, sym->Name);
        } else {
            sprintf(buf, "  #%lu 0x%p\n", (unsigned long)i, (void*)addr);
        }
        write_stderr(buf);
    }
    SymCleanup(proc);
    return EXCEPTION_EXECUTE_HANDLER;
}

struct CrashHandlerInstaller {
    CrashHandlerInstaller() { SetUnhandledExceptionFilter(crash_filter); }
} g_crashHandler;
}
#endif
