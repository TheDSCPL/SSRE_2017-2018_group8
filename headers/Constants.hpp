#pragma once

namespace SSRE_CONSTANTS {
    //in milliseconds
    const static unsigned int SAMPLING = 1;
}

#define delete_reset(ptr) delete ptr; ptr = nullptr;