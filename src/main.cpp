#include <iostream>
#include <stdio.h>
#include <SDL.h>
#include <stdint.h>
#include <fstream>

int main(int argc, char** argv)
{
  int ret = SDL_Init(SDL_INIT_VIDEO);
  if (ret < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  else
  {
    SDL_Window* window = SDL_CreateWindow("binrawview",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    else
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

      SDL_Surface* screen = SDL_GetWindowSurface(window);
      SDL_Surface* image = SDL_CreateRGBSurface(0, 800, 600, 24,
                                                rmask, gmask, bmask, 0);

      std::ifstream fin("test.dat", std::ios::binary);

      int mapped_width = 256;
      int start_offset = 0;
      bool quit = false;
      while(!quit)
      {
        SDL_LockSurface(image);
        uint8_t* p = static_cast<uint8_t*>(image->pixels);
        fin.seekg(start_offset);
        for(int y = 0; y < image->h; ++y)
        {
          /*
          for(int x = 0; x < image->w; ++x)
          {
            p[y * image->pitch + x * image->format->BytesPerPixel + 0] = 255;
            p[y * image->pitch + x * image->format->BytesPerPixel + 1] = 255;
            p[y * image->pitch + x * image->format->BytesPerPixel + 2] = 255;
            p[y * image->pitch + x * image->format->BytesPerPixel + 3] = 255;
          }
          */

          fin.read(reinterpret_cast<char*>(p + y * image->pitch), mapped_width * image->format->BytesPerPixel);
          // fin.gcount();
        }
        SDL_UnlockSurface(image);

        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_BlitSurface(image, NULL, screen, NULL);

        SDL_UpdateWindowSurface(window);

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
          switch(event.type)
          {
            case SDL_QUIT:
              std::cout << "quit" << std::endl;
              quit = true;
              break;

            case SDL_WINDOWEVENT:
              break;

            case SDL_KEYUP:
              break;

            case SDL_KEYDOWN:
              switch(event.key.keysym.sym)
              {
                case SDLK_ESCAPE:
                  quit = true;
                  break;

                case SDLK_KP_PLUS:
                  std::cout << "width+" << mapped_width << std::endl;
                  mapped_width += 1;
                  break;

                case SDLK_KP_MINUS:
                  std::cout << "width- " << mapped_width << std::endl;
                  mapped_width -= 1;
                  break;

                case SDLK_PAGEUP:
                  std::cout << "offset: " << start_offset << std::endl;
                  start_offset -= 256 * 256;
                  break;

                case SDLK_PAGEDOWN:
                  std::cout << "offset: " << start_offset << std::endl;
                  start_offset += 256 * 256;
                  break;
              }
              break;

            default:
              std::cout << "unknown event:" << event.type << std::endl;
              break;
          }
        }
      }
      SDL_FreeSurface(image);
      //SDL_Delay(2000);
      SDL_DestroyWindow(window);
      SDL_Quit();
    }
  }

  return 0;
}

/* EOF */
