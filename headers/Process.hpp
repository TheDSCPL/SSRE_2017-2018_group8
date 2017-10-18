#ifndef SSRE_PROCESS_HPP
#define SSRE_PROCESS_HPP
#include <sstream>
#include <set>

#include "Thread.hpp"

class Resources;

class Process{
    static Mutex newOperatorMutex, processesSetMutex;
    static volatile bool lastCreateIsDynamic;
    static Thread watchdog;
    static std::set<Process*> processes;
    static void runWatchdog();

    int PID=-1;
    struct timespec *start_time=nullptr, *end_time=nullptr;
    FILE* out = nullptr;
    volatile bool _started=false, _finished=false, _dynamically_allocated=false; //to prevent join to immediately leave after an immediate subsequent call to start because the thread hasn't started yet.

    std::vector<const Resources*> resourcesHistory;
    mutable Mutex startMutex;   //locked when start is called and unlocked when PID is found

    //Caller should delete the pointer after use
    //Returns nullptr if the process is not running;
    Resources* getInstantResources() const;
    const std::string command;
    std::stringstream output;
    Thread outputReader;
    //attempts to start the process. Here to avoid the sparse error of PID as 1;

public:
    //'command' shouldn't have an ampersign (&) at the end!
    Process(const std::string&);
    ~Process();
    void start();
    void kill();

    //returns elapsed time of the process in nanoseconds
    long int getUpTime() const;
    bool isRunning() const;
    bool isDone() const;
    bool isDynamic() const;
    bool hasStarted() const;
    bool hadErrorStarting() const;
    void join() const;
    std::string getOutput() const;

    const std::vector<const Resources*>& getResourcesHistory() const;

    void *operator new(size_t size){
        Process::newOperatorMutex.lock();
        Process::lastCreateIsDynamic = true;
        return ::operator new(size);
    }
};

class Resources{
    int PID, disk, memory, cpu;
public:
    explicit Resources(int PID);
    //Resources(int PID, int disk, int memory, int cpu);
    //~Resources();
    const int & getPID() const;
    const int & getDisk() const;
    const int & getMemory() const;
    const int & getCPU() const;
};

#endif
