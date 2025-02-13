#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <unistd.h>

// get current process pid
inline int GetCurrentPid()
{
    return getpid();
}

struct rusage usage;


inline float GetMemoryUsage()
{
getrusage(RUSAGE_SELF, &usage);
return usage.ru_maxrss / 1024.0;
}

inline float GetMemoryUsage(int pid)
{
getrusage(pid, &usage);
return usage.ru_maxrss / 1024.0;
}