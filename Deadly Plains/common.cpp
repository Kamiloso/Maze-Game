#include <string>

#include "common.h"

using namespace std;

unsigned int int_parse_any_string(const char* c_str)
{
    string str(c_str);

    unsigned int build_int = 0;
    while (!str.empty())
    {
        build_int *= 10;
        build_int += str[0] - '0';
        str = str.substr(1);
    }
    return build_int;
}