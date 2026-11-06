#include "process.hpp"
#include <tlhelp32.h>

bool Process::attach(const wchar_t* exeName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W pe{ sizeof(pe) };
    for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe)) {
        if (_wcsicmp(pe.szExeFile, exeName) == 0) {
            pid_ = pe.th32ProcessID;
            break;
        }
    }
    CloseHandle(snap);
    if (!pid_) return false;

    h_ = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION,
                     FALSE, pid_);
    return h_ != nullptr;
}

void Process::detach() {
    if (h_) { CloseHandle(h_); h_ = nullptr; }
    pid_ = 0;
}

bool Process::alive() const {
    if (!h_) return false;
    DWORD code = 0;
    return GetExitCodeProcess(h_, &code) && code == STILL_ACTIVE;
}

std::uintptr_t Process::moduleBase(const wchar_t* dllName) const {
    if (!pid_) return 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid_);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    MODULEENTRY32W me{ sizeof(me) };
    std::uintptr_t base = 0;
    for (BOOL ok = Module32FirstW(snap, &me); ok; ok = Module32NextW(snap, &me)) {
        if (_wcsicmp(me.szModule, dllName) == 0) {
            base = reinterpret_cast<std::uintptr_t>(me.modBaseAddr);
            break;
        }
    }
    CloseHandle(snap);
    return base;
}
