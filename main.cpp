#include "nrex.hpp"
#include <iostream>
#include <string>

#ifdef NREX_UNICODE
#define TEST_STR std::wstring
#define TEST_COUT std::wcout
#else
#define TEST_STR std::string
#define TEST_COUT std::cout
#endif

int main()
{
    nrex n;
    n.compile(NREX_STR("^f*?[a-f]+((\\w)\\2+)o(\\w*)bar$"));
    nrex_result_list results;
    TEST_STR test = NREX_STR("ffffoooooo2000bar");
    n.match(test.c_str(), results);
    TEST_COUT << test << std::endl;
    for (nrex_result_list::iterator it = results.begin(); it != results.end(); ++it)
    {
        TEST_COUT << test.substr(it->start, it->length) << std::endl;
    }
    return 0;
}
