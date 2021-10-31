#include "ifs/os.h"
#include "string.h"

#include <osconfig.h>

#ifndef _WIN32
# include <unistd.h>  // gethostname, sysconf
# include <sys/utsname.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#endif

#ifdef MacOS
#include <mach/task.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#endif


namespace fibjs
{

result_t os_base::hostname(std::string& retVal)
{
    char s[255];
    int r = gethostname(s, 255);

    if (r < 0)
        return LastError();

    retVal = s;
    return 0;
}

result_t os_base::type(std::string& retVal)
{

#ifdef _WIN32
    retVal = "Windows";
#else
    struct utsname info;

    uname(&info);
    retVal = info.sysname;
#endif

    return 0;
}

result_t os_base::release(std::string& retVal)
{
#ifdef _WIN32
    OSVERSIONINFO info = {sizeof(info)};
    char release[256];

    if (GetVersionEx(&info) == 0)
        return LastError();

    sprintf(release, "%d.%d.%d", static_cast<int>(info.dwMajorVersion),
            static_cast<int>(info.dwMinorVersion), static_cast<int>(info.dwBuildNumber));
    retVal = release;
#else
    struct utsname info;

    uname(&info);
    retVal = info.release;
#endif

    return 0;
}

result_t os_base::arch(std::string& retVal)
{

    return 0;
}

result_t os_base::CPUInfo(v8::Local<v8::Array>& retVal)
{
    v8::HandleScope scope;
    retVal = v8::Array::New();

#ifdef _WIN32

    for (int i = 0; i < 32; i++)
    {
        char key[128] = "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\";
        char processor_number[32];
        itoa(i, processor_number, 10);
        strncat(key, processor_number, 2);

        HKEY processor_key = NULL;

        DWORD cpu_speed = 0;
        DWORD cpu_speed_length = sizeof(cpu_speed);

        char cpu_brand[256];
        DWORD cpu_brand_length = sizeof(cpu_brand);

        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_QUERY_VALUE,
                         &processor_key) != ERROR_SUCCESS)
        {
            if (i == 0)
                return LastError();

            continue;
        }

        if (RegQueryValueEx(processor_key, "~MHz", NULL, NULL,
                            (LPBYTE)&cpu_speed, &cpu_speed_length)
                != ERROR_SUCCESS)
            return LastError();

        if (RegQueryValueEx(processor_key, "ProcessorNameString", NULL, NULL,
                            (LPBYTE)&cpu_brand, &cpu_brand_length)
                != ERROR_SUCCESS)
            return LastError();

        RegCloseKey(processor_key);

        v8::Local<v8::Object> times_info = v8::Object::New();
        times_info->Set(v8::String::New("user"), v8::Integer::New(0));
        times_info->Set(v8::String::New("nice"), v8::Integer::New(0));
        times_info->Set(v8::String::New("sys"), v8::Integer::New(0));
        times_info->Set(v8::String::New("idle"), v8::Integer::New(0));
        times_info->Set(v8::String::New("irq"), v8::Integer::New(0));

        v8::Local<v8::Object> cpu_info = v8::Object::New();
        cpu_info->Set(v8::String::New("model"), v8::String::New(cpu_brand));
        cpu_info->Set(v8::String::New("speed"), v8::Integer::New(cpu_speed));
        cpu_info->Set(v8::String::New("times"), times_info);
        retVal->Set(i,cpu_info);
    }

#elif defined(Linux)
    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;
    unsigned int ticks = (unsigned int)sysconf(_SC_CLK_TCK),
                         multiplier = ((uint64_t)1000L / ticks), cpuspeed;
    int numcpus = 0, i = 0;
    unsigned long long ticks_user, ticks_sys, ticks_idle, ticks_nice, ticks_intr;
    char line[512], speedPath[256], model[512];
    FILE *fpStat = fopen("/proc/stat", "r");
    FILE *fpModel = fopen("/proc/cpuinfo", "r");
    FILE *fpSpeed;

    if (fpModel)
    {
        while (fgets(line, 511, fpModel) != NULL)
        {
            if (strncmp(line, "model name", 10) == 0)
            {
                numcpus++;
                if (numcpus == 1)
                {
                    char *p = strchr(line, ':') + 2;
                    strcpy(model, p);
                    model[strlen(model)-1] = 0;
                }
            }
            else if (strncmp(line, "cpu MHz", 7) == 0)
            {
                if (numcpus == 1)
                    sscanf(line, "%*s %*s : %u", &cpuspeed);
            }
        }
        fclose(fpModel);
    }

    if (fpStat)
    {
        while (fgets(line, 511, fpStat) != NULL)
        {
            if (strncmp(line, "cpu ", 4) == 0)
                continue;
            else if (strncmp(line, "cpu", 3) != 0)
                break;

            sscanf(line, "%*s %llu %llu %llu %llu %*llu %llu",
                   &ticks_user, &ticks_nice, &ticks_sys, &ticks_idle, &ticks_intr);
            snprintf(speedPath, sizeof(speedPath),
                     "/sys/devices/system/cpu/cpu%u/cpufreq/cpuinfo_max_freq", i);

            fpSpeed = fopen(speedPath, "r");

            if (fpSpeed)
            {
                if (fgets(line, 511, fpSpeed) != NULL)
                {
                    sscanf(line, "%u", &cpuspeed);
                    cpuspeed /= 1000;
                }
                fclose(fpSpeed);
            }

            cpuinfo = v8::Object::New();
            cputimes = v8::Object::New();
            cputimes->Set(v8::String::New("user"), v8::Number::New(ticks_user * multiplier));
            cputimes->Set(v8::String::New("nice"), v8::Number::New(ticks_nice * multiplier));
            cputimes->Set(v8::String::New("sys"), v8::Number::New(ticks_sys * multiplier));
            cputimes->Set(v8::String::New("idle"), v8::Number::New(ticks_idle * multiplier));
            cputimes->Set(v8::String::New("irq"), v8::Number::New(ticks_intr * multiplier));

            cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
            cpuinfo->Set(v8::String::New("speed"), v8::Number::New(cpuspeed));

            cpuinfo->Set(v8::String::New("times"), cputimes);
            retVal->Set(i++, cpuinfo);
        }
        fclose(fpStat);
    }
#elif defined(MacOS)
    v8::Local<v8::Object> cpuinfo;
    v8::Local<v8::Object> cputimes;
    unsigned int ticks = (unsigned int)sysconf(_SC_CLK_TCK),
                         multiplier = ((uint64_t)1000L / ticks);
    char model[512];
    uint64_t cpuspeed;
    size_t size;

    size = sizeof(model);
    if (sysctlbyname("hw.model", &model, &size, NULL, 0) < 0)
        return LastError();

    size = sizeof(cpuspeed);
    if (sysctlbyname("hw.cpufrequency", &cpuspeed, &size, NULL, 0) < 0)
        return LastError();

    natural_t numcpus;
    mach_msg_type_number_t count;
    processor_cpu_load_info_data_t *info;
    if (host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &numcpus,
                            reinterpret_cast<processor_info_array_t*>(&info),
                            &count) != KERN_SUCCESS)
        return LastError();

    retVal = v8::Array::New(numcpus);
    for (unsigned int i = 0; i < numcpus; i++)
    {
        cpuinfo = v8::Object::New();
        cputimes = v8::Object::New();
        cputimes->Set(v8::String::New("user"),
                      v8::Number::New((uint64_t)(info[i].cpu_ticks[0]) * multiplier));
        cputimes->Set(v8::String::New("nice"),
                      v8::Number::New((uint64_t)(info[i].cpu_ticks[3]) * multiplier));
        cputimes->Set(v8::String::New("sys"),
                      v8::Number::New((uint64_t)(info[i].cpu_ticks[1]) * multiplier));
        cputimes->Set(v8::String::New("idle"),
                      v8::Number::New((uint64_t)(info[i].cpu_ticks[2]) * multiplier));
        cputimes->Set(v8::String::New("irq"), v8::Number::New(0));

        cpuinfo->Set(v8::String::New("model"), v8::String::New(model));
        cpuinfo->Set(v8::String::New("speed"), v8::Number::New(cpuspeed/1000000));

        cpuinfo->Set(v8::String::New("times"), cputimes);
        retVal->Set(i, cpuinfo);
    }
    vm_deallocate(mach_task_self(), (vm_address_t)info, count);


#endif

    return 0;
}

result_t os_base::networkInfo(v8::Local<v8::Array>& retVal)
{
    v8::HandleScope scope;

#ifdef _WIN32


#else
    struct ::ifaddrs *addrs, *ent;
    struct ::sockaddr_in *in4;
    struct ::sockaddr_in6 *in6;
    char ip[INET6_ADDRSTRLEN];
    v8::Local<v8::Object> ret, o;
    v8::Local<v8::String> name, ipaddr, family;

    if (getifaddrs(&addrs) != 0)
        return LastError();

    ret = v8::Object::New();

    for (ent = addrs; ent != NULL; ent = ent->ifa_next)
    {
        bzero(&ip, sizeof (ip));
        if (!(ent->ifa_flags & IFF_UP && ent->ifa_flags & IFF_RUNNING))
            continue;

        if (ent->ifa_addr == NULL)
            continue;

        if (ent->ifa_addr->sa_family != AF_INET6 && ent->ifa_addr->sa_family != AF_INET)
            continue;

        name = v8::String::New(ent->ifa_name);
        if (ret->Has(name))
        {
            retVal = v8::Local<v8::Array>::Cast(ret->Get(name));
        }
        else
        {
            retVal = v8::Array::New();
            ret->Set(name, retVal);
        }

        if (ent->ifa_addr->sa_family == AF_INET6)
        {
            in6 = (struct sockaddr_in6 *)ent->ifa_addr;
            inet_ntop(AF_INET6, &(in6->sin6_addr), ip, INET6_ADDRSTRLEN);
            family = v8::String::New("IPv6");
        }
        else if (ent->ifa_addr->sa_family == AF_INET)
        {
            in4 = (struct sockaddr_in *)ent->ifa_addr;
            inet_ntop(AF_INET, &(in4->sin_addr), ip, INET6_ADDRSTRLEN);
            family = v8::String::New("IPv4");
        }

        o = v8::Object::New();
        o->Set(v8::String::New("address"), v8::String::New(ip));
        o->Set(v8::String::New("family"), family);
        o->Set(v8::String::New("internal"), ent->ifa_flags & IFF_LOOPBACK ?
               v8::True() : v8::False());

        retVal->Set(retVal->Length(), o);

    }

    freeifaddrs(addrs);
#endif

    return 0;
}


}
