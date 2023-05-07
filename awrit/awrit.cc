// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include "awrit.h"

#include "include/base/cef_atomic_flag.h"
#include "include/base/cef_callback.h"
#include "include/base/cef_scoped_refptr.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/cef_parser.h"
#include "include/internal/cef_linux.h"
#include "include/internal/cef_ptr.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "tui.h"

namespace {
AwritClient* g_awrit_client = nullptr;

std::string GetDataURI(const std::string& data, const std::string& mime_type) {
  return "data:" + mime_type + ";base64," +
         CefURIEncode(CefBase64Encode(data.data(), data.size()), false)
             .ToString();
}

}  // namespace

AwritClient::AwritClient() : is_closing_(false) {
  DCHECK(!g_awrit_client);
  g_awrit_client = this;
}

AwritClient::~AwritClient() { g_awrit_client = nullptr; }

AwritClient* AwritClient::GetInstance() { return g_awrit_client; }

void AwritClient::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                const CefString& title) {
  CEF_REQUIRE_UI_THREAD();
  SetTitle(title.ToString());
}

void AwritClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  browser_list_.push_back(browser);
}

bool AwritClient::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  if (browser_list_.size() == 1) {
    is_closing_ = true;
  }

  return false;
}

void AwritClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  for (auto it = browser_list_.begin(); it != browser_list_.end(); ++it) {
    if ((*it)->IsSame(browser)) {
      browser_list_.erase(it);
      break;
    }
  }

  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop.
    CefQuitMessageLoop();
  }
}

void AwritClient::OnLoadError(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame, ErrorCode errorCode,
                              const CefString& errorText,
                              const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED) {
    return;
  }

  // Display a load error message using a data: URI.
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL "
     << std::string(failedUrl) << " with error " << std::string(errorText)
     << " (" << errorCode << ").</h2></body></html>";

  frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void AwritClient::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI, base::BindOnce(&AwritClient::CloseAllBrowsers, this,
                                       force_close));
    return;
  }

  if (browser_list_.empty()) {
    return;
  }

  for (auto it = browser_list_.begin(); it != browser_list_.end(); ++it) {
    (*it)->GetHost()->CloseBrowser(force_close);
  }
}

void AwritClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  auto x = WindowSize();
  rect.Set(0, 0, x.width, x.height);
}

void AwritClient::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
                          const RectList& dirtyRects, const void* buffer,
                          int width, int height) {
  // ignore popups for now
  if (type != PaintElementType::PET_VIEW) return;

  Paint(dirtyRects, buffer, width, height);
}

Awrit::Awrit() : quitting_(base::MakeRefCounted<base::AtomicFlag>()) {}

void Awrit::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();
  CefRefPtr<AwritClient> client(new AwritClient());
  CefBrowserSettings browser_settings;
  browser_settings.windowless_frame_rate = 60;

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

  std::string url = command_line->GetSwitchValue("url");
  if (url.empty()) {
    url = "https://devdocs.io/cpp/header/coroutine";
  }

  CefWindowInfo window_info;
  window_info.SetAsWindowless(0L);

  CefBrowserHost::CreateBrowser(window_info, client, url, browser_settings,
                                nullptr, nullptr);
}

CefRefPtr<CefClient> Awrit::GetDefaultClient() {
  return AwritClient::GetInstance();
}
