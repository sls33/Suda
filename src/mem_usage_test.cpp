#include "include/mem_usage.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <algorithm>
#include <sys/resource.h>
#include <malloc.h>
#include <vector>




    std::atomic<bool> stop_sub_thread;
int threadShowMemUsage(std::string describe) {
    stop_sub_thread=false;
    int pid = GetCurrentPid();
    float mem_usage = 0;
    while (!stop_sub_thread) {
        mem_usage = std::max(mem_usage, GetMemoryUsage(pid));
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"max mam usage of "<<describe<<"="<<mem_usage<<"MB"<<std::endl;
    }
    std::cout<<"max mam usage of "<<describe<<"="<<mem_usage<<"MB"<<std::endl;
    return 0;
}


int main1() {

    

     std::thread t(threadShowMemUsage, "test");

    // 等待一段时间
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // 强制结束线程
    stop_sub_thread = true;

    t.join();

    return 0;

}

   
int main()
{
    struct rusage usage;
    // struct mallinfo m = mallinfo();
    std::vector<std::vector<uint64_t>> x[4];
    for (int i=0; i<4; i++)
    {

getrusage(RUSAGE_SELF, &usage); // 获取当前进程资源使用情况
    

    printf("Memory usage in kilobytes:\n");
    printf("User CPU time used: %ld\n", usage.ru_utime.tv_sec);
    printf("System CPU time used: %ld\n", usage.ru_stime.tv_sec);
    printf("Maximum resident set size: %ld KB\n", usage.ru_maxrss);
    printf("Integral shared memory size: %ld KB\n", usage.ru_ixrss);
    printf("Integral unshared data size: %ld KB\n", usage.ru_idrss);
    printf("Integral unshared stack size: %ld KB\n", usage.ru_isrss);
    printf("Page reclaims: %ld\n", usage.ru_minflt);
    printf("Page faults: %ld\n", usage.ru_majflt);
    printf("Swaps: %ld\n", usage.ru_nswap);

    std::cout<<"MEM="<<GetMemoryUsage()<<std::endl;
        x[i].resize(1<<30);
    std::cout<<"---------------------"<<std::endl;
    
    }
    

    
}