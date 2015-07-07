//  NREX: Node RegEx
//
//  Copyright (c) 2015, Zher Huei Lee
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//   1. Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//   2. Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//   3. Neither the name of the copyright holder nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
//  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
//  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "nrex.hpp"
#include <stack>
#include <bitset>

#ifdef NREX_UNICODE
#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <wctype.h>
#else
#include <wchar.h>
#endif
#define ISALPHANUM iswalnum
#else
#define ISALPHANUM isalnum
#endif

#ifdef NREX_THROW_ERROR
#define NREX_COMPILE_ERROR(M) throw nrex_compile_error(M)
#else
#define NREX_COMPILE_ERROR(M) reset(); return false
#endif

typedef nrex_string::value_type nrex_char;
static const nrex_string escapes = NREX_STR("^$()[]\\.+*?-aefnrtv");
static const nrex_string escaped_pairs = NREX_STR("^$()[]\\.+*?-\a\e\f\n\r\t\v");
static const nrex_string numbers = NREX_STR("0123456789");
static const nrex_string uppers = NREX_STR("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
static const nrex_string lowers = NREX_STR("abcdefghijklmnopqrstuvwxyz");
static const nrex_string whitespaces = NREX_STR(" \t\r\n\f");
static const nrex_string quantifiers = NREX_STR("+*?{");
static const nrex_string shorthands = NREX_STR("wWsSdD");

struct nrex_search
{
    public:
        const nrex_string& str;
        nrex_result_list& captures;
        int start;
        int end;

        nrex_char at(int pos)
        {
            return str.at(pos);
        }

        nrex_search(const nrex_string& str, nrex_result_list& results)
            : str(str)
            , captures(results)
            , start(0)
            , end(str.length())
        {
        }
};

bool nrex_shorthand_test(nrex_char repr, nrex_char c)
{
    bool found = false;
    bool invert = false;
    switch (repr)
    {
        case NREX_STR('.'):
            found = true;
            break;
        case NREX_STR('W'):
            invert = true;
        case NREX_STR('w'):
            if (c == '_' || ISALPHANUM(c))
            {
                found = true;
            }
            break;
        case NREX_STR('D'):
            invert = true;
        case NREX_STR('d'):
            if (numbers.find(c) != numbers.npos)
            {
                found = true;
            }
            break;
        case NREX_STR('S'):
            invert = true;
        case NREX_STR('s'):
            if (whitespaces.find(c) != whitespaces.npos)
            {
                found = true;
            }
            break;
    }
    return (found != invert);
}

struct nrex_node
{
        nrex_node* next;
        nrex_node* previous;
        nrex_node* parent;
        bool quantifiable;

        nrex_node(bool quantify = false)
            : next(NULL)
            , previous(NULL)
            , parent(NULL)
            , quantifiable(quantify)
        {
        }

        virtual ~nrex_node()
        {
            if (next)
            {
                delete next;
            }
        }

        virtual int test(nrex_search* s, int pos) const
        {
            return next ? next->test(s, pos) : -1;
        }

        virtual int test_parent(nrex_search* s, int pos) const
        {
            if (next)
            {
                pos = next->test(s, pos);
            }
            if (parent && pos >= 0)
            {
                pos = parent->test_parent(s, pos);
            }
            return pos;
        }
};

struct nrex_node_group : public nrex_node
{
        int capturing;
        std::vector<nrex_node*> childset;
        nrex_node* back;

        nrex_node_group(int capturing)
            : nrex_node(true)
            , capturing(capturing)
        {
            add_childset();
        }

        virtual ~nrex_node_group()
        {
            std::vector<nrex_node*>::iterator it;
            for (it = childset.begin(); it != childset.end(); ++it)
            {
                delete *it;
            }

        }

        int test(nrex_search* s, int pos) const
        {
            if (capturing >= 0)
            {
                s->captures[capturing].start = pos;
            }
            std::vector<nrex_node*>::const_iterator it;
            for (it = childset.begin(); it != childset.end(); ++it)
            {
                int res = (*it)->test(s, pos);
                if (res >= 0)
                {
                    if (capturing >= 0)
                    {
                        s->captures[capturing].length = res - pos;
                    }
                    return next ? next->test(s, res) : res;
                }
            }
            return -1;
        }

        virtual int test_parent(nrex_search* s, int pos) const
        {
            if (capturing >= 0)
            {
                s->captures[capturing].length = pos - s->captures[capturing].start;
            }
            return nrex_node::test_parent(s, pos);
        }

        void add_childset()
        {
            back = new nrex_node();
            childset.push_back(back);
        }

        void add_child(nrex_node* node)
        {
            node->parent = this;
            node->previous = back;
            back->next = node;
            back = node;
        }
};

struct nrex_node_char : public nrex_node
{
        nrex_char ch;

        nrex_node_char(nrex_char c)
            : nrex_node(true)
            , ch(c)
        {
        }

        int test(nrex_search* s, int pos) const
        {
            if (s->end == pos || s->at(pos) != ch)
            {
                return -1;
            }
            return next ? next->test(s, pos + 1) : pos + 1;
        }
};

struct nrex_node_class : public nrex_node
{
        struct nrex_range
        {
                nrex_char start;
                nrex_char end;
                nrex_range(nrex_char s, nrex_char e)
                    : start(s)
                    , end(e)
                {
                }
        };

        nrex_string characters;
        std::vector<nrex_char> shorthands;
        std::vector<nrex_range> ranges;
        bool negate;

        nrex_node_class()
            : nrex_node(true)
            , negate(false)
        {
        }

        void add_char(nrex_char c)
        {
            characters.push_back(c);
        }

        void add_char_range(nrex_char start, nrex_char end)
        {
            ranges.push_back(nrex_range(start, end));
        }

        void add_shorthand(nrex_char c)
        {
            shorthands.push_back(c);
        }

        int test(nrex_search* s, int pos) const
        {
            if (s->end == pos)
            {
                return -1;
            }
            nrex_char c = s->at(pos);
            bool found = false;
            if (characters.find(c) != characters.npos)
            {
                found = true;
            }
            else
            {
                std::vector<nrex_char>::const_iterator it;
                for (it = shorthands.begin(); it != shorthands.end(); ++it)
                {
                    if (nrex_shorthand_test(*it, c))
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (!found)
            {
                std::vector<nrex_range>::const_iterator it;
                for (it = ranges.begin(); it != ranges.end(); ++it)
                {
                    if (it->start <= c && c <= it->end)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found == negate)
            {
                return -1;
            }
            return next ? next->test(s, pos + 1) : pos + 1;
        }
};

struct nrex_node_shorthand_class : public nrex_node
{
        nrex_char repr;

        nrex_node_shorthand_class(nrex_char c)
            : nrex_node(true)
            , repr(c)
        {
        }

        int test(nrex_search* s, int pos) const
        {
            if (s->end == pos || !nrex_shorthand_test(repr, s->at(pos)))
            {
                return -1;
            }
            return next ? next->test(s, pos + 1) : pos + 1;
        }
};

struct nrex_node_quantifier : public nrex_node
{
        int min;
        int max;
        bool greedy;
        nrex_node* child;

        nrex_node_quantifier()
            : nrex_node()
            , min(0)
            , max(0)
            , greedy(true)
            , child(NULL)
        {
        }

        virtual ~nrex_node_quantifier()
        {
            if (child)
            {
                delete child;
            }
        }

        int test(nrex_search* s, int pos) const
        {
            std::stack<int> backtrack;
            backtrack.push(pos);
            while (backtrack.top() <= s->end)
            {
                if (max >= 1 && backtrack.size() > (std::size_t)max)
                {
                    break;
                }
                if (!greedy && (std::size_t)min < backtrack.size())
                {
                    int res = backtrack.top();
                    if (next)
                    {
                        res = next->test(s, res);
                    }
                    if (res >= 0 && parent->test_parent(s, res) >= 0)
                    {
                        return res;
                    }
                }
                int res = child->test(s, backtrack.top());
                if (res < 0 || res == backtrack.top())
                {
                    break;
                }
                backtrack.push(res);
            }
            while (greedy && (std::size_t)min < backtrack.size())
            {
                int res = backtrack.top();
                if (next)
                {
                    res = next->test(s, res);
                }
                if (res >= 0 && parent->test_parent(s, res) >= 0)
                {
                    return res;
                }
                backtrack.pop();
            }
            return -1;
        }
};

struct nrex_node_anchor : public nrex_node
{
        bool end;

        nrex_node_anchor(bool end)
            : nrex_node()
            , end(end)
        {
        }

        int test(nrex_search* s, int pos) const
        {
            if (!end && pos != s->start)
            {
                return -1;
            }
            else if (end && pos != s->end)
            {
                return -1;
            }
            return next ? next->test(s, pos) : pos;
        }
};

struct nrex_node_backreference : public nrex_node
{
        int ref;

        nrex_node_backreference(int ref)
            : nrex_node()
            , ref(ref)
        {
        }

        int test(nrex_search* s, int pos) const
        {
            nrex_result& r = s->captures[ref];
            for (int i = 0; i < r.length; ++i)
            {
                if (pos + i >= s->end)
                {
                    return -1;
                }
                if (s->at(r.start + i) != s->at(pos + i))
                {
                    return -1;
                }
            }
            return next ? next->test(s, pos + r.length) : pos + r.length;
        }
};

nrex::nrex()
    : _capturing(0)
    , _root(0)
{
}

nrex::~nrex()
{
    if (_root)
    {
        delete _root;
    }
}

bool nrex::valid()
{
    return (_root != NULL);
}

void nrex::reset()
{
    _capturing = 0;
    if (_root)
    {
        delete _root;
    }
}

bool nrex::compile(const nrex_string& pattern)
{
    reset();
    nrex_node_group* root = new nrex_node_group(_capturing);
    std::stack<nrex_node_group*> stack;
    stack.push(root);
    _root = root;

    nrex_string::const_iterator c;
    for (c = pattern.begin(); c != pattern.end(); ++c)
    {
        if (*c == NREX_STR('('))
        {
            if (*(c + 1) == NREX_STR('?'))
            {
                if (*(c + 2) == NREX_STR(':'))
                {
                    c += 2;
                    nrex_node_group* group = new nrex_node_group(-1);
                    stack.top()->add_child(group);
                    stack.push(group);
                }
                else
                {
                    NREX_COMPILE_ERROR("unrecognised qualifier for parenthesis");
                }
            }
            else
            {
                nrex_node_group* group = new nrex_node_group(++_capturing);
                stack.top()->add_child(group);
                stack.push(group);
            }
        }
        else if (*c == NREX_STR(')'))
        {
            if (stack.size() > 1)
            {
                stack.pop();
            }
            else
            {
                NREX_COMPILE_ERROR("unexpected ')'");
            }
        }
        else if (*c == NREX_STR('['))
        {
            nrex_node_class* next = new nrex_node_class;
            stack.top()->add_child(next);
            if (*(c + 1) == NREX_STR('^'))
            {
                next->negate = true;
                ++c;
            }
            while (true)
            {
                if (++c == pattern.end())
                {
                    NREX_COMPILE_ERROR("unclosed character class '[]'");
                }
                if (*c == NREX_STR(']'))
                {
                    break;
                }
                else if (*c == NREX_STR('\\'))
                {
                    nrex_char repr = *(c + 1);
                    nrex_string::size_type pos = escapes.find(repr);
                    if (pos != escapes.npos)
                    {
                        next->add_char(escaped_pairs.at(pos));
                        ++c;
                    }
                    else if (shorthands.find(repr) != shorthands.npos)
                    {
                        next->add_shorthand(repr);
                        ++c;
                    }
                    else
                    {
                        NREX_COMPILE_ERROR("escape token not recognised");
                    }
                }
                else
                {
                    if (uppers.find(*c) != uppers.npos)
                    {
                        if (*(c + 1) == NREX_STR('-'))
                        {
                            if (uppers.find(*(c + 2)) != uppers.npos)
                            {
                                next->add_char_range(*c, *(c + 2));
                                c += 2;
                                continue;
                            }
                        }
                    }
                    if (lowers.find(*c) != lowers.npos)
                    {
                        if (*(c + 1) == NREX_STR('-'))
                        {
                            if (lowers.find(*(c + 2)) != lowers.npos)
                            {
                                next->add_char_range(*c, *(c + 2));
                                c += 2;
                                continue;
                            }
                        }
                    }
                    if (numbers.find(*c) != numbers.npos)
                    {
                        if (*(c + 1) == NREX_STR('-'))
                        {
                            if (numbers.find(*(c + 2)) != numbers.npos)
                            {
                                next->add_char_range(*c, *(c + 2));
                                c += 2;
                                continue;
                            }
                        }
                    }
                    next->add_char(*c);
                }

            }
        }
        else if (quantifiers.find(*c) != quantifiers.npos)
        {
            nrex_node* child = stack.top()->back;
            if (!child->quantifiable)
            {
                NREX_COMPILE_ERROR("element not quantifiable");
            }
            nrex_node_quantifier* quant = new nrex_node_quantifier;
            if (*c == NREX_STR('?'))
            {
                quant->min = 0;
                quant->max = 1;
            }
            else if (*c == NREX_STR('+'))
            {
                quant->min = 1;
                quant->max = -1;
            }
            else if (*c == NREX_STR('*'))
            {
                quant->min = 0;
                quant->max = -1;
            }
            else if (*c == NREX_STR('{'))
            {
                bool max_set = false;
                quant->min = 0;
                quant->max = -1;
                while (true)
                {
                    if (++c == pattern.end())
                    {
                        NREX_COMPILE_ERROR("unclosed range quantifier '{}'");
                    }
                    else if (*c == NREX_STR('}'))
                    {
                        break;
                    }
                    else if (*c == NREX_STR(','))
                    {
                        max_set = true;
                        continue;
                    }
                    std::size_t pos = numbers.find(*c);
                    if (pos == numbers.npos)
                    {
                        NREX_COMPILE_ERROR("expected numeric digits, ',' or '}'");
                    }
                    if (max_set)
                    {
                        if (quant->max < 0)
                        {
                            quant->max = int(pos);
                        }
                        else
                        {
                            quant->max = quant->max * 10 + int(pos);
                        }
                    }
                    else
                    {
                        quant->min = quant->min * 10 + int(pos);
                    }
                }
                if (!max_set)
                {
                    quant->max = quant->min;
                }
            }
            if (*(c + 1) == NREX_STR('?'))
            {
                quant->greedy = false;
                ++c;
            }
            stack.top()->add_child(quant);
            child->previous->next = quant;
            child->previous = NULL;
            child->next = NULL;
            child->parent = quant;
            quant->child = child;
        }
        else if (*c == NREX_STR('|'))
        {
            stack.top()->add_childset();
        }
        else if (*c == NREX_STR('^') || *c == NREX_STR('$'))
        {
            nrex_node_anchor* next = new nrex_node_anchor((*c == NREX_STR('$')));
            stack.top()->add_child(next);
        }
        else if (*c == NREX_STR('.'))
        {
            nrex_node_shorthand_class* next = new nrex_node_shorthand_class('.');
            stack.top()->add_child(next);
        }
        else if (*c == NREX_STR('\\'))
        {
            nrex_char repr = *(c + 1);
            nrex_string::size_type pos = escapes.find(repr);
            if (pos != escapes.npos)
            {
                nrex_node_char* next = new nrex_node_char(escaped_pairs.at(pos));
                stack.top()->add_child(next);
                ++c;
            }
            else if (shorthands.find(repr) != shorthands.npos)
            {
                nrex_node_shorthand_class* next = new nrex_node_shorthand_class(repr);
                stack.top()->add_child(next);
                ++c;
            }
            else
            {
                pos = numbers.find(repr);
                if (pos != numbers.npos && pos > 0)
                {
                    nrex_string::size_type pos2 = numbers.find(*(c + 2));
                    int ref = 0;
                    if (pos2 != numbers.npos)
                    {
                        ref = int(pos) * 10 + int(pos2);
                        c += 2;
                    }
                    else
                    {
                        ref = int(pos);
                        ++c;
                    }
                    if (ref > _capturing)
                    {
                        NREX_COMPILE_ERROR("backreference to non-existent capture");
                    }
                    nrex_node_backreference* next = new nrex_node_backreference(ref);
                    stack.top()->add_child(next);
                }
                else
                {
                    NREX_COMPILE_ERROR("escape token not recognised");
                }
            }
        }
        else
        {
            nrex_node_char* next = new nrex_node_char(*c);
            stack.top()->add_child(next);
        }
    }
    return true;
}

bool nrex::match(const nrex_string& str, nrex_result_list& results) const
{
    results.resize(_capturing + 1);
    nrex_search s(str, results);
    nrex_result_list::iterator res_it;
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        for (res_it = results.begin(); res_it != results.end(); ++res_it)
        {
            res_it->start = 0;
            res_it->length = 0;
        }
        if (_root->test(&s, s.start + i) >= 0)
        {
            return true;
        }
    }
    return false;
}
