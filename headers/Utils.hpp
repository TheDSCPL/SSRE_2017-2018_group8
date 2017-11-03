#pragma once

#include <string>

#define delete_reset(ptr) delete ptr; ptr = nullptr;

namespace Utils {
    //removes all trailing \n and \r
    void removeTrailingNR(std::string&);
    void removeTrailingNR(char*);
}
