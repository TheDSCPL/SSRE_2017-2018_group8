#include <iostream>
#include "../headers/Test.hpp"
#include "../headers/Properties.hpp"

using namespace std;

std::string Test::toString(Test::Languages l) {
    static const string _C = "C";
    static const string _Cpp = "Cpp";
    static const string _Java = "Java";
    static const string _Python = "Python";

    switch (l) {
        case Test::Languages::C:
            return _C;
        case Test::Languages::Cpp:
            return _Cpp;
        case Test::Languages::Java:
            return _Java;
        case Test::Languages::Python:
            return _Python;
    }
    return string();
}

std::string Test::getExecutable(Test::Languages l) {
    switch (l) {
        case Test::Languages::C:
            return Properties::getDefault().getProperty("SLAVES_DIRECTORY")+"C/slave";
        case Test::Languages::Cpp:
            return Properties::getDefault().getProperty("SLAVES_DIRECTORY")+"Cpp/slave";
        case Test::Languages::Java:
            return string("java -jar ") + Properties::getDefault().getProperty("SLAVES_DIRECTORY") + "Java/slave.jar";
        case Test::Languages::Python:
            return Properties::getDefault().getProperty("SLAVES_DIRECTORY")+"Python/slave.py";
    }
    throw std::invalid_argument("Unknown language!");
}

map<string, unsigned int> Test::getNRepeatsForDataSize() {
    map<string, unsigned int> ret;
    std::stringstream ss(Properties::getDefault().getProperty("DATA_SIZES"));

    string i;

    while (ss >> i)
    {
        ret.insert(make_pair(
                i,
                (unsigned int)atol( (Properties::getDefault().getProperty(string("N_REPEATS_")+i)).c_str() )));

        while (ss.peek() == ' ')
            ss.ignore();
    }

//    for(const auto & i : ret)
//        cout << i.first << ": " << i.second << endl;

    return ret;
}

void Test::createDataIfNotExists() {
    smatch m;
    static regex getNumRegex = regex(R"(\d+)");
    static regex getMultRegex = regex(R"([kKMG]$)");
    regex_search(dataSize,m,getNumRegex);
    string num=m[0];

    regex_search(dataSize,m,getMultRegex);
    string mult=m.size()?m[0]:string();
    stringstream tmp;
    tmp << "./createDataIfNotExists.sh " << num << " " << mult;
    //cout << tmp.str() << endl;
    Process p(tmp.str(),false);
    p.start();
    p.join();
}

unsigned int Test::getNRepeats() const {
    return getNRepeatsForDataSize()[dataSize];
}

Test::Test(Test::Languages language, const std::string &dataSize, unsigned int algorithm,
           unsigned int implementation, bool decrypt) :
                            language(language),
                            dataSize(dataSize),
                            algorithm(algorithm),
                            implementation(implementation),
                            decrypt(decrypt) {
    createDataIfNotExists();
}

Test::~Test() {
    clear();
    if(printable)
        free(printable);
    printable = nullptr;
}

void Test::clear() {
    delete this->p;
    this->p = nullptr;
}

void Test::test() {
    if(!this->p) {
        stringstream s;
        s << getExecutable(language) << " " << Properties::getDefault().getProperty("SLAVES_DIRECTORY") << "data" << dataSize << " " << getNRepeats() << " " << algorithm << " " << implementation << " " << decrypt;
        this->p = new Process(s.str());
        p->start();
        p->join();
    }
}

void Test::cleanTest() {
    clear();
    test();
}

std::string TestBatch::getHeader() {
    const static string ret = "Language,data_size,repetitions,algorithm,implementation,decrypt,memKbytes,cpuTime,readBytes";
    return ret;
}

TestBatch::TestBatch(Test::Languages language) : language(language) {

}

TestBatch::~TestBatch() {
    if(printable)
        free(printable);
    printable = nullptr;
}

string TestBatch::runAndExport() {
    stringstream ret;
    runAndExport(ret);
    return ret.str();
}

void TestBatch::runAndExport(std::ostream & s) {
    map <int,vector<int>> m;
    switch (language) {
        case Test::Languages::C:
            m.insert(make_pair(0,vector<int>({0,1,2,3,4,5,6})));
            m.insert(make_pair(1,vector<int>({0,1,2,3,4,5,6})));
            break;
        case Test::Languages::Cpp:
            m.insert(make_pair(0,vector<int>({0,1,2,3,4,5,6})));
            break;
        case Test::Languages::Java:
            m.insert(make_pair(0,vector<int>({0,1,2,3,4,5,6})));
            break;
        case Test::Languages::Python:
            m.insert(make_pair(0,vector<int>({0,1,4})));
            break;
    }
    for(const auto & impl : m) {
        for(const auto & alg : impl.second) {
            for(const auto & i : Test::getNRepeatsForDataSize()) {
                //ret << "Test(" << Test::toString(language)<<","<<i.first<<","<<(unsigned int)alg<<","<<(unsigned int)impl.first<<")" << endl;
                s << Test(language,i.first,(unsigned int)alg,(unsigned int)impl.first) << endl;
            }
        }
    }
}