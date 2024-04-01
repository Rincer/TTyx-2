#ifndef _TTYX_H_
#define _TTYX_H_

// The main TTyx
class TTyx
{
	public:
		void MainLoop();
		void Startup(HINSTANCE Instance, int CmdShow);
		void Shutdown();

	private:
		unsigned int m_FrameCounter;
		CWindow m_Wnd; // main window for the app.			
		Scene m_Scene;
		Renderer m_Renderer;
};

extern TTyx* gTTyx;

#endif