/*
 * os_win.cpp
 *
 *  Created on: Nov 11, 2012
 *      Author: lion
 */


#include <osconfig.h>

#ifdef _WIN32

#include "ifs/os.h"
#include <iphlpapi.h>
#include <psapi.h>
#include "utf8.h"
#include "inetAddr.h"

typedef struct
_SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
    LARGE_INTEGER IdleTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER DpcTime;
    LARGE_INTEGER InterruptTime;
    ULONG Reserved2;
} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
static PROCNTQSI pNtQuerySystemInformation;

#define SystemBasicInformation 0
#define SystemPerformanceInformation 2
#define SystemTimeInformation 3
#define SystemProcessorPerformanceInformation 8

namespace fibjs
{

result_t os_base::get_type(std::string& retVal)
{
	retVal = "Windows";
	return 0;
}

result_t os_base::get_version(std::string& retVal)
{
	OSVERSIONINFO info =
	{	sizeof(info)};
	char release[256];

	if (GetVersionEx(&info) == 0)
		return LastError();

	sprintf(release, "%d.%d.%d", static_cast<int>(info.dwMajorVersion),
			static_cast<int>(info.dwMinorVersion), static_cast<int>(info.dwBuildNumber));
	retVal = release;

	return 0;
}

result_t os_base::uptime(double& retVal)
{
	BYTE stack_buffer[4096];
	BYTE* malloced_buffer = NULL;
	BYTE* buffer = (BYTE*) stack_buffer;
	size_t buffer_size = sizeof(stack_buffer);
	DWORD data_size;

	PERF_DATA_BLOCK* data_block;
	PERF_OBJECT_TYPE* object_type;
	PERF_COUNTER_DEFINITION* counter_definition;

	DWORD i;

	for (;;)
	{
		LONG result;

		data_size = (DWORD) buffer_size;
		result = RegQueryValueExW(HKEY_PERFORMANCE_DATA, L"2", NULL, NULL,
				buffer, &data_size);
		if (result == ERROR_SUCCESS)
		{
			break;
		}
		else if (result != ERROR_MORE_DATA)
			return LastError();

		free(malloced_buffer);

		buffer_size *= 2;
		/* Don't let the buffer grow infinitely. */
		if (buffer_size > 1 << 20)
		{
			goto internalError;
		}

		buffer = malloced_buffer = (BYTE*) malloc(buffer_size);
		if (malloced_buffer == NULL)
			return LastError();
	}

	if (data_size < sizeof(*data_block))
		goto internalError;

	data_block = (PERF_DATA_BLOCK*) buffer;

	if (wmemcmp(data_block->Signature, L"PERF", 4) != 0)
		goto internalError;

	if (data_size < data_block->HeaderLength + sizeof(*object_type))
		goto internalError;

	object_type = (PERF_OBJECT_TYPE*) (buffer + data_block->HeaderLength);

	if (object_type->NumInstances != PERF_NO_INSTANCES)
		goto internalError;

	counter_definition = (PERF_COUNTER_DEFINITION*) (buffer
			+ data_block->HeaderLength + object_type->HeaderLength);
	for (i = 0; i < object_type->NumCounters; i++)
	{
		if ((BYTE*) counter_definition + sizeof(*counter_definition)
				> buffer + data_size)
		{
			break;
		}

		if (counter_definition->CounterNameTitleIndex == 674
				&& counter_definition->CounterSize == sizeof(uint64_t))
		{
			if (counter_definition->CounterOffset + sizeof(uint64_t) > data_size
					|| !(counter_definition->CounterType & PERF_OBJECT_TIMER))
			{
				goto internalError;
			}
			else
			{
				BYTE* address = (BYTE*) object_type
						+ object_type->DefinitionLength
						+ counter_definition->CounterOffset;
				uint64_t value = *((uint64_t*) address);
				retVal = (double) (object_type->PerfTime.QuadPart - value)
						/ (double) object_type->PerfFreq.QuadPart;
				free(malloced_buffer);
				return 0;
			}
		}

		counter_definition =
				(PERF_COUNTER_DEFINITION*) ((BYTE*) counter_definition
						+ counter_definition->ByteLength);
	}

	/* If we get here, the uptime value was not found. */
	free(malloced_buffer);
	return LastError();

	internalError: free(malloced_buffer);
	return LastError();
}

result_t os_base::loadavg(v8::Handle<v8::Array>& retVal)
{
	double avg[3] = {0, 0, 0};

	retVal = v8::Array::New(3);
	retVal->Set(0, v8::Number::New(avg[0]));
	retVal->Set(1, v8::Number::New(avg[1]));
	retVal->Set(2, v8::Number::New(avg[2]));

	return 0;
}

result_t os_base::totalmem(int64_t& retVal)
{
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	if(!GlobalMemoryStatusEx(&memory_status))
		return LastError();

	retVal = memory_status.ullTotalPhys;
	return 0;
}

result_t os_base::freemem(int64_t& retVal)
{
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);

	if(!GlobalMemoryStatusEx(&memory_status))
		return LastError();

	retVal = memory_status.ullAvailPhys;
	return 0;
}

result_t os_base::CPUs(int32_t& retVal)
{
	static int cpus = 0;

	if (cpus > 0)
	{
		retVal = cpus;
		return 0;
	}

	SYSTEM_INFO system_info;

	GetSystemInfo(&system_info);
	cpus = system_info.dwNumberOfProcessors;

	retVal = cpus;

	return 0;
}

result_t os_base::CPUInfo(v8::Handle<v8::Array>& retVal)
{
	retVal = v8::Array::New();

	SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION* sppi;
	DWORD sppi_size;
	SYSTEM_INFO system_info;
	DWORD cpu_count, i, r;
	ULONG result_size;
	result_t hr = 0;

	if (pNtQuerySystemInformation == NULL)
		pNtQuerySystemInformation = (PROCNTQSI) GetProcAddress(
				GetModuleHandle("ntdll"), "NtQuerySystemInformation");

	if (pNtQuerySystemInformation == NULL)
		return LastError();

	GetSystemInfo(&system_info);
	cpu_count = system_info.dwNumberOfProcessors;

	sppi_size = sizeof(*sppi) * cpu_count;
	sppi = (SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION*) malloc(sppi_size);
	if (!sppi)
		return LastError();

	r = pNtQuerySystemInformation(SystemProcessorPerformanceInformation, sppi,
			sppi_size, &result_size);
	if (r != ERROR_SUCCESS || result_size != sppi_size)
	{
		hr = LastError();
		free(sppi);
		return hr;
	}

	for (i = 0; i < cpu_count; i++)
	{
		WCHAR key_name[128];
		HKEY processor_key;
		DWORD cpu_speed;
		DWORD cpu_speed_size = sizeof(cpu_speed);
		char cpu_brand[256];
		DWORD cpu_brand_size = sizeof(cpu_brand);

		_snwprintf(key_name, sizeof(key_name) / sizeof(key_name[0]),
				L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);

		r = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key_name, 0, KEY_QUERY_VALUE,
				&processor_key);
		if (r != ERROR_SUCCESS)
		{
			hr = LastError();
			free(sppi);
			return hr;
		}

		if (RegQueryValueExW(processor_key, L"~MHz", NULL, NULL,
				(BYTE*) &cpu_speed, &cpu_speed_size) != ERROR_SUCCESS)
		{
			hr = LastError();
			RegCloseKey(processor_key);
			free(sppi);
			return hr;
		}

		if (RegQueryValueExA(processor_key, "ProcessorNameString", NULL, NULL,
				(BYTE*) &cpu_brand, &cpu_brand_size) != ERROR_SUCCESS)
		{
			hr = LastError();
			RegCloseKey(processor_key);
			free(sppi);
			return hr;
		}

		RegCloseKey(processor_key);

		v8::Local<v8::Object> times_info = v8::Object::New();
		times_info->Set(v8::String::New("user"),
				v8::Integer::New(
						(int32_t) (sppi[i].UserTime.QuadPart / 10000)));
		times_info->Set(v8::String::New("nice"), v8::Integer::New(0));
		times_info->Set(v8::String::New("sys"),
				v8::Integer::New(
						(int32_t) (sppi[i].KernelTime.QuadPart
								- sppi[i].IdleTime.QuadPart) / 10000));
		times_info->Set(v8::String::New("idle"),
				v8::Integer::New(
						(int32_t) (sppi[i].IdleTime.QuadPart / 10000)));
		times_info->Set(v8::String::New("irq"),
				v8::Integer::New(
						(int32_t) (sppi[i].InterruptTime.QuadPart / 10000)));

		v8::Local<v8::Object> cpu_info = v8::Object::New();
		cpu_info->Set(v8::String::New("model"), v8::String::New(cpu_brand));
		cpu_info->Set(v8::String::New("speed"), v8::Integer::New(cpu_speed));
		cpu_info->Set(v8::String::New("times"), times_info);
		retVal->Set(i, cpu_info);
	}

	return hr;
}

result_t os_base::networkInfo(v8::Handle<v8::Object>& retVal)
{
	unsigned long size = 0;
	IP_ADAPTER_ADDRESSES* adapter_addresses;
	IP_ADAPTER_ADDRESSES* adapter_address;
	IP_ADAPTER_UNICAST_ADDRESS_XP* unicast_address;

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, NULL, &size)
			!= ERROR_BUFFER_OVERFLOW)
		return LastError();

	adapter_addresses = (IP_ADAPTER_ADDRESSES*) malloc(size);
	if (!adapter_addresses)
		return LastError();

	if (GetAdaptersAddresses(AF_UNSPEC, 0, NULL, adapter_addresses, &size)
			!= ERROR_SUCCESS)
		return LastError();

	retVal = v8::Object::New();

	for (adapter_address = adapter_addresses; adapter_address != NULL;
			adapter_address = adapter_address->Next)
	{
		v8::Local<v8::Array> ret;
		v8::Local<v8::Object> o;
		v8::Local<v8::String> name, ipaddr, family;

		if (adapter_address->OperStatus != IfOperStatusUp)
			continue;

		name = v8::String::New((const uint16_t*) adapter_address->FriendlyName);

		ret = v8::Array::New();
		retVal->Set(name, ret);

		unicast_address =
				(IP_ADAPTER_UNICAST_ADDRESS_XP*) adapter_address->FirstUnicastAddress;
		while (unicast_address)
		{
			inetAddr* sock_addr = (inetAddr*) unicast_address->Address.lpSockaddr;

			o = v8::Object::New();
			o->Set(v8::String::New("address"), v8::String::New(sock_addr->str().c_str()));
			o->Set(v8::String::New("family"), sock_addr->family() == net_base::_AF_INET6 ?
					v8::String::New("IPv6") : v8::String::New("IPv4"));
			o->Set(v8::String::New("internal"),
					adapter_address->IfType == IF_TYPE_SOFTWARE_LOOPBACK ?
							v8::True() : v8::False());

			ret->Set(ret->Length(), o);

			unicast_address = unicast_address->Next;
		}
	}

	free(adapter_addresses);

	return 0;
}

result_t os_base::get_execPath(std::string& retVal)
{
	WCHAR szFileName[MAX_PATH];

	GetModuleFileNameW(NULL, szFileName, MAX_PATH);
	retVal = utf16to8String(szFileName);
	return 0;
}

result_t os_base::memoryUsage(v8::Handle<v8::Object>& retVal)
{
	size_t rss = 0;

	HANDLE current_process;
	PROCESS_MEMORY_COUNTERS pmc;

	current_process = GetCurrentProcess();

	if (!GetProcessMemoryInfo(current_process, &pmc, sizeof(pmc)))
		return LastError();

	rss = pmc.WorkingSetSize;

	v8::Handle<v8::Object> info = v8::Object::New();

	v8::HeapStatistics v8_heap_stats;
	v8::Isolate::GetCurrent()->GetHeapStatistics(&v8_heap_stats);
	info->Set(v8::String::New("rss"), v8::Integer::New((int32_t)rss));
	info->Set(v8::String::New("heapTotal"),
			v8::Integer::New((int32_t)v8_heap_stats.total_heap_size()));
	info->Set(v8::String::New("heapUsed"),
			v8::Integer::New((int32_t)v8_heap_stats.used_heap_size()));

	retVal = info;

	return 0;
}

}

#endif
