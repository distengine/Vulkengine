#pragma once
#include <string>
#include <SDL\SDL.h>
#include <SDL\SDL_syswm.h>

class Window
{
public:
	Window();
	~Window();
	// Creates a SDL Window with a title, at a given size. The Window is centered
	void CreateSDLWindow(const std::string& windowTitle, int width, int height);
	// Returns a SDL_SysWMinfo handle which we can use to recieve the HWND and HANDLE of the SDL window for Surface creation
	SDL_SysWMinfo GetWindowHandle();
	// Poll the windows state
	bool IsOpen();
	// Closes the window, Quits SDL, and eventually calls the destructor since the window will only close if the game ends
	void Close();
private:
	SDL_Window* m_Window;
};