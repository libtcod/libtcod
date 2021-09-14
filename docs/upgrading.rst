
Upgrading
=========

Porting to 1.19
---------------

Any new project should use the :ref:`getting-started` guide so that they can start using contexts right way.
Older projects will have to convert to contexts over a series of steps.

The old way of initializing a global state with :any:`TCOD_console_init_root` or ``TCODConsole::initRoot`` has been deprecated.
These have been replaced with a public context object.

New tileset API
^^^^^^^^^^^^^^^

You can switch away from :any:`TCOD_console_set_custom_font` before using contexts by using the :any:`TCOD_set_default_tileset` function.

.. code-block:: cpp

    // The old way to load tilesets:
    // TCOD_console_set_custom_font("terminal8x8_gs_tc.png", TCOD_FONT_LAYOUT_TCOD, 32, 8);

    // You can update old code to use this new method of loading tilesets before using contexts properly.
    tcod::TilesetPtr tileset = tcod::load_tilesheet("terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);
    TCOD_set_default_tileset(tileset.get());

    // This is deprecated, but will work with the above tileset.
    TCOD_console_init_root(80, 25, "Window title", false, TCOD_RENDERER_OPENGL2);

Using the new console functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is less state with the new functions.
Instead of setting colors before calling the print functions you now pass the colors and flags needed for each print call.

.. code-block:: cpp

  static constexpr auto WHITE = TCOD_ColorRGB{255, 255, 255};

  TCODConsole* console = new TCODConsole(80, 25);  // Deprecated.

  tcod::print(
    static_cast<TCOD_Console&>(*console),  // TCODConsole& can be static cast to TCOD_Console&.
    {0, 0},  // Coordinates are passed together.
    tcod::stringf("%s %s", "Hello", "world"),  // Printf-like strings are encapsulated in tcod::stringf.
    &WHITE,  // Colors are passed by pointer, this is so that you can pass nullptr.
    nullptr,  // Passing nullptr here leaves the background color unchanged.
  )

  TCOD_ColorRGB bg = TCOD_console_get_char_background(TCODConsole::root->get_data());
  tcod::draw_rect(
    static_cast<TCOD_Console&>(*TCODConsole::root),  // This cast also works with TCODConsole::root.
    {0, 0, 20, 1},  // {left, top, width, height}
    0x2500,  // "─" BOX DRAWINGS LIGHT HORIZONTAL.  Unicode is expected for character codes.
    &WHITE,
    &bg,
  )

Adapting to contexts and a rootless console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before initializing a context properly you can access both the context and the root console.
This lets you use context methods like :any:`TCOD_Context::present` easily in older code.
This also lets you use the root console on functions which can't accept `nullptr`.

.. code-block:: cpp

    TCOD_console_init_root(80, 25, "Window title", false, TCOD_RENDERER_OPENGL2);  // Deprecated.

    // Get a temporary non-owning pointer to the context made by TCOD_console_init_root or TCODConsole::initRoot.
    TCOD_Context* context = TCOD_sys_get_internal_context();

    // Get a temporary non-owning pointer to the root console made by TCOD_console_init_root or TCODConsole::initRoot.
    TCOD_Console* root_console = TCOD_sys_get_internal_console();
    // From now on use root_console instead of NULL, make this global if you have to.

    // Using the root console with the context is similar to calling TCOD_console_flush() with some exceptions.
    context->present(*root_console);  // Or in C: TCOD_context_present(context, root_console, NULL)

Using the context present function like this will break some functions which say they're not compatible with contexts.
Most importantly any timing-related functions will need to be updated.  See `Timing`_ below.

Window manipulation
^^^^^^^^^^^^^^^^^^^

With the temporary context from the previous step or with :any:`TCOD_sys_get_sdl_window` you can access the ``SDL_Window`` pointer.
You use this to replace several window-related functions such as :any:`TCOD_console_set_fullscreen`, :any:`TCOD_console_is_active` or :any:`TCOD_console_set_window_title`.
See the `SDL2 window documentation <https://wiki.libsdl.org/CategoryVideo>`_ for what you can do with the ``SDL_Window`` pointer.

.. code-block:: cpp

    TCOD_console_init_root(80, 25, "Window title", false, TCOD_RENDERER_OPENGL2);  // Deprecated.
    TCOD_Context* context = TCOD_sys_get_internal_context();
    SDL_Window* sdl_window = context->get_sdl_window();

    if (sdl_window) {
      SDL_SetWindowTitle(sdl_window, "New title");
      if (SDL_GetWindowFlags(sdl_window) & SDL_WINDOW_INPUT_FOCUS) {}
    }

Event systems
^^^^^^^^^^^^^

Libtcod's event systems have been deprecated in favor of using `SDL2`_ directly for events.
:any:`TCOD_Context::convert_event_coordinates` is the recommended way to convert pixel coordinates to tiles.
:any:`tcod::sdl2::process_event` might work better for converting old code to use the new system.

.. code-block:: cpp

  // tcod::ContextPtr context = tcod::new_context(...);  // For code using contexts.
  // TCOD_Context* context = TCOD_sys_get_internal_context();  // For code still using the old API.

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      // context->convert_event_coordinates(event);  // Optional, converts mouse pixel coordinates into tile coordinates.
      switch (event.type) {
        case SDL_QUIT:
          std::exit(EXIT_SUCCESS);
          break;
        case SDL_KEYDOWN: {
          TCOD_mouse_t key;
          tcod::sdl2::process_event(event, key);  // Convert a SDL key to a libtcod key event, to help port older code.
          switch (event.key.keysym.sym) {
            case SDLK_EQUALS: // equals/plus key symbol.
              if (event.key.keysym.mod & KMOD_SHIFT) {
                // Handle plus key.
              }
              break;
            default:
              break;
          }
        } break;
        case SDL_MOUSEBUTTONDOWN: {
          TCOD_mouse_t mouse;
          tcod::sdl2::process_event(event, mouse);  // Convert SDL into a libtcod mouse event, to help port older code.
          // The above expects pixel coordinates.  So you can't use convert_event_coordinates before process_event.
          context->convert_event_coordinates(event);
        } break;
        default:
          break;
      }
    }
  }

Timing
^^^^^^

All of the libtcod timing functions have been deprecated.
Many will stop working once you switch to using contexts.
Instead you should use :any:`tcod::Timer` and SDL2's timing functions.
Remember that you have to add ``#include <libtcod/timer.h>`` to access :any:`tcod::Timer`, this also requires the SDL2 headers.

.. code-block:: cpp

  int desired_fps = 30;
  auto timer = tcod::Timer();

  while (1) {
    uint32_t current_time_ms = SDL_GetTicks();
    float current_time = static_cast<float>(current_time_ms) / 1000.f;
    float delta_time = timer.sync(desired_fps);
    // ...
  }

Switching to contexts
^^^^^^^^^^^^^^^^^^^^^

With all the above done you can now switch away from :any:`TCOD_console_init_root` and start using :any:`TCOD_ContextParams` and :any:`tcod::new_context`.

.. code-block:: cpp

    #include <libtcod.h>
    #include <SDL2.h>

    int main(int argc, char* argv[]) {
      tcod::ConsolePtr root_console = tcod::new_console(80, 25);
      tcod::TilesetPtr tileset = tcod::load_tilesheet("terminal8x8_gs_tc.png", {32, 8}, tcod::CHARMAP_TCOD);

      TCOD_ContextParams params{};
      params.tcod_version = TCOD_COMPILEDVERSION;
      params.console = root_console.get();
      params.window_title = "Window title";
      params.sdl_window_flags = SDL_WINDOW_RESIZABLE;
      params.vsync = true;
      params.argc = argc;
      params.argv = argv;
      params.tileset = tileset.get();

      tcod::ContextPtr context = tcod::new_context(params);

      while (1) {
        context->present(*root_console);

        SDL_Event event;
        while (SDL_PollEvent(&event)){
          switch (event.type) {
            case SDL_QUIT:
              return 0;  // Exit.
          }
        }
      }
    }

Porting to 1.6
--------------

The largest and most influential change to libtcod, between versions 1.5.2 and 1.6.0, was the move to replace SDL with `SDL2`_.  SDL2 made many extensive changes to concepts used in SDL.  Only one of these changes, the separation of text and key events, required a change in the libtcod API requiring users to update their code in the process of updating the version of libtcod they use.

When a user presses a key, they may be pressing ``SHIFT`` and ``=``.  On some keyboards, depending on the user's language and location, this may show ``+`` on the screen.  On other user's keyboards, who knows what it may show on screen.  SDL2 changes the way "the text which is displayed on the user's screen" is sent in key events.  This means that the key event for ``SHIFT`` and ``=`` will be what happens for presses of both ``+`` and ``=`` (for user's with applicable keyboards), and there will be a new text event that happens with the displayed ``+``.

In libtcod 1.5.x
^^^^^^^^^^^^^^^^

SDL would when sending key events, provide the unicode character for the key event, ready for use.  This meant that if the user happened to be using a British keyboard (or any that are similarly laid out), and pressed ``SHIFT`` and ``=``, the event would be for the character ``+``.

.. code-block:: c
    :caption: C / C++

    if (key->c == '+') {
        /* Handle any key that displays a plus. */
    }

In libtcod 1.6.x
^^^^^^^^^^^^^^^^

With SDL2, the raw key-presses still occur, but they are fundamentally linked to the keyboard of the user.  Now there will still be an event where it says ``SHIFT`` and ``=`` are pressed, but the event will always be for the unmodified character ``=``.  The unicode text arrives in a new kind of event, and getting it requires explicitly checking that the event is the new text event, and then looking for the value in the relevant ``text`` field for the language being used.

.. code-block:: c
   :caption: C / C++

   if (key->vk == TCODK_TEXT)
       if (key.text[0] == '+') {
           ; /* Handle any key that displays a plus. */
       }

Still confused?
^^^^^^^^^^^^^^^

Run your code from a terminal or DOS window and print out the event attributes/fields and look at what is going on.  Have your code print out the modifiers, the keycode, the character, the text, and then run it and try pressing some keys. It will be much faster than posting "I don't understand" or "Can someone explain" somewhere and waiting for a response.


.. _SDL2: https://www.libsdl.org/index.php
