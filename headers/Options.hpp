#ifndef SSRE_OPTIONS_HPP
#define SSRE_OPTIONS_HPP

#include <string>
#include <vector>
#include <getopt.h>
#include <functional>

enum class HasArgument {
    NO_ARGUMENT,
    REQUIRED_ARGUMENT,
    OPTIONAL_ARGUMENT
};

class OptionAbstract {
    friend class Args;
private:
    const HasArgument _hasArgument;
    const std::string longForm;
    const int shortForm;
    static std::function<void()> EMPTY_FUNCTION;
protected:
    static const HasArgument DEFAULT_HAS_ARGUMENT;
    std::function<void()> action;
    int * flag = nullptr;
    int flagValue;

    const HasArgument & getHasArgument() const;
    bool isNoArgument() const;
    bool isRequiredArgument() const;
    bool isOptionalArgument() const;
public:
    //shortForm=0 -> no short form
    //longForm="" -> no long form
    OptionAbstract(const int& shortForm,
                   const std::string& longForm,
                   const HasArgument& hasArgument = DEFAULT_HAS_ARGUMENT,
                   const std::function<void()> & action = EMPTY_FUNCTION);

    //sets the "flag" field will not null (unless null is passed to this function) and when the "Option" subclass runs, it will set the value of the "flag" pointer to "val"
    OptionAbstract& setFlag(int * flag, int val);

    bool hasLongForm() const;
    const std::string & getLongForm() const;
    bool hasShortForm() const;
    const int & getShortForm() const;
};

class Args;

class Option : public OptionAbstract {
    friend class Args;
private:
    const std::string argument;
    const std::string printableForm;

    void doAction();
    std::string makePrintableForm() const;
    Option(const OptionAbstract & optionTemplate, const std::string & argument);
public:

    bool hasArgument() const;
    const std::string & getArgument() const;

    operator const char *() const {
        return printableForm.c_str();
    }
};

class Args {
private:
    std::vector<Option*> options;
    std::vector<std::string> args;

    Args(int argc, char **argv, const std::vector<OptionAbstract> &opts);
    ~Args();

    static unsigned int countLongOptions(const std::vector<OptionAbstract> &opts);
    static option * makeLongOptions(const std::vector<OptionAbstract> &opts);
    static char* makeShortOptions(const std::vector<OptionAbstract> &opts);
public:
    static const Args &getArgs(int argc = 0, char **argv = nullptr,
                               std::vector<OptionAbstract> opts = std::vector<OptionAbstract>());

    Args(Args const&)           = delete;
    void operator=(Args const&) = delete;
};

#endif