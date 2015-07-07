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
    n.compile(NREX_STR("^f*?([a-f]{2,3})(o+)\\2o(\\w+)bar$"));
    nrex_result_list results;
    nrex_string test = NREX_STR("ffffoooooo2000bar");
    n.match(test, results);
    TEST_COUT << test << std::endl;
    for (nrex_result_list::iterator it = results.begin(); it != results.end(); ++it)
    {
        TEST_COUT << test.substr(it->start, it->length) << std::endl;
    }
    return 0;
}
