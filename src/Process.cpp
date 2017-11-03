#include <iostream>
#include "../headers/Process.hpp"
#include "../headers/Constants.hpp"
#include "../headers/exitCodes.hpp"
#include "../headers/Utils.hpp"
#include <cstring>
#include <fstream>

using namespace std;
using namespace SSRE_CONSTANTS;

Mutex Process::newOperatorMutex;
Mutex Process::processesSetMutex;
volatile bool Process::lastCreateIsDynamic=false;
set<Process*> Process::processes;

Thread Process::watchdog([](){
//pragmas to stop the endless loop warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {
        processesSetMutex.lock();
        for(Process* p : Process::processes) {
            //cout << "In watchdog!" << endl;
            if(!p || !p->isRunning() || p->PID<10)
                continue;
            const Resources* temp = p->getInstantResources();
            if(temp)    //process is running, log resources
                p->resourcesHistory.insert(temp);
        }
        processesSetMutex.unlock();
        Thread::usleep(SAMPLING);
    }
#pragma clang diagnostic pop
}, [](){cout<<"Watchdog stopped!"<<endl;});

void Process::runWatchdog() {
    if(!Process::watchdog.isRunning())
        Process::watchdog.start();
}

Resources* Process::getInstantResources() const {
    if(!isRunning())
        return nullptr;
    return new Resources(this->PID);
}

Process::Process(const std::string & c, bool b) :
        command(c),
        monitor(b),
        outputReader([this](){
            char buf[20];
            bzero(buf,20);
            bool readANumber=false;
            //allocated here so there's no overhead of allocating memory after the process ends. Also, this maintains the object's 'end_time'=nullptr while it is running.
            struct timespec* tempEndTime = new struct timespec;
            bzero(tempEndTime, sizeof(struct timespec));
            while (fgets(buf, 20, this->out) != nullptr) {
                this->output << buf;
                Utils::removeTrailingNR(buf);
                char* _p = nullptr;
                auto tempPID = (int)strtol(buf,&_p,10);
                //tempPID>=10 is in the following if because the ampersand output(usually '1' unless the command starts background processes too) and the output of echo come unordered
                if ((tempPID>=10) && !readANumber && (int)(_p-buf)==(int)strlen(buf)){
                    this->PID = tempPID - 1;    //TODO: wtf?
                    cout << "Child process was created on PID " << this->PID << " with command = \"" << command << "\"" << endl;
                    readANumber=true;
                    startMutex.unlock();
                    //break;   //comment this to make it read all of the output
                }
            }
            //cerr << "Output: " << output.str() << endl;
            clock_gettime(CLOCK_MONOTONIC,tempEndTime);
            _finished=true;
            delete this->end_time;
            this->end_time=tempEndTime;
            tempEndTime = nullptr;
            if(this->out)//!hadErrorStarting() &&
                pclose(this->out);
            this->out = nullptr;

            if(!readANumber) {
                cout << "Child process of command \"" << command << "\" failed to start!" << endl;
                startMutex.unlock();
            }
        }, [this](){this->_finished=true;}) {
    _dynamically_allocated = Process::lastCreateIsDynamic;
    Process::lastCreateIsDynamic = false;
    Process::newOperatorMutex.unlock();
    processesSetMutex.lock();
    if(monitor)
        Process::processes.insert(this);
    processesSetMutex.unlock();
    runWatchdog();
}

Process::~Process() {
    cout << "Clearing memory " << resourcesHistory.size() << endl;
    processesSetMutex.lock();
    Process::processes.erase(this);
    processesSetMutex.unlock();
    kill();
    delete_reset(start_time);
    delete_reset(end_time);

    for(const Resources* r : resourcesHistory)
        delete(const_cast<Resources*>(r));

    resourcesHistory.clear();
    if(out) {
        pclose(out);
        out = nullptr;
    }
}

void Process::start() {
    if(hasStarted())
        return;

    startMutex.lock();

    _started = true;

    start_time = new timespec;
    bzero(start_time,sizeof(struct timespec));
    string tmp=string("( ") + command + " ) &> /dev/null & echo $!";
    //out will be closed in the destructor because 'pclose' waits for the end of the process
    out=popen(tmp.c_str(),"r");
    clock_gettime(CLOCK_MONOTONIC,start_time);

    outputReader.start();
}

void Process::kill() {
    //make sure the process isn't still starting
    startMutex.lock();
    if(isRunning()) {
        if(PID>1) {
            string cmd=string("kill ") + to_string(PID);
            cerr<<cmd<<endl;
            system(cmd.c_str());
        } else {
            cerr << "PID is -1" << endl;
        }
    }
    startMutex.unlock();
}

long int Process::getUpTime() const {
    if(start_time == nullptr)
        return 0;
    if(end_time == nullptr) {
        struct timespec now={0};
        clock_gettime(CLOCK_MONOTONIC,&now);
        return (now.tv_sec-start_time->tv_sec)*1000000000UL + now.tv_nsec - start_time->tv_nsec;
    }
    return (end_time->tv_sec - start_time->tv_sec)*1000000000UL + end_time->tv_nsec - start_time->tv_nsec;
}

bool Process::isRunning() const {
    //cerr << "isRunning(): outputReader.isRunning()=" << outputReader.isRunning() << " (hasStarted()&&!isDone())=" << (hasStarted()&&!isDone()) << endl;
    return outputReader.isRunning() || (hasStarted()&&!isDone());
}

bool Process::isDone() const {
    return _finished;
}

bool Process::isDynamic() const {
    return _dynamically_allocated;
}

bool Process::hasStarted() const {
    return _started;
}

bool Process::hadErrorStarting() const {
    return hasStarted()&&(PID<0);
}

void Process::join() const {
    if(!isRunning())
        return;
    outputReader.join();
    //cerr << "Finished thread join" << endl;
    while(!_finished) { //to wait for cleanup code after outputReader thread ends
        Thread::usleep(1);
    }
    //cerr << "Finished process join" << endl;
}

std::string Process::getOutput() const {
    return output.str();
}

const std::set<const Resources*>& Process::getResourcesHistory() const {
    return this->resourcesHistory;
}

Resources::Resources(int PID) : PID(PID), disk(0), memory(0), cpu(0){
    if(PID <= 10) { //this shouldn't be a valid PID
        cerr << "Tried to get resources from invalid PID (" << PID << ")! Resources will default to 0!" << endl;
        disk = memory = cpu = 0;
    } else {
        ifstream proc;
        //proc.open(string("/proc/") + to_string(PID) + )
    }
}