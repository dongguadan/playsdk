#ifndef _SDLRENDER_H
#define _SDLRENDER_H

#include "common.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "SDL.h"
#include "SDL_thread.h"

#ifdef __cplusplus
}
#endif

class CSDLRender
{
public:
    CSDLRender();
    virtual ~CSDLRender();
	int         InitSDL(void *hWnd);
	int         PutFrame(YUVFrame *frame, unsigned long srcWidth, unsigned long srcHeight, unsigned long long pts);
    int         Flush();
	int         CreateOffScreen(unsigned long w, unsigned long h);
	void        Stop();
private:
    SDL_Window   *m_window;
	SDL_Renderer *m_ren;
	SDL_Texture  *m_texture;
	SDL_Rect     m_rect;
};

#endif
