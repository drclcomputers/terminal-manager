#include<iostream>

using namespace std;

#ifdef _WIN32
#include<windows.h>
#include <psapi.h>
void wait(int n) {
	Sleep(n);
}

void clear() {
	system("cls");
}

float get_cpu() {
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	static ULONGLONG prev_idle = 0, prev_total = 0;
	ULONGLONG idle = *((ULONGLONG*)&idleTime);
	ULONGLONG total = *((ULONGLONG*)&kernelTime) + *((ULONGLONG*)&userTime);

	float usage = (float)(total - prev_total - (idle - prev_idle)) / (total - prev_total) * 100;

	prev_idle = idle;
	prev_total = total;

	return usage;
}

int freemem() {
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);
	return memStatus.ullAvailPhys / (1024 * 1024);
}

int totalmem() {
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	GlobalMemoryStatusEx(&memStatus);
	return memStatus.ullTotalPhys / (1024 * 1024);
}

int totalstorage(wchar_t* drive) {
	ULARGE_INTEGER freeBytesAvailableToUser;
	ULARGE_INTEGER totalNumberOfBytes;
	ULARGE_INTEGER totalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(drive, &freeBytesAvailableToUser, &totalNumberOfBytes, &totalNumberOfFreeBytes);
	return totalNumberOfBytes.QuadPart/1024/1024/1024;
}

int freestorage(wchar_t* drive) {
	ULARGE_INTEGER freeBytesAvailableToUser;
	ULARGE_INTEGER totalNumberOfBytes;
	ULARGE_INTEGER totalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(drive, &freeBytesAvailableToUser, &totalNumberOfBytes, &totalNumberOfFreeBytes);
	return totalNumberOfFreeBytes.QuadPart / 1024 / 1024/1024;
}

void listdrives() {
	cout << "Storage (used/total):\n";
	wchar_t buffer[10000];
	DWORD result = GetLogicalDriveStrings(sizeof(buffer), buffer);
	for (wchar_t* drive = buffer; *drive != '\0'; drive += 4) {
		int totsto = totalstorage(drive);
		wcout << drive << " : " << totsto-freestorage(drive) << "/" << totsto << "GB\n";
	}
}

int get_nr_processes() {
	DWORD processIds[1024], nr, cb;
	EnumProcesses(processIds, sizeof(processIds), &cb);
	nr = cb / sizeof(DWORD);
	return nr;
}

#else
#include<unistd.h>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include <sys/statvfs.h>
#include<pwd.h>

void wait(int n) {
	sleep(n/1000);
}

void clear() {
	system("clear");
}

float get_cpu() {
	ifstream cpufile("/proc/stat");
	unsigned long long cpu1, cpu2, cpu3, cpu4;
	char cpuarray[100000];
	cpufile.getline(cpuarray, 10000);
	cpufile.close();
	char* p = strtok(cpuarray, " ");
	p = strtok(NULL, " ");
	cpu1 = atoi(p);
	p = strtok(NULL, " ");
	cpu2 = atoi(p);
	p = strtok(NULL, " ");
	cpu3 = atoi(p);
	p = strtok(NULL, " ");
	cpu4 = atoi(p);

	static unsigned long long prev_idle = 0, prev_total = 0;
	unsigned long long total = cpu1 + cpu2 + cpu3 + cpu4;
	float usage = 1.0f - (float)(cpu4 - prev_idle) / (total - prev_total);

	prev_idle = cpu4;
	prev_total = total;

	return usage * 100;
}

int freemem() {
	ifstream memfile("/proc/meminfo");
	unsigned long long free;
	char memarray[1000];
	memfile.getline(memarray, 1000);
	memfile.getline(memarray, 1000);
	char* p = strtok(memarray, " ");
	p = strtok(NULL, " ");
	free = atoi(p);
	return free/1024;
}

int totalmem() {
	ifstream memfile("/proc/meminfo");
	unsigned long long total;
	char memarray[1000];
	memfile.getline(memarray, 1000);
	char* p = strtok(memarray, " ");
	p = strtok(NULL, " ");
	total = atoi(p);
	return total / 1024;
}

void listdrives() {
	struct statvfs buffer;
	statvfs("/", &buffer);
	unsigned long total = buffer.f_frsize * buffer.f_blocks/1024/1024/1024;
	unsigned long free = buffer.f_frsize * buffer.f_bfree / 1024 / 1024 / 1024;
	cout << "Storage (used/total) : " << total - free << '/' << total << "GB\n";
}

int get_nr_processes() {
	int nr = 0;
	struct passwd* pw;
	setpwent();
	while ((pw = getpwent()) != nullptr)
		nr++;
	endpwent();
	return nr;
}

#endif

int main(int argc, char *argv[]) {
	if (argc == 1) {
		while (true) {
			clear();
			cout << "CPU (usage): " << get_cpu() << "%\n";
			int totalmemory = totalmem();
			cout << "Memory (used/total): " << totalmemory - freemem() << '\\' << totalmemory << "MB\n";
			listdrives();
			cout << "Processes : " << get_nr_processes() << "\n";
			wait(1000);
		}
	}
	else {
		if (argc == 2) {
			if (strcmp(argv[1], "-static") == 0) {
				cout << "CPU (usage): " << get_cpu() << "%\n";
				int totalmemory = totalmem();
				cout << "Memory (used/total): " << totalmemory - freemem() << '\\' << totalmemory << "MB\n";
				listdrives();
				cout << "Processes : " << get_nr_processes() << "\n";
			}
			else if (strcmp(argv[1], "-slow") == 0) {
				while (true) {
					clear();
					cout << "CPU (usage): " << get_cpu() << "%\n";
					int totalmemory = totalmem();
					cout << "Memory (used/total): " << totalmemory - freemem() << '\\' << totalmemory << "MB\n";
					listdrives();
					cout << "Processes : " << get_nr_processes() << "\n";
					wait(3000);
				}
			}
			else if (strcmp(argv[1], "-cpu") == 0) {
				while (true) {
					clear();
					cout << "CPU (usage): " << get_cpu() << "%\n";
					wait(1000);
				}
			}
			else if (strcmp(argv[1], "-ram") == 0) {
				while (true) {
					clear();
					int totalmemory = totalmem();
					cout << "Memory (used/total): " << totalmemory - freemem() << '\\' << totalmemory << "MB\n";
					wait(1000);
				}
			}
			else if (strcmp(argv[1], "-storage") == 0) {
				listdrives();
			}
			else if (strcmp(argv[1], "-processes") == 0) {
				while (true) {
					clear();
					cout << "Processes : " << get_nr_processes() << "\n";
					wait(1000);
				}
			}
			else {
				cout << "Unknoun parameter passed!\n";
			}
		}
		else {
			if (strcmp(argv[1], "-cpu") == 0 && strcmp(argv[2], "-static") == 0) {
				cout << "CPU (usage): " << get_cpu() << "%\n";
			}
			else if (strcmp(argv[1], "-ram") == 0 && strcmp(argv[2], "-static") == 0) {
				int totalmemory = totalmem();
				cout << "Memory (used/total): " << totalmemory - freemem() << '\\' << totalmemory << "MB\n";
			}
			else if (strcmp(argv[1], "-processes") == 0 && strcmp(argv[2], "-static") == 0) {
				cout << "Processes : " << get_nr_processes() << "\n";
			}
			else {
				cout << "One of parameters passed is unknoun!\n";
			}
		}
	}
	return 0;
}