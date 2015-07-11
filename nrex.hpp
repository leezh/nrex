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

#ifndef NREX_HPP
#define NREX_HPP

#include <vector>

//#define NREX_UNICODE
//#define NREX_THROW_ERROR

#ifdef NREX_UNICODE
typedef wchar_t nrex_char;
#else
typedef char nrex_char;
#endif

struct nrex_result
{
    public:
        int start;
        int length;
};

class nrex_node;
typedef std::vector<nrex_result> nrex_result_list;

class nrex
{
    private:
        int _capturing;
        nrex_node* _root;
    public:
        nrex();
        ~nrex();
        void reset();
        bool valid();
        bool compile(const nrex_char* pattern);
        bool match(const nrex_char* str, nrex_result_list& results, int start = 0, int end = -1) const;
};

#ifdef NREX_THROW_ERROR
#include <string>
#include <stdexcept>

class nrex_compile_error : std::runtime_error
{
    public:
        nrex_compile_error(const std::string& message)
            : std::runtime_error(message)
        {
        }

        ~nrex_compile_error() throw()
        {
        }
};

#endif

#endif // NREX_HPP
