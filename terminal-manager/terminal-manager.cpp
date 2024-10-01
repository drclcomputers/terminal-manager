#include<iostream>

using namespace std;

#ifdef _WIN32
#include<windows.h>
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
#else
#include<unistd.h>
#include<fstream>
#include<cstring>
#include<cstdlib>

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

#endif

int main() {
	cout << "CPU: " << get_cpu() << '\n';
	int totalmemory = totalmem();
	cout << "Memory: " << totalmemory-freemem() << '\\' << totalmemory << "MB\n";
	return 0;
}