#include <iostream>
#include "../headers/Process.hpp"
#include "../headers/Constants.hpp"
#include "../headers/exitCodes.hpp"
#include "../headers/Utils.hpp"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <wait.h>

using namespace std;
using namespace SSRE_CONSTANTS;

vector<Process*> Process::processes;

Thread Process::watchdog([](){
//pragmas to stop the endless loop warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while(true) {
        for(Process* p : Process::processes) {
            cerr << "In watchdog!" << endl;
            if(!p || !p->isRunning())
                continue;
            /*const Resources* temp = p->getInstantResources();
            if(temp)    //process is running, log resources
                p->resourcesHistory.push_back(temp);*/
        }
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

Process::Process(const std::string & c) :
        command(c),
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
                if (!readANumber && (int)(_p-buf)==(int)strlen(buf)){
                    cout << "Child process was created on PID " << tempPID << " with command = \"" << command << "\"" << endl;
                    this->PID = tempPID;
                    readANumber=true;
                }
            }
            clock_gettime(CLOCK_MONOTONIC,tempEndTime);
            if(this->end_time)
                delete this->end_time;
            this->end_time=tempEndTime;
            tempEndTime = nullptr;
            if(this->out)
                pclose(this->out);
            this->out = nullptr;
            if(!readANumber)
                cout << "Child process of command \"" << command << "\" failed to start!" << endl;
        }) {
    runWatchdog();
}

//TODO: aqui
Process::~Process() {
    cerr << "Clearing memory " << resourcesHistory.size() << endl;
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
    start_time = new timespec;
    bzero(start_time,sizeof(struct timespec));
    string tmp=command + " &> /dev/null & echo $!";
    //out will be closed in the destructor because 'pclose' waits for the end of the process
    out=popen(tmp.c_str(),"r");
    clock_gettime(CLOCK_MONOTONIC,start_time);

    outputReader.start();
}

void Process::kill() {
    if(isRunning())
        system((string("kill ") + to_string(PID)).c_str());
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
    //TODO: change this to actually check if it is running
    return outputReader.isRunning();
}

bool Process::isDone() const {
    return start_time&&end_time;
}

bool Process::hasStarted() const {
    return start_time!=nullptr;
}

bool Process::hadErrorStarting() const {
    return start_time&&(PID<0);
}

void Process::join() const {
    if(!isRunning())
        return;
    outputReader.join();
}

std::string Process::getOutput() const {
    return output.str();
}

const std::vector<const Resources*>& Process::getResourcesHistory() const {
    return this->resourcesHistory;
}

Resources::Resources(int PID) : PID(0), disk(0), memory(0), cpu(0){
    //TODO:
}