namespace ProcMemory {
	uintptr_t GetProcessId(const wchar_t* procName) {
		uintptr_t procId = NULL;
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
	uintptr_t GetModuleBaseAddr(DWORD procId, const wchar_t* modName) {
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
