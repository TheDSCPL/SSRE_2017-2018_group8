#ifndef SSRE_EXITCODES_HPP
#define SSRE_EXITCODES_HPP

namespace exitcodes {
    //If either an option doesn't exist or is missing a required argument
    const static int UNKNOWN_LACKS_ARGUMENT_OPTION = 1;
    const static int CODING_ERROR_IN_OPTIONS_DEFINITION = 2;
    const static int UNEXPECTED_ERROR = 3;
}

#endif
