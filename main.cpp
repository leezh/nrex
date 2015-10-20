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
    //test(n, TEST_CSTR("^f*?[a-f]+((\\w)\\2+)o(\\w*)bar$"), TEST_CSTR("ffffoooooo2000bar"));
    test(n, TEST_CSTR("\"((?:\\\\.|[^\"])*)\""), TEST_CSTR("\"And he said \\\"t'was great\\\"\""));
    test(n, TEST_CSTR(":(?:\\s+<([^>]+)>)?\\s+(.*)"), TEST_CSTR(": <abc> def"));
    test(n, TEST_CSTR("a.(?!b|c)"), TEST_CSTR("a1b a2c a3d"));
    test(n, TEST_CSTR("a.(?=c)"), TEST_CSTR("a1b a2c a3d"));
    test(n, TEST_CSTR("a[^bc]"), TEST_CSTR("ab ac ad"));
    test(n, TEST_CSTR("a{r}"), TEST_CSTR("ba{r}"));
    test(n, TEST_CSTR("[[:xdigit:]]{5}\\x20\\u0046"), TEST_CSTR("x09afA F"));
    test(n, TEST_CSTR("[[:alnu]{5}"), TEST_CSTR("laun:dry"));
    test(n, TEST_CSTR("\\bab.\\b"), TEST_CSTR("ab1c ab2 ab3"));
    return 0;
}
