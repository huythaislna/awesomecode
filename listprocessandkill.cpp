//Get idea at https://github.com/JamieMJohns/Get-list-of-running-processes-and-delete-specified-processs---c-
#include <windows.h> // needed for windows API)
#include <tlhelp32.h> // for CreateToolhelp32Snapshot
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>    // sort
#include <utility> //pair
using namespace std;

vector <pair<DWORD, string>> getRunningProcess() {
	HANDLE hProcessSnap; //create handle identifier for object hprocesssnap
	HANDLE hProcess; //create handle identifier for object hprocess
	PROCESSENTRY32 pe32; //Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //take "snapshot" of processes running
	pe32.dwSize = sizeof(PROCESSENTRY32); //size of PROCESSENTRY32 [unused]
	//vector <string> process_name;
	pair<DWORD, string> process;
	vector <pair<DWORD, string>> proceses; //process id - process name
	while (Process32Next(hProcessSnap, &pe32)) {
		//char to string
		wstring ws(pe32.szExeFile);
		string process_name(ws.begin(), ws.end());

		process.first = pe32.th32ProcessID;
		process.second = process_name;
		proceses.push_back(process);
	};
	// sort by name
	sort(proceses.begin(), proceses.end(), [](auto& left, auto& right) {
		return left.second < right.second;
		});
	return proceses;
}

int main() {
	system("color A");
	while (1) {
		vector <pair<DWORD, string>> proceses = getRunningProcess();
		cout << "[+] Processes that are running:" << endl;
		cout << "\tPID \t|\tPROCESS NAME" << endl;
		for (auto process : proceses) {
			cout << "\t" << process.first << "\t|\t";
			cout << process.second << endl;;
		}
		cout << "-------------------------------------------------------" << endl;
		string name_of_process;
		//system command to kill process
		string cmmdtsk;
		//pid or name
		char option;
		cout << "[+] Process name or PID to terminate(n/p): ";
		cin >> option;
		if (option == 'n') {
			cout << "[+] Process name: ";
			cin >> name_of_process;
			cmmdtsk = "taskkill /IM " + name_of_process + " /F";
		}
		else {
			cout << "[+] PID : ";
			cin >> name_of_process;
			cmmdtsk = "taskkill /PID " + name_of_process + " /F";
		}
		system(cmmdtsk.c_str());
		cout << "[+] PRESS ENTER TO GET UPDATED LIST OF RUNNING PROCESSES." << endl;
		system("pause");
		system("cls");
	};
	return 0;
}