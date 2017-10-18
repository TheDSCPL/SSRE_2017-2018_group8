#pragma once

namespace SSRE_CONSTANTS {
    //in milliseconds
    const static unsigned int SAMPLING = 1;
    const static unsigned int MAX_START_PROCESS_ATTEMPTS = 3;
}

#define delete_reset(ptr) delete ptr; ptr = nullptr;