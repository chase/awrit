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

#include "platform_folders.h"

namespace folders {

std::string getHome() {
  std::string res;
  int uid = getuid();
  const char* homeEnv = std::getenv("HOME");
  if (uid != 0 && homeEnv) {
    // We only acknowlegde HOME if not root.
    res = homeEnv;
    return res;
  }
  struct passwd* pw = nullptr;
  struct passwd pwd;
  long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  if (bufsize < 0) {
    bufsize = 16384;
  }
  std::vector<char> buffer;
  buffer.resize(bufsize);
  int error_code = getpwuid_r(uid, &pwd, buffer.data(), buffer.size(), &pw);
  if (error_code) {
    return {};
  }
  const char* tempRes = pw->pw_dir;
  if (!tempRes) {
    return {};
  }
  res = tempRes;
  return res;
}

namespace {

std::string getLinuxFolderDefault(const char* envName,
                                  const char* defaultRelativePath) {
  std::string res;
  const char* tempRes = std::getenv(envName);
  if (tempRes) {
    // rel dir error
    if (res[0] != '/') {
    };
    res = tempRes;
    return res;
  }
  res = getHome() + "/" + defaultRelativePath;
  return res;
}

}  // namespace

std::string getCacheDir() {
#if defined(__APPLE__)
  auto home = getHome();
  if (home.empty()) return {};
  return home + "/Library/Caches";
#else
  return getLinuxFolderDefault("XDG_CACHE_HOME", ".cache");
#endif
}

}  // namespace folders
