#pragma once
#include <windows.h>
#include <cstdint>
#include <string>

class Process {
public:
    bool  attach(const wchar_t* exeName);
    void  detach();
    bool  alive() const;

    DWORD                pid()   const { return pid_; }
    std::uintptr_t       moduleBase(const wchar_t* dllName) const;

    template <typename T>
    T read(std::uintptr_t addr) const {
        T v{};
        SIZE_T n = 0;
        if (!h_) return v;
        ReadProcessMemory(h_, reinterpret_cast<LPCVOID>(addr), &v, sizeof(T), &n);
        return v;
    }

    bool readRaw(std::uintptr_t addr, void* dst, std::size_t n) const {
        if (!h_) return false;
        SIZE_T got = 0;
        return ReadProcessMemory(h_, reinterpret_cast<LPCVOID>(addr), dst, n, &got)
               && got == n;
    }

    std::string readString(std::uintptr_t addr, std::size_t max = 128) const {
        if (!h_ || !addr) return {};
        std::string s(max, '\0');
        SIZE_T got = 0;
        if (!ReadProcessMemory(h_, reinterpret_cast<LPCVOID>(addr),
                               s.data(), max, &got) || got == 0) {
            return {};
        }
        s.resize(strnlen(s.data(), got));
        return s;
    }

    std::uintptr_t readPtrChain(std::uintptr_t base,
                                std::initializer_list<std::uintptr_t> offs) const {
        std::uintptr_t a = read<std::uintptr_t>(base);
        for (auto o : offs) {
            if (!a) return 0;
            a = read<std::uintptr_t>(a + o);
        }
        return a;
    }

private:
    HANDLE h_   = nullptr;
    DWORD  pid_ = 0;
};
