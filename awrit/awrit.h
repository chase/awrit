// Copyright (c) 2023 Chase Colman. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef AWRIT_AWRIT_H_
#define AWRIT_AWRIT_H_

#include <list>

#include "include/base/cef_atomic_flag.h"
#include "include/cef_app.h"
#include "include/cef_thread.h"
#include "include/cef_render_handler.h"

class AwritClient : public CefClient,
                    public CefDisplayHandler,
                    public CefLifeSpanHandler,
                    public CefLoadHandler,
                    public CefRenderHandler {
 public:
  AwritClient();
  ~AwritClient();

  static AwritClient* GetInstance();

  // CefClient
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() override {
    return this;
  }

  // CefDisplayHandler
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) override;

  // CefLifeSpanHandler
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // CefLoadHandler
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame, ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) override;

  // CefRenderHandler
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
               const RectList& dirtyRects, const void* buffer, int width,
               int height) override;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }
  CefRefPtr<CefBrowser> Active() { return active_; }
  void ListenToInput(CefRefPtr<base::RefCountedData<base::AtomicFlag>> quitting);

 private:
  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;
  CefRefPtr<CefBrowser> active_;
  bool is_closing_;
  CefRefPtr<CefThread> input_thread_;
  CefRefPtr<base::RefCountedData<base::AtomicFlag>> quitting_;

  IMPLEMENT_REFCOUNTING(AwritClient);
};

class Awrit : public CefApp, public CefBrowserProcessHandler {
 public:
  Awrit();
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  void OnContextInitialized() override;
  CefRefPtr<CefClient> GetDefaultClient() override;
  void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override;

 private:
  IMPLEMENT_REFCOUNTING(Awrit);
};

#endif  // AWRIT_AWRIT_H_
