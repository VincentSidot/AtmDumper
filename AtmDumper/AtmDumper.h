#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream> 
#include <vector>
#include <iomanip>

#define var template<typename T>

namespace AtmDumper
{
	DWORD dump(PVOID const addr, PVOID buffer, DWORD size)
	{
		DWORD oldProtecAddr, oldProtecBuff;

		VirtualProtect(addr, size, PAGE_READONLY, &oldProtecAddr);
		VirtualProtect(buffer, size, PAGE_READWRITE, &oldProtecBuff);
		ZeroMemory(buffer, size);
		CopyMemory(buffer, addr, size);
		VirtualProtect(addr, size, oldProtecAddr, &oldProtecAddr);
		VirtualProtect(buffer, size, oldProtecBuff, &oldProtecBuff);
		return 0;
	}// if okay return 0
	std::string print(PBYTE buffer, DWORD size, DWORD bytePerLine, bool Cformat = false)
	{
		std::stringstream ss;
		std::string tmp;
		if (Cformat)
		{
			ss << "unsigned char rawData[" << size << "] = {\n";
			tmp = ss.str();
			std::fill(tmp.begin(), tmp.end(), ' ');
			ss << tmp;
		}
		for (DWORD i = 0; i < (size - 1); i++)
		{
			ss << "0x" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (WORD) *(buffer + i) << ((Cformat) ? ',' : ' ');
			if (i%bytePerLine == bytePerLine - 1)
			{
				ss << '\n';
				if (Cformat)
					ss << tmp;
			}
		}
		ss << "0x" << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << (WORD) *(buffer + (size - 1));
		if (Cformat)
		{
			ss << "\n";
			for (int i = 0; i < (tmp.length() - 1); ++i)
				ss << ' ';
			ss << "};";
		}
		ss << std::endl;
		return ss.str();
	}
	PROCESSENTRY32 getProcess(WCHAR* const name)
	{
		PROCESSENTRY32 process;
		process.dwSize = sizeof(process);
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(snap, &process))
		{
			do
			{
				if (lstrcmpiW(process.szExeFile, name) == 0)
				{
					return process;
				}
			} while (Process32Next(snap, &process));
		}
		return process;
	}
	DWORD getPid(WCHAR* const name)
	{
		return getProcess(name).th32ProcessID;
	}
	std::vector<PROCESSENTRY32> getAllProcess()
	{
		std::vector<PROCESSENTRY32> rep;
		PROCESSENTRY32 process;
		process.dwSize = sizeof(process);
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (Process32First(snap, &process))
		{
			do
			{
				rep.push_back(process);
			} while (Process32Next(snap, &process));
		}
		return rep;
	}
	DWORD dump(WCHAR* const name, PVOID const addr, PVOID buffer, DWORD size)
	{
		DWORD BuffProtect,AddrProtect;
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, getPid(name));
		if (hProcess == NULL)
			goto error;
		if (VirtualProtect(buffer, size, PAGE_READWRITE, &BuffProtect) == 0)
			goto error;
		DWORD byteRead;
		if (ReadProcessMemory(hProcess, addr, buffer, size, &byteRead) == 0)
			
		if (byteRead != size)
			goto error;
		VirtualProtect(buffer, size, BuffProtect, &BuffProtect);
		return 0;
	error:
		CloseHandle(hProcess);
		return GetLastError();
	}
	DWORD write(WCHAR* const name, PVOID const addr, PVOID buffer, DWORD size)
	{
		DWORD BuffProtect, AddrProtect;
		HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, getPid(name));
		if (hProcess == NULL)
			goto error;
		if (VirtualProtect(buffer, size, PAGE_READONLY, &BuffProtect) == 0)
			goto error;
		if (VirtualProtectEx(hProcess, addr, size, PAGE_READWRITE, &AddrProtect) == 0)
			goto error;
		DWORD byteWrite;
		if (WriteProcessMemory(hProcess, addr, buffer, size, &byteWrite) == 0)
			goto error;
		if (byteWrite != size)
			goto error;
		VirtualProtect(buffer, size, BuffProtect, &BuffProtect);
		VirtualProtectEx(hProcess, addr, size, AddrProtect, &AddrProtect);
		return 0;
	error:
		CloseHandle(hProcess);
		return GetLastError();
	}
	
	class Debuguer
	{
	public:
		bool attach(LPWSTR name)
		{
			PROCESSENTRY32 process;
			process.dwSize = sizeof(process);
			HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (Process32First(snap,&process))
			{
				do
				{
					if (lstrcmpiW(process.szExeFile, name) == 0)
					{
						pid = process.th32ProcessID;
						hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
						CloseHandle(snap);
						return true;
					}
				} while(Process32Next(snap, &process));
			}
			return false;
		}
		~Debuguer() {
			CloseHandle(hProcess);
		}
		var T read(DWORD addr)
		{
			T rep;
			ReadProcessMemory(hProcess, (PVOID)addr, &rep, sizeof(T), NULL);
			return rep;
		}
		var void write(DWORD addr, T val)
		{
			WriteProcessMemory(hProcess,(PVOID)addr,&val,sizof(T),NULL)
		}
		void read(DWORD addr, PBYTE buffer, DWORD buffsize)
		{
			ReadProcessMemory(hProcess, (LPCVOID)addr, buffer, buffsize, NULL);
		}
		void write(DWORD addr, PBYTE const buffer, DWORD buffsize)
		{
			WriteProcessMemory(hProcess, (PVOID)addr, buffer, buffsize, NULL);
		}
		void write(DWORD addr, char* const buffer, DWORD buffsize)
		{
			WriteProcessMemory(hProcess, (PVOID)addr, buffer, buffsize, NULL);
		}
	private:
		DWORD pid;
		HANDLE hProcess;
	};
}