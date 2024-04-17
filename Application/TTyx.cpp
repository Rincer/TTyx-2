// TTyx.cpp : Defines the entry point for the application.
#include "stdafx.h"
#include "..\Windows\Window.h"
#include "..\Memory\MemoryManager.h"
#include "..\GraphicsDevice\Vulkan\DeviceVK.h"
#include "..\Scene\Scene.h"
#include "..\Rendering\Renderer.h"
#include "..\Testing\Testing.h"

#include "TTyx.h"

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {    
        case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }

//        case WM_SIZE:
//        {
//            break;
//        }

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

//--------------------------------------------------------------------------------------
static TTyx sTTyx; // One global instance of the app

//--------------------------------------------------------------------------------------
void TTyx::Startup(HINSTANCE Instance, int CmdShow)
{
    m_Wnd.Initialize(WndProc, Instance, CmdShow, L"TTyx", L"TTyx", 1024, 768);
    CWindow::SetCurrentViewWnd(m_Wnd.GetWnd());
    m_Renderer.Initialize();
    m_Scene.initialize(&m_Renderer);
 //   Testing::RunTests();
}

//--------------------------------------------------------------------------------------
void TTyx::Shutdown()
{
    m_Renderer.Dispose();
}

//--------------------------------------------------------------------------------------
void TTyx::MainLoop()
{
    m_Scene.draw(&m_Renderer);
    m_Renderer.Execute();
    m_FrameCounter++;
}

int APIENTRY _tWinMain(	HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow)
{	
    lpCmdLine;
    hPrevInstance;
    sTTyx.Startup(hInstance, nCmdShow);
    MSG Msg;
    // Main message loop:
    bool bGotMsg;
    Msg.message = WM_NULL;
    PeekMessage( &Msg, NULL, 0U, 0U, PM_NOREMOVE );
    while( WM_QUIT != Msg.message )
    {
        bGotMsg = ( PeekMessage( &Msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );
        if( bGotMsg )
        {
			TranslateMessage( &Msg );
			DispatchMessage( &Msg );
        }
        else
        {
            sTTyx.MainLoop();
        }
    }
    sTTyx.Shutdown();
    atexit(CMemoryManager::ShutDown);
    return (int) Msg.wParam;
}



