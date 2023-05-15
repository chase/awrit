/*
Its is under the MIT license, to encourage reuse by cut-and-paste.
The original files are hosted here: https://github.com/sago007/PlatformFolders
Copyright (c) 2015-2016 Poul Sander
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef THIRD_PARTY_PLATFORM_FOLDERS_H
#define THIRD_PARTY_PLATFORM_FOLDERS_H

#include <pwd.h>
#include <unistd.h>

#include <string>
#include <vector>

namespace folders {

/**
 * Retrives the effective user's home dir.
 * If the user is running as root we ignore the HOME environment. It works badly
 * with sudo. Writing to $HOME as root implies security concerns that a
 * multiplatform program cannot be assumed to handle.
 * @return The home directory. HOME environment is respected for non-root users
 * if it exists.
 */
std::string getHome();

std::string getCacheDir(); 

}

#endif  // THIRD_PARTY_PLATFORM_FOLDERS_H
