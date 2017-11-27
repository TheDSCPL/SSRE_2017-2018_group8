#ifndef SSRE_TEST_HPP
#define SSRE_TEST_HPP

#include <exception>
#include <map>

#include "Process.hpp"
#include "Properties.hpp"

class TestBatch;

class Test {
    friend class TestBatch;
public:
    enum class Languages {
        C,
        Cpp,
        Java,
        Python
    };

private:
    static std::string toString(Test::Languages l);
    static std::string getExecutable(Test::Languages l);
    static std::map<std::string, unsigned int> getNRepeatsForDataSize();

    void createDataIfNotExists();
    unsigned int getNRepeats() const;

    Process* p=nullptr;
    mutable char* printable=nullptr;
    Test::Languages language;
    const std::string dataSize;
    unsigned int algorithm, implementation;
    bool decrypt;

public:
    Test(Test const&)            = delete;
    void operator=(Test const&)  = delete;


    Test(Test::Languages language, const std::string &dataSize, unsigned int algorithm,
             unsigned int implementation, bool decrypt=false);
    ~Test();
    void clear();
    //the caller should delete this object
    void test();
    void cleanTest();

    operator const char *() {
        test();
        std::stringstream s;
        s << Test::toString(language).c_str() << "," << dataSize << "," << getNRepeats() << "," << algorithm << "," << implementation << "," << decrypt << "," << p->getResourses();
        if(printable)
            free(printable);
        printable = nullptr;
        printable = strdup(s.str().c_str());
        return printable;
    }
};

class TestBatch {
    const Test::Languages language;
    char* printable = nullptr;

public:
    static std::string getHeader();

    TestBatch(Test::Languages language);
    ~TestBatch();
    std::string runAndExport();
    void runAndExport(std::ostream &);

    operator const char *() {
        if(printable)
            free(printable);
        printable = nullptr;
        printable = strdup(runAndExport().c_str());
        return printable;
    }
};

#endif
