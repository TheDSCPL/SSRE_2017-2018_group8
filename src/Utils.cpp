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

unsigned int Utils::countLines(const std::string & s) {
    unsigned int ret = s.empty()?0:1;
    for(auto c:s)
        ret+=(c=='\n'?1:0);
    return ret;
}

bool Utils::onlyDigitsAndWhiteSpace(const std::string &s) {
    for(auto ch : s)
        if(ch!='\n'&&ch!='\r'&&ch!='\t'&&ch!=' '&&!isdigit(ch))
            return false;
    return true;
}