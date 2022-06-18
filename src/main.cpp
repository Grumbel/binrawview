#include <cstdlib>
#include <fstream>
#include <iostream>

#include <fmt/format.h>

#include "binrawview.hpp"

struct Options
{
  std::string filename = {};
};

Options parse_options(int argc, char** argv)
{
  Options opts;

  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
    exit(EXIT_FAILURE);
  }

  opts.filename = argv[1];

  return opts;
}

int main(int argc, char** argv) try
{
  Options const opts = parse_options(argc, argv);

  int ret = SDL_Init(SDL_INIT_VIDEO);
  if (ret < 0) {
    throw std::runtime_error(fmt::format("SDL could not initialize! SDL_Error: {}",  SDL_GetError()));
  }
  atexit(SDL_Quit);

  SDL_Window* const window = SDL_CreateWindow("binrawview",
                                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                              1024, 1024,
                                              SDL_WINDOW_SHOWN);
  if (!window) {
    throw std::runtime_error(fmt::format("Window could not be created! SDL_Error: {}", SDL_GetError()));
  }

  std::cout << "Opening " << opts.filename << std::endl;
  std::ifstream fin(opts.filename, std::ios::binary);
  if (!fin) {
    throw std::runtime_error(fmt::format("{}: opening failed: ", opts.filename));
  }

  binrawview::Binrawview app(window, fin);
  app.run();
} catch (std::exception& err) {
  std::cerr << "Error: " << err.what() << std::endl;
}

/* EOF */
