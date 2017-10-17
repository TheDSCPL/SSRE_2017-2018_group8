#ifndef SSRE_PROCESS_HPP
#define SSRE_PROCESS_HPP
#include <vector>
#include <sstream>

#include "Thread.hpp"

class Resources;

class Process{
    static Mutex mutex;
    static Thread watchdog;
    static std::vector<Process*> processes;
    static void runWatchdog();

    int PID=-1;
    struct timespec *start_time=nullptr, *end_time=nullptr;
    FILE* out = nullptr;

    std::vector<const Resources*> resourcesHistory;

    //Caller should delete the pointer after use
    //Returns nullptr if the process is not running;
    Resources* getInstantResources() const;
    const std::string command;
    std::stringstream output;
    Thread outputReader;

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
    bool hasStarted() const;
    bool hadErrorStarting() const;
    void join() const;
    std::string getOutput() const;

    const std::vector<const Resources*>& getResourcesHistory() const;
};

class Resources{
    friend class Process;
public:
    explicit Resources(int PID);
    Resources(int PID, int disk, int memory, int cpu);
    //~Resources();
    const int PID, disk, memory, cpu;
};

#endif
