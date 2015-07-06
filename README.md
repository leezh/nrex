# NREX: Node RegEx

Small node-based regular expression library. It only does text pattern
matchhing, not replacement. To use add the files `nrex.hpp` and
`nrex.cpp` to your project and follow the example:

	nrex regex;
	regex.compile("^(fo+)bar$");
	std::string text = "foobar";

	nrex_result_list captures;
	if (regex.match(text, captures))
	{
		std::cout << text.substr(captures[0].start, captures[0].length);
	}

Currently supported features:
 * Capturing `()` and non-capturing `(?:)` groups
 * Any character `.`
 * Shorthand caracter classes `\w\W\s\S\d\D`
 * User-defined character classes such as `[A-Za-z]`
 * Simple quantifiers `?`, `*` and `+`
 * Lazy (non-greedy) quantifiers `*?`
 * Begining `^` and end `$` anchors
 * Alternation `|`

To do list:
 * Range quantifiers `{0,1}`
 * More efficient quantifiers (expand then backtrack)
 * Backreferences `\1`
 * Unicode `\uFFFF` code points

## License

Copyright (c) 2015, Zher Huei Lee
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
