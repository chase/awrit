// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "awrit.h"
#include "include/base/cef_logging.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "third_party/platform_folders.h"
#include "tui.h"

#if defined(OS_MAC)
#include "include/wrapper/cef_library_loader.h"
extern void MacInit();
extern void MacCleanup();
#endif

#if defined(OS_WIN) && defined(CEF_USE_SANDBOX)
#include "include/cef_sandbox_win.h"
#pragma comment(lib, "cef_sandbox.lib")
#endif

#if defined(OS_POSIX)
#include <unistd.h>
#endif

int main(int argc, char* argv[]) {
#if defined(OS_MAC)
  CefScopedLibraryLoader library_loader;
  if (!library_loader.LoadInMain()) {
    return 1;
  }
#endif

#if defined(OS_WIN)
  CefMainArgs main_args(::GetModuleHandle(nullptr));
#else
  CefMainArgs main_args(argc, argv);
#endif

  void* win_sandbox_info = nullptr;

#if !defined(OS_MAC)
  int exit_code = CefExecuteProcess(main_args, nullptr, win_sandbox_info);
  if (exit_code >= 0) {
    // exit sub-process
    return exit_code;
  }
#endif

  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromArgv(argc, argv);

  CefSettings settings;
#if defined(OS_MAC)
  MacInit();
#endif

  settings.log_severity = cef_log_severity_t::LOGSEVERITY_FATAL;
  settings.persist_user_preferences = true;
  auto cacheDir = folders::getCacheDir();
  if (!cacheDir.empty()) {
    CefString(&settings.cache_path).FromString(cacheDir + "/awrit");
  }
  settings.windowless_rendering_enabled = true;

#if !defined(CEF_USE_SANDBOX)
  settings.no_sandbox = true;
#endif

  Initialize();
  CefRefPtr<Awrit> app(new Awrit);
  CefInitialize(main_args, settings, app.get(), win_sandbox_info);
  CefRunMessageLoop();
  CefShutdown();
  Restore();

#if defined(OS_MAC)
  MacCleanup();
#endif

  return 0;
}
