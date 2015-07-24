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

void test(nrex& n, const nrex_char* exp, const TEST_STR& test)
{
    n.compile(exp);
    nrex_result results[n.capture_size()];
    n.match(test.c_str(), results);
    TEST_COUT << TEST_CSTR("Expression: ") << exp << std::endl;
    TEST_COUT << TEST_CSTR("String: ") << test << std::endl;
    for (int i = 0; i < n.capture_size(); ++i)
    {
        TEST_COUT << test.substr(results[i].start, results[i].length) << std::endl;
    }
    TEST_COUT << std::endl;
}

int main()
{
    nrex n;
    test(n, TEST_CSTR("^f*?[a-f]+((\\w)\\2+)o(\\w*)bar$"), TEST_CSTR("ffffoooooo2000bar"));
    test(n, TEST_CSTR("\"((?:\\\\.|[^\"])*)\""), TEST_CSTR("\"abc \\\" twas\""));
    return 0;
}
