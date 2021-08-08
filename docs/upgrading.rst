
Upgrading
=========

1.6 -> latest 1.x
-----------------

New project should use the :ref:`getting-started` guide so that they can start using contexts right way.
Older projects will have to convert to contexts over a series of steps.

Libtcod's systems have been deprecated in favor of using `SDL2`_ directly for events.
:any:`TCOD_Context::convert_event_coordinates` is the recommended way to convert pixel coordinates to tiles.
:any:`tcod::sdl2::process_event` might work better for converting old code to use the new system.

.. code-block:: cpp

  // tcod::ContextPtr context = tcod::new_context(...);  // For code using contexts.
  // TCOD_Context* context = TCOD_sys_get_internal_context();  // For code still using the old API.

  while (true) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      // context->convert_event_coordinates(event);  // Optional, converts ALL pixel coordinates into tile coordinates.
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


1.5 -> 1.6
----------

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
