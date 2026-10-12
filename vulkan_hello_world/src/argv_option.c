#include "argv_option.h"

#include <getopt.h>
#include <stdio.h>

#include "utils/utils.h"

struct argv_option parse_options(int argc, char** argv)
{
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"load", required_argument, 0, 'l'},
        {"save", required_argument, 0, 's'},
    };

    struct argv_option res = {0};

    int c;
    while ((c = getopt_long(argc, argv, "s:l:h", long_options, nullptr)) != -1)
    {
        switch (c)
        {
        case 's':
            res.save_to = optarg;
            break;
        case 'l':
            res.load_to = optarg;
            break;
        default:
            printf("TODO: list options\n");
            ASSERT(false, "learn your option young man");
            break;
        }
    }
    return res;
}
