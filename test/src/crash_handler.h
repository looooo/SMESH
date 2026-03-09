// Crash handler for Windows: prints stack trace to stderr on unhandled exception.
// Used in CI to debug crashes without a local Windows machine.
// Uses StackWalk64 from dbghelp (no CaptureStackBackTrace/kernel32 dependency).
#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdint.h>
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
    char buf[256];
    sprintf(buf, "%08lX", (unsigned long)p->ExceptionRecord->ExceptionCode);
    write_stderr(buf);
    write_stderr(" at ");
    sprintf(buf, "%p\n", (void*)p->ExceptionRecord->ExceptionAddress);
    write_stderr(buf);

    HANDLE proc = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    if (!SymInitialize(proc, NULL, TRUE)) {
        write_stderr("[CRASH] SymInitialize failed\n");
        return EXCEPTION_EXECUTE_HANDLER;
    }
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);

    STACKFRAME64 frame = {};
    CONTEXT* ctx = p->ContextRecord;
#ifdef _WIN64
    DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset = ctx->Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = ctx->Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = ctx->Rsp;
    frame.AddrStack.Mode = AddrModeFlat;
#else
    DWORD machine = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset = ctx->Eip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = ctx->Ebp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = ctx->Esp;
    frame.AddrStack.Mode = AddrModeFlat;
#endif

    write_stderr("[CRASH] Stack trace:\n");
    for (int i = 0; i < 64; i++) {
        if (!StackWalk64(machine, proc, thread, &frame, ctx, NULL,
                         SymFunctionTableAccess64, SymGetModuleBase64, NULL))
            break;

        if (frame.AddrPC.Offset == 0)
            break;

        DWORD64 addr = frame.AddrPC.Offset;
        char sym_buf[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO* sym = (SYMBOL_INFO*)sym_buf;
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = 255;

        if (SymFromAddr(proc, addr, NULL, sym)) {
            sprintf(buf, "  #%d %p %s\n", i, (void*)(uintptr_t)addr, sym->Name);
        } else {
            sprintf(buf, "  #%d %p\n", i, (void*)(uintptr_t)addr);
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
