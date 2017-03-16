

#include <iostream>
#include "SharedMemory.hpp"


#ifdef WIN32

#include <Windows.h>
#include <TlHelp32.h>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: 
	{
		//SHM::ProcessMutex::InitMutex();
		SHM::SharedMemoriesManagent & mem = SHM::SharedMemoriesManagent::GetInstace();
		DWORD id = GetCurrentProcessId();
		SHM::SharedMemory &shm = mem.GetSharedMemory("Session");
		//std::cout << id << std::endl;
		mem.ProcessAttached(id); 
	}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//SHM::ProcessMutex::MutexCloseLock();
		SHM::SharedMemoriesManagent::Destory();

		//getchar();
		break;
	}
	return TRUE;
}
std::string pname() 
{
	unsigned long pid = GetCurrentProcessId();
	static std::string name = "";
	if (name != "") 
	{
		return name;
	}
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, pid);
	if (hSnapshot) {
		PROCESSENTRY32 pe32;
		bool found = false;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32)) {
			do {
				if (pe32.th32ProcessID == pid) {
					//printf("pid %d %s\n", pe32.th32ProcessID, pe32.szExeFile);
					found = true;
					name = std::string(pe32.szExeFile);
					break;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
		return name;
		//return found;
	}
	return name;

}
bool CheckProcessExist( unsigned long id ) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, id);
	//std::string name = pname();

	if (hSnapshot) {
		PROCESSENTRY32 pe32;
		bool found = false;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32)) {
			do {
				if (pe32.th32ProcessID == id) {
					//printf("pid %d %s\n", pe32.th32ProcessID, pe32.szExeFile);
					//std::string pname = std::string(pe32.szExeFile);
					//if (name == pname) {
						found = true;
						break;
					//}
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
		return found;
	}
	return false;
}

#else

#ifdef __GNUC__ 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>
//#ifdef __cplusplus
//extern "C" {
//#endif
	static void handler(int sig) 
	{
		exit(128+sig);
	}
	__attribute__((constructor))
		/**
		* initializer of the dylib.
		*/
		static void Initializer(int argc, char** argv, char** envp)
	{
		//signal(SIGKILL, handler);
		signal(SIGINT, handler);
		SHM::SharedMemoriesManagent & mem = SHM::SharedMemoriesManagent::GetInstace();
		pid_t id = getpid();
		SHM::SharedMemory &shm = mem.GetSharedMemory("Session");
		//std::cout << id << std::endl;
		mem.ProcessAttached(id);
	}

	__attribute__((destructor))
		/**
		* It is called when dylib is being unloaded.
		*
		*/
		static void Finalizer()
	{
		
		SHM::SharedMemoriesManagent::Destory();
	}



	/* checks if the string is purely an integer
	* we can do it with `strtol' also
	*/
	int check_if_number(char *str)
	{
		int i;
		for (i = 0; str[i] != '\0'; i++)
		{
			if (!isdigit(str[i]))
			{
				return 0;
			}
		}
		return 1;
	}

#define MAX_BUF 1024
#define PID_LIST_BLOCK 32

	bool CheckProcessExist(unsigned long pid)
	{
		DIR *dirp;
		struct dirent *entry;
		char path[MAX_BUF];
		bool found = false;

		dirp = opendir("/proc/");
		if (dirp == NULL)
		{
			perror("Fail");
			return false;
		}
		while ((entry = readdir(dirp)) != NULL)
		{
			if (check_if_number(entry->d_name))
			{
				int id = atoi(entry->d_name);
				if (pid == id) {
					// std::cout

					// printf("found pid:%d\n", pid);
					found = true;
					break;
				}

			}
		}
		closedir(dirp);
		return found;
	}
//#ifdef __cplusplus
//}
//#endif
#endif

#endif
//void test_call() 
//{
//	SHM_GUARD;
//	//mut;
//	int counter = 30;
//	do 
//	{
//		std::cout << counter << std::endl;
//		Sleep(100);
//	} while (counter-->0);
//
//}