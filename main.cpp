#include "nrex.hpp"
#include <iostream>

#ifdef NREX_UNICODE
#define TEST_COUT std::wcout
#else
#define TEST_COUT std::cout
#endif

int main()
{
    nrex n;
    n.compile(NREX_STR("^(fo+)bar$"));
    nrex_result_list results;
    nrex_string test = NREX_STR("fooobar");
    n.match(test, results);
    TEST_COUT << test << std::endl
        << test.substr(results[0].start, results[0].length) << std::endl
        << test.substr(results[1].start, results[1].length) << std::endl;
    return 0;
}

