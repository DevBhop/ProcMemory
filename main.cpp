#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>

namespace ProcMemory {
	DWORD GetProcessId(const wchar_t* procName) {
		DWORD procId = NULL;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (hSnap != INVALID_HANDLE_VALUE) {
			PROCESSENTRY32 procEntry;
			procEntry.dwSize = sizeof(procEntry);

			if (Process32First(hSnap, &procEntry)) {
				do {
					if (!_wcsicmp(procEntry.szExeFile, procName)) {
						procId = procEntry.th32ProcessID;
					}
				} while (Process32Next(hSnap, &procEntry));
			}
		}
		CloseHandle(hSnap);
		return procId;
	}

	DWORD GetModuleBaseAddr(DWORD procId, const wchar_t* modName) {
		uintptr_t moduleBaseAddr = NULL;
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

		if (hSnap != INVALID_HANDLE_VALUE) {
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);

			if (Module32First(hSnap, &modEntry)) {
				do {
					if (!_wcsicmp(modEntry.szModule, modName)) {
						moduleBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}
		CloseHandle(hSnap);
		return moduleBaseAddr;
	}

	DWORD FindDMAAddy(HANDLE hProcess, DWORD ptr, std::vector<unsigned int> offsets) {
		DWORD addr = ptr;
		for (unsigned int i = 0; i < offsets.size(); i++) {
			ReadProcessMemory(hProcess, (BYTE*)addr, &addr, sizeof(addr), nullptr);
			addr += offsets[i];
		}
		return addr;
	}

	template<typename T> T RPM(HANDLE hProcess, SIZE_T address) {
		T buffer;
		ReadProcessMemory(hProcess, (LPCVOID*)address, &buffer, sizeof(buffer), NULL);
		return buffer;
	}

	template<typename T> T WPM(HANDLE hProcess, SIZE_T address, T buffer) {
		WriteProcessMemory(hProcess, (LPCVOID*)address, &buffer, sizeof(buffer), NULL);
		return buffer;
	}
}