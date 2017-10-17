#include "../headers/Options.hpp"

#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include "../headers/exitCodes.hpp"

using namespace std;

//----------------------OptionAbstract----------------------//

int dummy;
function<void()> OptionAbstract::EMPTY_FUNCTION = [](){};
const HasArgument OptionAbstract::DEFAULT_HAS_ARGUMENT = HasArgument::NO_ARGUMENT;

OptionAbstract::OptionAbstract(const int &shortForm,
                               const string &longForm,
                               const HasArgument &hasArgument,
                               const function<void()> &action) :
        _hasArgument(hasArgument),
        longForm(longForm),
        shortForm(shortForm),
        action(action) {
    if(!(hasLongForm() || hasShortForm())) {
        printf("All options should have a long form or (inclusive or) a short form. Exiting...");
        exit(exitcodes::CODING_ERROR_IN_OPTIONS_DEFINITION);
    }
}

OptionAbstract &OptionAbstract::setFlag(int *flag, int val) {
    this->flag = flag;
    this->flagValue = val;
    return *this;
}

bool OptionAbstract::hasLongForm() const {
    return !getLongForm().empty();
}

const string & OptionAbstract::getLongForm() const {
    return longForm;
}

bool OptionAbstract::hasShortForm() const {
    return getShortForm() != 0;
}

const int & OptionAbstract::getShortForm() const {
    return shortForm;
}

const HasArgument & OptionAbstract::getHasArgument() const {
    return _hasArgument;
}

bool OptionAbstract::isNoArgument() const {
    return _hasArgument==HasArgument::NO_ARGUMENT;
}

bool OptionAbstract::isRequiredArgument() const {
    return _hasArgument==HasArgument::REQUIRED_ARGUMENT;
}

bool OptionAbstract::isOptionalArgument() const {
    return _hasArgument == HasArgument::OPTIONAL_ARGUMENT;
}

//----------------------Option----------------------//

string Option::makePrintableForm() const {
    stringstream ret;
    ret << "Long form: " << (hasLongForm() ? ("'" + getLongForm() + "'") : "<doesn't have>") << endl;
    ret << "Short form: " << (hasShortForm() ? (string("'") + (char)getShortForm() + "'") : "<doesn't have>") << endl;
    ret << "Argument: " << (hasArgument() ? ("'" + getArgument() + "'") : "<doesn't have>") << endl;
    return ret.str();
}

Option::Option(const OptionAbstract &optionTemplate, const string &argument) :
        OptionAbstract(optionTemplate),
        argument(argument),
        printableForm(makePrintableForm()) {}

bool Option::hasArgument() const {
    return !argument.empty();
}

const string& Option::getArgument() const {
    return argument;
}

void Option::doAction() const {
    if(flag != nullptr)
        *flag = flagValue;
    action();
}

//----------------------Args----------------------//

Args::Args(int argc, char **argv, const vector<OptionAbstract> &opts) {
    int c, prev_optind = optind, prev_opterr = opterr;

    opterr = 1;
    optind = 1;

    options.clear();

    struct option* longOptions  = makeLongOptions (opts);
    char* shortOptions          = makeShortOptions(opts);

    //cout << shortOptions << endl;

    /*for(const auto & e : opts) {
        cout << "Short:" << (char)e.getShortForm() << " ";
        cout << "Long:" << e.getLongForm() << endl;
    }*/

    while(true) {
        int option_index = 0;

        c=getopt_long(argc, argv, shortOptions, longOptions, &option_index);

        OptionAbstract const* currentOptionAbstract=nullptr;

        if(c==-1)
            break;

        else if(c == 0) {   //option by longForm
            string longFormName = (longOptions[option_index].name);
            for(const auto& it : opts)
                if(it.hasLongForm() && it.getLongForm()==longFormName) {
                    currentOptionAbstract = &it;
                    break;
                }
        }

        else if(c == '?' || c == ':') {
            //cerr << "Option " << (char)optopt << " not recognized! " << (optarg?strlen(optarg):0) << "optopt: " << (char)c << endl;
            exit(exitcodes::UNKNOWN_LACKS_ARGUMENT_OPTION);
        }

        else {  //option by shortForm
            for(const auto & it : opts)
                if(it.hasShortForm() && it.getShortForm()==c) {
                    currentOptionAbstract = &it;
                    break;
                }
        }

        if(currentOptionAbstract == nullptr)
            exit(exitcodes::UNEXPECTED_ERROR);

        //Now that I have the option template, create the Option pointer for the return
        const Option* ___temp = new Option(*currentOptionAbstract, optarg == nullptr ? "" : optarg);
        options.push_back(___temp);
        ___temp->doAction();
    }

    args.clear();

    while(optind < argc)
        args.emplace_back(argv[optind++]);

    if(longOptions)
        delete[] longOptions;
    if(shortOptions)
        free(shortOptions);

    optind = prev_optind;
    opterr = prev_opterr;
}

Args::~Args() {
    for(auto it : options)
        delete it;
    options.clear();
}

unsigned int Args::countLongOptions(const vector<OptionAbstract> &opts) {
    unsigned int counter = 0;
    for (const auto& it : opts)
        if(it.hasLongForm())
            counter++;
    return counter;
}

struct option * Args::makeLongOptions(const vector<OptionAbstract> &opts) {
    unsigned int longOptionsCount = countLongOptions(opts);
    struct option* ret = new struct option[longOptionsCount+1];
    for (unsigned int i = 0, longOptIndex = 0; i < opts.size() && longOptIndex < longOptionsCount; i++) {
        if (!opts[i].hasLongForm())
            continue;

        struct option temp;

        temp.name = opts[i].getLongForm().c_str();
        switch (opts[i].getHasArgument()) {
            case HasArgument::NO_ARGUMENT:
                temp.has_arg = no_argument;
                break;
            case HasArgument::OPTIONAL_ARGUMENT:
                temp.has_arg = optional_argument;
                break;
            case HasArgument::REQUIRED_ARGUMENT:
                temp.has_arg = required_argument;
        }
        temp.flag   = nullptr;
        temp.val    = opts[i].hasShortForm() ? opts[i].getShortForm() : 0;

        ret[longOptIndex++] = temp;
    }
    ret[longOptionsCount] = {0};
    return ret;
}

char *Args::makeShortOptions(const vector<OptionAbstract> &opts) {
    stringstream ret;
    //ret << ':';
    for(auto it : opts) {
        if(!it.hasShortForm())
            continue;
        ret << (char)it.getShortForm() << (it.isNoArgument() ? "" : it.isRequiredArgument() ? ":" : "::") ;
    }
    return strdup(ret.str().c_str());
}

const Args & Args::getArgs(int argc, char **argv, vector<OptionAbstract> opts) {
    const static Args instance(argc,argv, opts);

    return instance;
}

const std::vector<Option const *> &Args::getUsedOptions() const {
    return options;
}

const std::vector<std::string> Args::getNonOptionArguments() const {
    return args;
}
