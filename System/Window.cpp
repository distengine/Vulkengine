#include "Window.h"

Window::Window()
{
	m_Window = nullptr;
	SDL_InitSubSystem(SDL_INIT_VIDEO);
}


Window::~Window()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}

void Window::CreateSDLWindow(const std::string& windowTitle, int width, int height)
{
	m_Window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_VULKAN);
}

SDL_SysWMinfo Window::GetWindowHandle()
{
	SDL_SysWMinfo wmInfo;
	SDL_version version;
	SDL_GetVersion(&version);
	wmInfo.version = version;
	SDL_GetWindowWMInfo(m_Window, &wmInfo);
	return wmInfo;
}

bool Window::IsOpen()
{
	if (m_Window == nullptr)
		return false;
	else
		return true;
}

void Window::Close()
{
	m_Window = nullptr;
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	SDL_Quit();
}