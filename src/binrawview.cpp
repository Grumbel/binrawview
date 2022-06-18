#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <stdexcept>

#include "binrawview.hpp"

namespace binrawview {

Binrawview::Binrawview(SDL_Window* window, std::istream& fin) :
  m_quit(false),
  m_window(window),
  m_screen(SDL_GetWindowSurface(window)),
  m_fin(fin),
  m_start_offset(0),
  m_mapped_width(1024),
  m_queue_redraw(true)
{
  Uint32 rmask, gmask, bmask, amask;
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
  {
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
  }
  else
  {
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
  }

  m_image = SDL_CreateRGBSurface(0, 1024, 1024, 24, rmask, gmask, bmask, 0);
}

Binrawview::~Binrawview()
{
  SDL_FreeSurface(m_image);
  SDL_DestroyWindow(m_window);
}

void
Binrawview::process_events()
{
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
        std::cout << "quit" << std::endl;
        m_quit = true;
        break;

      case SDL_WINDOWEVENT:
        break;

      case SDL_KEYUP:
        break;

      case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            m_quit = true;
            break;

          case SDLK_RIGHT:
          case SDLK_KP_PLUS:
            std::cout << "width+ " << m_mapped_width << std::endl;
            if (event.key.keysym.mod & KMOD_SHIFT) {
              m_mapped_width += 1;
            } else {
              m_mapped_width += 16;
            }
            queue_redraw();
            break;

          case SDLK_LEFT:
          case SDLK_KP_MINUS:
            std::cout << "width- " << m_mapped_width << std::endl;
            if (event.key.keysym.mod & KMOD_SHIFT) {
              m_mapped_width -= 1;
            } else {
              m_mapped_width -= 16;
            }
            queue_redraw();
            break;

          case SDLK_UP:
          case SDLK_PAGEUP:
            std::cout << "offset: " << m_start_offset << std::endl;
            m_start_offset -= 256 * 256;
            queue_redraw();
            break;

          case SDLK_DOWN:
          case SDLK_PAGEDOWN:
            std::cout << "offset: " << m_start_offset << std::endl;
            m_start_offset += 256 * 256;
            queue_redraw();
            break;
        }
        break;

      default:
        //std::cout << "unknown event:" << event.type << std::endl;
        break;
    }
  }
}

void
Binrawview::update_image()
{
  std::cout << "update_image()" << std::endl;
  m_start_offset = std::max(m_start_offset, std::streamoff(0));

  m_fin.clear();
  m_fin.seekg(m_start_offset);

  SDL_FillRect(m_image, nullptr, SDL_MapRGB(m_image->format, 64, 64, 64));
  SDL_LockSurface(m_image);
  uint8_t* const p = static_cast<uint8_t*>(m_image->pixels);
  for(int y = 0; y < m_image->h; ++y)
  {
    std::streamsize const len = std::min(m_mapped_width * m_image->format->BytesPerPixel,
                                         m_image->pitch);
    m_fin.read(reinterpret_cast<char*>(p + y * m_image->pitch), len);
  }
  SDL_UnlockSurface(m_image);
}

void
Binrawview::queue_redraw()
{
  m_queue_redraw = true;
}

void
Binrawview::run()
{
  m_quit = false;
  while(!m_quit)
  {
    if (m_queue_redraw) {
      update_image();
      m_queue_redraw = false;
    }

    SDL_FillRect(m_screen, NULL, SDL_MapRGB(m_screen->format, 0, 0, 0));
    SDL_BlitSurface(m_image, NULL, m_screen, NULL);

    SDL_UpdateWindowSurface(m_window);

    process_events();
    SDL_Delay(20);
  }
}

} // namespace binrawview

/* EOF */
