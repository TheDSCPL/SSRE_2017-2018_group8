#ifndef SSRE_PROCESS_HPP
#define SSRE_PROCESS_HPP
#include <sstream>
#include <set>
#include <regex>

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

    mutable Mutex startMutex;   //locked when start is called and unlocked when PID is found

    //Caller should delete the pointer after use
    //Returns nullptr if the process is not running;
    std::string getIOresources() const;
    const std::string command;
    const bool monitor; //if this process should have its resources monitored
    std::stringstream output;

    std::string lastIOread;
    Thread outputReader;
    //attempts to start the process. Here to avoid the sparse error of PID as 1;

public:
    std::string _getResources() const;

public:
    //'command' shouldn't have an ampersign (&) at the end!
    Process(const std::string&, bool monitor = true);
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
    Resources getResourses() const;

    void *operator new(size_t size){
        Process::newOperatorMutex.lock();
        Process::lastCreateIsDynamic = true;
        return ::operator new(size);
    }
};

class Resources {
    friend class Process;
    unsigned int cpuTime, readBytes, memKbytes; //cpuTime in milliseconds
    std::string printable;

    const static std::regex cpuTimeR, readBytesR, memKbytesR;
public:
    Resources(std::string s);
    unsigned int getCpuTime() const;
    unsigned int getReadBytes() const;
    unsigned int getMemKbytes() const;

    operator const char *() const {
        return printable.c_str();
    }
};

#endif
