#include "sdlrender.h"

CSDLRender::CSDLRender()
{
	m_window  = NULL;
	m_ren     = NULL;
	m_texture = NULL;
	m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = 0;
    m_rect.h = 0;
}

CSDLRender::~CSDLRender()
{
	
}

int CSDLRender::InitSDL(void *hWnd)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        printf("Could not initialize SDL -%s\n", SDL_GetError());
        return -1;
    }
	return 0;
}

int CSDLRender::CreateOffScreen(unsigned long w, unsigned long h)
{
    m_window = SDL_CreateWindow("MyPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              w, h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if (m_window == NULL)
    {
        printf("Could not create window -%s\n", SDL_GetError());
        return -1;
    }


    m_ren = SDL_CreateRenderer(m_window, -1, 0);
    if (m_ren == NULL)
    {
        printf("Could not create render -%s\n", SDL_GetError());
        return -1;
    }

    m_texture = NULL;
    m_texture = SDL_CreateTexture(m_ren, SDL_PIXELFORMAT_IYUV,
                               SDL_TEXTUREACCESS_STREAMING, w, h);

    m_rect.x = 0;
    m_rect.y = 0;
    m_rect.w = w;
    m_rect.h = h;
	return 0;
}

void CSDLRender::Stop()
{
	
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        SDL_Quit();
        exit(0);
        break;
    default:
        break;
    }
    SDL_DestroyTexture(m_texture);
	return;
}

int CSDLRender::PutFrame(YUVFrame *frame, unsigned long srcWidth, unsigned long srcHeight, unsigned long long pts)
{
	if (frame == NULL)
	{
		return -1;
	}
	SDL_UpdateYUVTexture(m_texture, &m_rect, frame->data[0], frame->linesize[0],
						 frame->data[1], frame->linesize[1], frame->data[2], frame->linesize[2]);
	SDL_RenderClear(m_ren);
	SDL_RenderCopy(m_ren, m_texture, &m_rect, &m_rect);
	SDL_RenderPresent(m_ren);
	return 0;
}

int CSDLRender::Flush()
{
	return 0;
}