#pragma once

struct argv_option
{
    char* save_to;
    char* load_from;
};

struct argv_option parse_options(int argc, char** argv);
