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

//#define NREX_UNICODE
//#define NREX_THROW_ERROR

#ifdef NREX_UNICODE
typedef wchar_t nrex_char;
#else
typedef char nrex_char;
#endif

/*!
 * \brief Struct to contain the range of a capture result
 *
 * The range provided is relative to the begining of the string when searching
 * i.e. as if start was at 0.
 *
 * \see nrex_node::match()
 */
struct nrex_result
{
    public:
        int start; /*!< Start of text range */
        int length; /*!< Length of text range */
};

class nrex_node;

/*!
 * \brief Holds the compiled regex pattern
 */
class nrex
{
    private:
        int _capturing;
        nrex_node* _root;
    public:
        nrex();
        ~nrex();

        /*!
         * \brief Removes the compiled regex and frees up the memory
         */
        void reset();

        /*!
         * \brief Checks if there is a compiled regex being stored
         * \return True if present, False if not present
         */
        bool valid();

        /*!
         * \brief Provides number of captures the compiled regex uses
         *
         * This is used to provide the array size of the captures needed for
         * nrex::match() to work. The size is actually the number of capture
         * groups + one for the matching of the entire pattern. The result is
         * always capped at 100.
         *
         * \return The number of captures
         */
        int capture_size();

        /*!
         * \brief Compiles the provided regex pattern
         *
         * This automatically removes the existing compiled regex if already
         * present.
         *
         * If the NREX_THROW_ERROR was defined it would automatically throw a
         * runtime error nrex_compile_error if it encounters a problem when
         * parsing the pattern.
         *
         * \param The regex pattern
         * \return True if the pattern was succesfully compiled
         */
        bool compile(const nrex_char* pattern);

        /*!
         * \brief Uses the pattern to search through the provided string
         * \param str       The text to search through
         * \param captures  The array of results to store the capture results.
         *                  The size of that array needs to be the same as the
         *                  size given in nrex::capture_size(). As it matches
         *                  the function fills the array with the results. 0 is
         *                  the result for the entire pattern, 1 and above
         *                  corresponds to the regex capture group if present.
         * \param start     The starting point of the search. This also
         *                  determines the starting anchor.
         * \param end       The end point of the search. This also determines
         *                  the ending anchor.
         * \return          True if a match was found. False otherwise.
         */
        bool match(const nrex_char* str, nrex_result* captures, int start = 0, int end = -1) const;
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
