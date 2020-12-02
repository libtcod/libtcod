.. _getting-started:

Getting Started
===============

Here is a simple example of setting up a libtcod context in C++ without using
deprecated functions.

.. code-block:: c++

    #include <libtcod.h>
    #include <SDL2.h>

    int main(int argc, char* argv[]) {
      tcod::ConsolePtr console = tcod::new_console(80, 25);  // Main console.

      // Configure the context.
      TCOD_ContextParams params = {};
      params.tcod_version = TCOD_COMPILEDVERSION;  // This is required.
      params.columns = console->w;  // Derive the window size from the console size.
      params.rows = console->h;
      params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
      params.vsync = true;
      params.argc = argc;  // This allows some user-control of the context.
      params.argv = argv;
      tcod::ContextPtr context = tcod::new_context(params);

      while (1) {  // Game loop.
        TCOD_console_clear(console.get());
        tcod::print(*console, 0, 0, "Hello World", nullptr, nullptr, TCOD_BKGND_NONE, TCOD_LEFT);
        context->present(*console);  // Updates the visible display.

        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
          case SDL_QUIT:
            return 0;  // Exit.
        }
      }
    }
