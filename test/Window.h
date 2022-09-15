#ifndef INC_LG3_GFX_WINDOW_H_
#define INC_LG3_GFX_WINDOW_H_
/**
@file Window.h
@author t-sakai
@date 2017/11/03

//--- Usage
//-----------------------------------------------------------
Put '#define LGFX_WINDOW_IMPLEMENTATION' before including this file to create the implementation.
*/
#if !defined(LG3_ASSERT) && defined(_DEBUG)
#include <cassert>
#define LG3_ASSERT(exp) assert(exp)
#endif

#include <cstdint>

#ifdef LG3_GFX_USE_XLIB
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifdef LG3_GFX_USE_XCB
#include <xcb/xcb.h>
#endif

#ifdef LG3_GFX_USE_WIN32
#include <Windows.h>
struct IDXGISwapChain;
#endif

#ifndef LG3_NULL
#   ifdef __cplusplus
#       if 201103L<=__cplusplus || 1900<=_MSC_VER
#           define LG3_CPP11 1
#       endif
#       ifdef LG3_CPP11
#           define LG3_NULL nullptr
#       else
#           define LG3_NULL 0
#       endif
#   else //__cplusplus
#       define LG3_NULL (void*)0
#   endif //__cplusplus
#endif

namespace lg3::gfx
{
#ifndef LG3_GFX_TYPES
#define LG3_GFX_TYPES
    typedef char Char;
    typedef wchar_t WChar;
    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

    typedef std::intptr_t intptr_t;
#endif

    class Window
    {
    public:
#ifdef LG3_GFX_USE_WIN32
        typedef MSG Event;
        typedef HWND Handle;

        static const u32 DefaultWindowStyle = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_BORDER | WS_VISIBLE;
        static const u32 DefaultWindowExStyle = WS_EX_CLIENTEDGE;
        static const u32 DefaultStyle = WS_VISIBLE | WS_CAPTION;
        static const u32 DefaultExStyle = WS_EX_APPWINDOW;

        struct Param
        {
            HINSTANCE instance_;
            u32 width_;
            u32 height_;
            u32 style_; //WS_VISIBLE | WS_CAPTION
            u32 exStyle_; //WS_EX_APPWINDOW
            WNDPROC wndProc_;
            LPCWSTR title_;
            bool windowed_;
        };
#endif
#ifdef LG3_GFX_USE_XCB
        typedef xcb_generic_event_t* Event;
        typedef xcb_window_t Handle;
        typedef bool (*WNDPROC)(Event);

        struct InitParam
        {
            u16 width_;
            u16 height_;
            WNDPROC wndProc_;
            const char* title_;
        };
#endif
#ifdef LG3_GFX_USE_XLIB
        typedef XEvent Event;
        typedef ::Window Handle;
        typedef bool (*WNDPROC)(Event);
        struct InitParam
        {
            u32 width_; 
            u32 height_;
            WNDPROC wndProc_;
            const char* title_;
        };
#endif

#ifdef LG3_GFX_USE_WIN32
        HINSTANCE getInstance();
        bool peekEvent(HWND hDlg);
#endif
#ifdef LG3_GFX_USE_XCB
        xcb_connection_t* getConnection();
#endif
#ifdef LG3_GFX_USE_XLIB
        Display* getDisplay();
#endif

        struct Vector2
        {
            s32 x_;
            s32 y_;
        };

        Window();
        Window(Window&& rhs);
        ~Window();

        Window& operator=(Window&& rhs);

        bool create(Param& param);
        void destroy();
        const Handle getHandle() const;
        Handle getHandle();
        void setShow(bool show);
        Vector2 getViewSize();

        bool peekEvent();

        bool isFullscreen() const;
#ifdef LG3_GFX_API_D3D12
        void toggleFullscreen(IDXGISwapChain* swapChain);
#endif
        void setWindowTopMost(bool topMost);
    private:
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

#ifdef LG3_GFX_USE_WIN32
        HINSTANCE instance_;
        Handle handle_;
        u32 windowStyle_;
        bool isFullscreen_;
        RECT windowRect_;
#endif
#ifdef LG3_GFX_USE_XCB
        xcb_connection_t* connection_;
        Handle handle_;
        xcb_intern_atom_reply_t* delete_;
        u16 width_;
        u16 height_;
        WNDPROC wndProc_;
#endif
#ifdef LG3_GFX_USE_XLIB
        Display* display_;
        Handle handle_;
        Atom delete_;
        s32 width_;
        s32 height_;
        WNDPROC wndProc_;
#endif
        Event event_;
    };
}
#endif //INC_LG3_GFX_WINDOW_H_

#ifdef LGFX_WINDOW_IMPLEMENTATION
#ifndef LGFX_WINDOW_IMPLEMENTATION_DONE
#define LGFX_WINDOW_IMPLEMENTATION_DONE
/**
@file Window.cpp
@author t-sakai
@date 2017/11/03
*/
#ifdef LG3_GFX_USE_XCB
#include <xcb/xcb_icccm.h>
#include <stdlib.h>
#include <string.h>
#endif

#ifdef LG3_GFX_USE_XLIB
#include <X11/Xatom.h>
#include <string.h>
#endif

namespace lg3::gfx
{
namespace
{
#ifdef LG3_GFX_USE_WIN32
    LPCWSTR CLASSNAME_ = L"LG3_WINDOW";
    static LRESULT CALLBACK WndProc(HWND hWnd, u32 msg, WPARAM wPara, LPARAM lParam);
#endif
}
    Window::Window()
#ifdef LG3_GFX_USE_WIN32
        :instance_(LG3_NULL)
        ,handle_(LG3_NULL)
        ,windowStyle_(WS_OVERLAPPEDWINDOW)
        ,isFullscreen_(false)
        ,windowRect_{}
#endif
#ifdef LG3_GFX_USE_XCB
        :connection_(LG3_NULL)
        ,handle_(0)
        ,delete_(LG3_NULL)
        ,wndProc_(LG3_NULL)
#endif
#ifdef LG3_GFX_USE_XLIB
        :display_(LG3_NULL)
        ,handle_(0)
        ,wndProc_(LG3_NULL)
#endif
    {
    }

    Window::Window(Window&& rhs)
#ifdef LG3_GFX_USE_WIN32
        :instance_(rhs.instance_)
        ,handle_(rhs.handle_)
        ,windowStyle_(rhs.windowStyle_)
        ,isFullscreen_(rhs.isFullscreen_)
        ,windowRect_(rhs.windowRect_)
#endif
#ifdef LG3_GFX_USE_XCB
        :connection_(rhs.connection_)
        ,handle_(rhs.handle_)
        ,delete_(rhs.delete_)
        ,wndProc_(rhs.wndProc_)
#endif
#ifdef LG3_GFX_USE_XLIB
        :display_(rhs.display_)
        ,handle_(rhs.handle_)
        ,wndProc_(rhs.wndProc_)
#endif
    {
#ifdef LG3_GFX_USE_WIN32
        rhs.instance_ = LG3_NULL;
        rhs.handle_ = LG3_NULL;
        rhs.windowStyle_ = WS_OVERLAPPEDWINDOW;
        rhs.isFullscreen_ = false;
        rhs.windowRect_ = {};
#endif
#ifdef LG3_GFX_USE_XCB
        rhs.connection_ = LG3_NULL;
        rhs.handle_ = 0;
        rhs.delete_ = LG3_NULL;
        rhs.wndProc_ = LG3_NULL;
#endif
#ifdef LG3_GFX_USE_XLIB
        rhs.display_ = LG3_NULL;
        rhs.handle_ = 0;
        rhs.wndProc_ = LG3_NULL;
#endif
    }

    Window::~Window()
    {
        destroy();
    }

    Window& Window::operator=(Window&& rhs)
    {
        if(this == &rhs){
            return *this;
        }

#ifdef LG3_GFX_USE_WIN32
        instance_ = rhs.instance_;
        handle_ = rhs.handle_;
        windowStyle_ = rhs.windowStyle_;
        isFullscreen_ = rhs.isFullscreen_;
        windowRect_ = rhs.windowRect_;

        rhs.instance_ = LG3_NULL;
        rhs.handle_ = LG3_NULL;
        rhs.windowStyle_ = WS_OVERLAPPEDWINDOW;
        rhs.isFullscreen_ = false;
        rhs.windowRect_ ={};
#endif
#ifdef LG3_GFX_USE_XCB
        connection_ = rhs.connection_;
        handle_ = rhs.handle_;
        delete_ = rhs.delete_;
        wndProc_ = rhs.wndProc_;

        rhs.connection_ = LG3_NULL;
        rhs.handle_ = 0;
        rhs.delete_ = LG3_NULL;
        rhs.wndProc_ = LG3_NULL;
#endif
#ifdef LG3_GFX_USE_XLIB
        display_ = rhs.display_;
        handle_ = rhs.handle_;
        wndProc_ = rhs.wndProc_;

        rhs.display_ = LG3_NULL;
        rhs.handle_ = 0;
        rhs.wndProc_ = LG3_NULL;
#endif
        return *this;
    }

    bool Window::create(Param& param)
    {
        LG3_ASSERT(LG3_NULL == handle_);
        LG3_ASSERT(LG3_NULL != param.title_);

#ifdef LG3_GFX_USE_WIN32
        if(LG3_NULL != param.instance_){
            instance_ = param.instance_;
        } else{
            if(FAILED(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, LG3_NULL, &instance_))){
                return false;
            }
        }

        //Create and register window class
        //-----------------------------------------------------------
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex.lpfnWndProc	= (LG3_NULL == param.wndProc_)? WndProc : param.wndProc_;
        wcex.cbClsExtra		= 0;
        wcex.cbWndExtra		= 0;
        wcex.hInstance		= instance_;
        wcex.hIcon			= LoadIcon(LG3_NULL, IDI_WINLOGO);
        wcex.hCursor		= LoadCursor(LG3_NULL, IDC_ARROW);
        wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);//(HBRUSH)(COLOR_WINDOW+1);
        wcex.lpszMenuName	= LG3_NULL;
        wcex.lpszClassName	= CLASSNAME_;
        wcex.hIconSm		= LoadIcon(wcex.hInstance, LG3_NULL);

        if(!RegisterClassEx(&wcex)){
            return false;
        }

        RECT rect;
        rect.left = 0;
        rect.top = 0;
        rect.right = param.width_;
        rect.bottom = param.height_;

        isFullscreen_ = false;

        windowStyle_ = param.style_;
        u32 exStyle = param.exStyle_;

        AdjustWindowRectEx(&rect, windowStyle_, FALSE, exStyle);

        handle_ = CreateWindowEx(
            exStyle,
            CLASSNAME_,
            param.title_,
            windowStyle_,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rect.right - rect.left,
            rect.bottom - rect.top,
            LG3_NULL,
            LG3_NULL,
            instance_,
            LG3_NULL);

        if(LG3_NULL == handle_){
            UnregisterClass(CLASSNAME_, instance_);
            return false;
        }

        //UpdateWindow(handle_);
        SetFocus(handle_);
#endif

#ifdef LG3_GFX_USE_XCB
        int screenIndex = 0;
        connection_ = xcb_connect(LG3_NULL, &screenIndex);
        if(xcb_connection_has_error(connection_)){
            connection_ = LG3_NULL;
            return false;
        }
        const xcb_setup_t* setup = xcb_get_setup(connection_);
        xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);
        for(int i=0; i<screenIndex; ++i){
            xcb_screen_next(&screenIterator);
        }
        xcb_screen_t* screen = screenIterator.data;
        handle_ = xcb_generate_id(connection_);

        u32 valueList[] =
        {
            screen->white_pixel,
            XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY,
        };

        xcb_create_window(
            connection_,
            XCB_COPY_FROM_PARENT,
            handle_,
            screen->root,
            -1, -1, param.width_, param.height_,
            0,
            XCB_WINDOW_CLASS_INPUT_OUTPUT,
            screen->root_visual,
            XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
            valueList);

        wndProc_ = param.wndProc_;

        xcb_flush(connection_);
        xcb_change_property(
            connection_,
            XCB_PROP_MODE_REPLACE,
            handle_,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8,
            strlen(param.title_),
            param.title_);

        xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(connection_, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* protocol_reply = xcb_intern_atom_reply(connection_, protocols_cookie, 0);
        xcb_intern_atom_cookie_t delete_cookie = xcb_intern_atom(connection_, 0, 16, "WM_DELETE_WINDOW");
        delete_ = xcb_intern_atom_reply(connection_, delete_cookie, 0);
        xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, handle_, (*protocol_reply).atom, 4, 32, 1, &(*delete_).atom);
        free(protocol_reply);
        
        xcb_size_hints_t hints;
        xcb_icccm_size_hints_set_min_size(&hints, param.width_, param.height_);
        xcb_icccm_size_hints_set_max_size(&hints, param.width_, param.height_);
        xcb_icccm_set_wm_size_hints(connection_, handle_, XCB_ATOM_WM_NORMAL_HINTS, &hints);

        width_ = param.width_;
        height_ = param.height_;
        xcb_map_window(connection_, handle_);
        xcb_flush(connection_);
#endif
#ifdef LG3_GFX_USE_XLIB
        display_ = XOpenDisplay(LG3_NULL);
        if(LG3_NULL == display_){
            return false;
        }
        int default_screen = DefaultScreen(display_);
        handle_ = XCreateWindow(
            display_,
            DefaultRootWindow(display_),
            -1, -1, param.width_, param.height_,
            0, //border width
            CopyFromParent, //color depth
            InputOutput, //
            DefaultVisual(display_, DefaultScreen(display_)),
            0, LG3_NULL);
        if(handle_<=0){
            XCloseDisplay(display_);
            display_ = LG3_NULL;
            return false;
        }
        wndProc_ = param.wndProc_;

        //------------------------
        XTextProperty name;
        name.value = reinterpret_cast<unsigned char*>(const_cast<char*>(param.title_));
        name.encoding = XA_STRING;
        name.format = 8;
        name.nitems = strlen(param.title_);
        XSetWMName(display_, handle_, &name);

        delete_ = XInternAtom(display_, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(display_, handle_, &delete_, 1);
        XSelectInput(display_, handle_, ExposureMask | StructureNotifyMask);

        XSizeHints hints;
        hints.min_width = param.width_;
        hints.min_height = param.height_;
        hints.max_width = param.width_;
        hints.max_height = param.height_;
        hints.flags = PMinSize | PMaxSize;
        XSetWMNormalHints(display_, handle_, &hints);

        width_ = static_cast<s32>(param.width_);
        height_ = static_cast<s32>(param.height_);
        XClearWindow(display_, handle_);
        XMapWindow(display_, handle_);
#endif
        return true;
    }

    void Window::destroy()
    {
#ifdef LG3_GFX_USE_WIN32
        if(LG3_NULL != handle_){
            DestroyWindow(handle_);
            handle_ = LG3_NULL;
            UnregisterClass(CLASSNAME_, instance_);
            instance_ = LG3_NULL;
        }
#endif
#ifdef LG3_GFX_USE_XCB
        if(LG3_NULL != delete_){
            free(delete_);
            delete_ = LG3_NULL;
        }
        if(LG3_NULL != connection_){
            wndProc_ = LG3_NULL;
            xcb_destroy_window(connection_, handle_);
            xcb_disconnect(connection_);
            handle_ = 0;
            connection_ = LG3_NULL;
        }
#endif
#ifdef LG3_GFX_USE_XLIB
        if(LG3_NULL != display_){
            wndProc_ = LG3_NULL;
            XDestroyWindow(display_, handle_);
            XCloseDisplay(display_);
            handle_ = 0;
            display_ = LG3_NULL;
        }
#endif
    }

    const Window::Handle Window::getHandle() const
    {
        return handle_;
    }

    Window::Handle Window::getHandle()
    {
        return handle_;
    }

    void Window::setShow(bool show)
    {
#ifdef LG3_GFX_USE_WIN32
        ShowWindow(handle_, (show)?SW_SHOW : SW_HIDE);
#endif
#ifdef LG3_GFX_USE_XCB
        if(show){
            xcb_map_window(connection_, handle_);
        }else{
            xcb_unmap_window(connection_, handle_);
        }
#endif
#ifdef LG3_GFX_USE_XLIB
        if(show){
            XMapWindow(display_, handle_);
        }else{
            XUnmapWindow(display_, handle_);
        }
#endif
    }

    Window::Vector2 Window::getViewSize()
    {
#ifdef LG3_GFX_USE_WIN32
        RECT rect;
        s32 width, height;
        if(TRUE == GetClientRect(handle_, &rect)){
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }else{
            width = 1;
            height = 1;
        }
        return {width, height};
#endif
#ifdef LG3_GFX_USE_XCB
        xcb_get_geometry_cookie_t cookie = xcb_get_geometry(connection_, handle_);
        xcb_get_geometry_reply_t* geom = xcb_get_geometry_reply(connection_, cookie, LG3_NULL);
        s32 width = geom->width - geom->border_width*2;
        s32 height = geom->height - geom->border_width*2;
        free(geom);
        return {width, height};
#endif
#ifdef LG3_GFX_USE_XLIB
        s32 x;
        s32 y;
        s32 width=1;
        s32 height=1;
        ::Window root;
        u32 border;
        u32 depth;
        XGetGeometry(display_, handle_,
            &root,
            &x, &y, reinterpret_cast<u32*>(&width), reinterpret_cast<u32*>(&height),
            &border, //border
            &depth);//depth
        return {width, height};
#endif
    }

    bool Window::peekEvent()
    {
#ifdef LG3_GFX_USE_WIN32
        while(PeekMessage(&event_, nullptr, 0, 0, PM_REMOVE)) {
        //while(PeekMessage(&event_, handle_, 0, 0, PM_REMOVE)) {
            if(event_.message == WM_QUIT) {
                return false;
            }

            TranslateMessage(&event_);
            DispatchMessage(&event_);
        }
        return true;
#endif
#ifdef LG3_GFX_USE_XCB
        for(;;){
            event_ = xcb_poll_for_event(connection_);
            if(LG3_NULL == event_){
                return true;
            }
            switch(event_->response_type & 0x7F){
            case XCB_CLIENT_MESSAGE:
                if((*(xcb_client_message_event_t*)event_).data.data32[0] == (*delete_).atom){
                    return false;
                }
                break;
            }
            if(LG3_NULL != wndProc_){
                if(!wndProc_(event_)){
                    return false;
                }
            }

        } //for(;;)
#endif
#ifdef LG3_GFX_USE_XLIB
        while(XPending(display_)){
            XNextEvent(display_, &event_);
            switch(event_.type){
            case ConfigureNotify:
                break;
            case DestroyNotify:
                return false;
            case ClientMessage:
                if(static_cast<u32>(event_.xclient.data.l[0]) == delete_){
                    return false;
                }
                break;
            }
            if(LG3_NULL != wndProc_){
                if(!wndProc_(event_)){
                    return false;
                }
            }
        } //while(XPending
        return true;
#endif
    }


#ifdef LG3_GFX_USE_WIN32
    HINSTANCE Window::getInstance()
    {
        return instance_;
    }

    bool Window::peekEvent(HWND hDlg)
    {
        while(PeekMessage(&event_, LG3_NULL, 0, 0, PM_NOREMOVE)){
            if(event_.message==WM_QUIT)
                return false;

            if(LG3_NULL != hDlg && IsDialogMessage(hDlg, &event_)){
                continue;
            }

            if(0<GetMessage(&event_, hDlg, 0, 0)){
                TranslateMessage(&event_);
                DispatchMessage(&event_);
            } else{
                return false;
            }
        }
        return true;
    }

    bool Window::isFullscreen() const
    {
        return isFullscreen_;
    }

#ifdef LG3_GFX_API_D3D12
    void Window::toggleFullscreen(IDXGISwapChain* swapChain)
    {
        if(isFullscreen_){
            SetWindowLong(handle_, GWL_STYLE, windowStyle_);

            SetWindowPos(
                handle_,
                HWND_NOTOPMOST,
                windowRect_.left,
                windowRect_.top,
                windowRect_.right - windowRect_.left,
                windowRect_.bottom - windowRect_.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);
            ShowWindow(handle_, SW_NORMAL);

        } else{
            GetWindowRect(handle_, &windowRect_);
            SetWindowLong(handle_, GWL_STYLE, windowStyle_ & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

            RECT fullscreenRect;
            if(LG3_NULL != swapChain){
                IDXGIOutput* output = LG3_NULL;
                if(FAILED(swapChain->GetContainingOutput(&output))){
                    return;
                }
                DXGI_OUTPUT_DESC desc;
                if(FAILED(output->GetDesc(&desc))){
                    output->Release();
                    return;
                }
                output->Release();
                fullscreenRect = desc.DesktopCoordinates;
            } else{
                DEVMODE devMode = {};
                devMode.dmSize = sizeof(DEVMODE);
                EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);

                fullscreenRect = {
                    devMode.dmPosition.x,
                    devMode.dmPosition.y,
                    devMode.dmPosition.x + static_cast<LONG>(devMode.dmPelsWidth),
                    devMode.dmPosition.y + static_cast<LONG>(devMode.dmPelsHeight)
                };
            }
            SetWindowPos(
                handle_,
                HWND_TOPMOST,
                fullscreenRect.left,
                fullscreenRect.top,
                fullscreenRect.right,
                fullscreenRect.bottom,
                SWP_FRAMECHANGED | SWP_NOACTIVATE);


            ShowWindow(handle_, SW_MAXIMIZE);
        }

        isFullscreen_ = !isFullscreen_;
    }
#endif

    void Window::setWindowTopMost(bool topMost)
    {
        RECT rect;
        GetWindowRect(handle_, &rect);

        SetWindowPos(
            handle_,
            topMost? HWND_TOPMOST : HWND_NOTOPMOST,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            SWP_FRAMECHANGED | SWP_NOACTIVATE);
    }

#endif

#ifdef LG3_GFX_USE_XCB
    xcb_connection_t* Window::getConnection()
    {
        return connection_;
    }
#endif

#ifdef LG3_GFX_USE_XLIB
    Display* Window::getDisplay()
    {
        return display_;
    }
#endif

    namespace
    {
#ifdef LG3_GFX_USE_WIN32
        static LRESULT CALLBACK WndProc(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam)
        {
            switch(msg)
            {
            case WM_SYSCOMMAND:
            {
                switch(wParam)
                {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0;
                }
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }

            //case WM_CLOSE:
            //    DestroyWindow(hWnd);
            //    break;

            case WM_PAINT:
                ValidateRect(hWnd, LG3_NULL);
                return 0;

            case WM_DESTROY:
                PostQuitMessage(0);
                return 1;

            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
#endif
    }
}
#endif //LGFX_WINDOW_IMPLEMENTATION_DONE
#endif //LGFX_WINDOW_IMPLEMENTATION
