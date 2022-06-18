#ifndef HEADER_BINRAWVIEW_HPP
#define HEADER_BINRAWVIEW_HPP

#include <SDL.h>
#include <iosfwd>

namespace binrawview {

class Binrawview
{
public:
  Binrawview(SDL_Window* window, std::istream& fin);
  ~Binrawview();

  void run();

private:
  void update_image();
  void process_events();
  void queue_redraw();

private:
  bool m_quit;
  SDL_Window* m_window;
  SDL_Surface* m_screen;
  SDL_Surface* m_image;
  std::istream& m_fin;

  std::streamoff m_start_offset;
  int m_mapped_width;
  bool m_queue_redraw;

public:
  Binrawview(Binrawview const&) = delete;
  Binrawview& operator=(Binrawview const&) = delete;
};

} // namespace binrawview

#endif

/* EOF */
