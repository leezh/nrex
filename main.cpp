#include "nrex.hpp"
#include <iostream>
#include <string>

#ifdef NREX_UNICODE
#define TEST_CSTR(X) L##X
#define TEST_STR std::wstring
#define TEST_COUT std::wcout
#else
#define TEST_CSTR(X) X
#define TEST_STR std::string
#define TEST_COUT std::cout
#endif

int main()
{
    nrex n;
    n.compile(TEST_CSTR("^f*?[a-f]+((\\w)\\2+)o(\\w*)bar$"));
    nrex_result results[n.capture_size()];
    TEST_STR test = TEST_CSTR("ffffoooooo2000bar");
    n.match(test.c_str(), results);
    TEST_COUT << test << std::endl;
    for (int i = 0; i < n.capture_size(); ++i)
    {
        TEST_COUT << test.substr(results[i].start, results[i].length) << std::endl;
    }
    return 0;
}
