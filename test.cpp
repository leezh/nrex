#include "nrex.hpp"
#include <locale>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#ifdef NREX_UNICODE
typedef std::wstring string;
typedef std::wifstream ifstream;
typedef std::wistringstream isstream;
#else
typedef std::string string;
typedef std::ifstream ifstream;
typedef std::istringstream isstream;
#endif

class ctype : public std::ctype<char>
{
        mask table[table_size];
    public:
        ctype(size_t refs = 0)
            : std::ctype<char>(&table[0], false, refs)
        {
            std::copy(classic_table(), classic_table() + table_size, table);
            table['/'] = (mask)space;
            table[' '] = (mask)punct;
        }
};

int main()
{
    ifstream file("test.txt");
    if (!file.is_open())
    {
        std::cout << "could not find test.txt" << std::endl;
        return -2;
    }

    int tests = 0;
    int passed = 0;
    int line_number = 0;

    std::locale locale(std::locale::classic(), new ctype);

    nrex n;
    std::cout << "==================" << std::endl;
    while (!file.eof())
    {
        string line;
        std::getline(file, line);
        line_number++;
        if (line.length() == 0 || line.at(0) == '#')
        {
            continue;
        }


        isstream stream(line);
        stream.imbue(locale);

        string pattern;
        stream >> pattern;
        tests++;
        std::cout << "Line " << line_number << " ";

        int captures = 0;
        stream >> captures;

        n.compile(pattern.c_str());
        if (n.capture_size() != captures)
        {
            std::cout << " FAILED (Compile)" << std::endl;
            continue;
        }

        if (captures == 0)
        {
            std::cout << " OK" << std::endl;
            passed++;
            continue;
        }

        string text;
        stream >> text;
        if (text.length() == 1 && text.at(0) == '#')
        {
            text.clear();
        }
        nrex_result results[captures];
        bool found = n.match(text.c_str(), results);

        bool failed = false;

        int position;
        stream >> position;
        if ((position >= 0) != found || (found && position != results[0].start))
        {
            failed = true;
        }

        for (int i = 0; i < captures; i++)
        {
            string result;
            stream >> result;

            if (results[i].length == 0 && result.length() == 0)
            {
                continue;
            }

            if (text.substr(results[i].start, results[i].length) != result)
            {
                failed = true;
            }
        }

        if (!failed)
        {
            std::cout << " OK" << std::endl;
            passed++;
        }
        else
        {
            std::cout << " FAILED (Tests)" << std::endl;
        }
    }
    std::cout << "==================" << std::endl;
    std::cout << "Tests: " << tests << std::endl;
    std::cout << "Successes: " << passed << std::endl;
    std::cout << "Failed: " << tests - passed << std::endl;
    if (tests != passed)
    {
        return -1;
    }
    return 0;
}
