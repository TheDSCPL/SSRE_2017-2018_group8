#include <cstring>
#include "../headers/Utils.hpp"

void Utils::removeTrailingNR(std::string & s) {
    while(!s.empty() && (s[s.length()-1]=='\n' || s[s.length()-1]=='\r'))
        s.erase(s.length()-1);
}

void Utils::removeTrailingNR(char *s) {
    if(!s)
        return;
    while(s[0] && (s[strlen(s)-1]=='\n' || s[strlen(s)-1]=='\r'))
        s[strlen(s)-1]='\0';
}