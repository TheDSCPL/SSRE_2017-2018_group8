#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <iostream>

#include "../headers/Options.hpp"
#include "../headers/Process.hpp"

using namespace std;

//static int verbose_flag;

int main (int argc, char** argv)
{
    /*int c;
    opterr = 0;

    while (true)
    {
        static struct option long_options[] =
                {
                        // These options set a flag.
                        {"verbose", required_argument,       &verbose_flag, 1},
                        {"brief",   no_argument,       &verbose_flag, 0},
                        // These options don't set a flag.
                        // We distinguish them by their indices.
                        {"add",     optional_argument, nullptr, 'a'},
                        {"append",  no_argument, nullptr, 'b'},
                        {"delete",  required_argument, nullptr, 'd'},
                        {"create",  required_argument, nullptr, 'c'},
                        {"file",    required_argument, nullptr, 'f'},
                        {nullptr, 0, nullptr, 0}
                };
        // getopt_long stores the option index here.
        int option_index = 0;

        c = getopt_long (argc, argv, "abc:d:f:k",
                         long_options, &option_index);
        // Detect the end of the options.
        if (c == -1)
            break;

        printf("optarg: '%s' ; option_index: %d\n", optarg, option_index);

        printf("%d\n",c);

        switch (c)
        {
            case 0:
                // If this option set a flag, do nothing else now.
//                if (long_options[option_index].flag != nullptr)
//                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'a':
                printf ("option -a '%s'\n", optarg);
                break;

            case 'b':
                puts ("option -b\n");
                break;

            case 'c':
                printf ("option -c with value `%s'\n", optarg);
                break;

            case 'd':
                printf ("option -d with value `%s'\n", optarg);
                break;

            case 'f':
                printf ("option -f with value `%s'\n", optarg);
                break;

            case '?':
                printf ("unknown option '%d'\n", optopt);
                // getopt_long already printed an error message.
                break;

            case ':':
                printf( "option '%c' could have an argument but doesn't '%s'\n", (char)optopt, optarg);
                break;

            default:
                exit (1);
        }
    }

    // Instead of reporting ‘--verbose’
    // and ‘--brief’ as they are encountered,
    // we report the final status resulting from them.
    if (verbose_flag)
        puts ("verbose flag is set");

    // Print any remaining command line arguments (not options).
    if (optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
            printf ("%s ", argv[optind++]);
        putchar ('\n');
    }*/

    /*int a = 0;
    const Args& args = Args::getArgs(argc, argv, std::vector<OptionAbstract> {
            {'o',"ola",HasArgument::OPTIONAL_ARGUMENT,[](){printf("Option ola!\n");}},
            OptionAbstract('a',"adeus").setFlag(&a,2)
    });

    cout << "a=" << a << endl << endl;

    for(const auto & e : args.getUsedOptions())
        cout << *e << endl;*/

    Process * test = new Process("sleep 3");
    test->start();
    test->join();

    delete test;

    Thread::usleep(100);

    exit (0);
}