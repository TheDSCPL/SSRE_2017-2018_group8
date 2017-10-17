#pragma once

#include <string>

namespace Utils {
    //removes all trailing \n and \r
    void removeTrailingNR(std::string&);
    void removeTrailingNR(char*);
}
