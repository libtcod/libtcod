
.. _SDL2: https://www.libsdl.org/index.php

Upgrading
=========

1.5.x -> 1.6.x
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

.. code-block:: python
   :caption: Python

   if key.c == "+":
       pass # Handle any key that displays a plus.

In libtcod 1.6.x
^^^^^^^^^^^^^^^^

With SDL2, the raw key-presses still occur, but they are fundamentally linked to the keyboard of the user.  Now there will still be an event where it says ``SHIFT`` and ``=`` are pressed, but the event will always be for the unmodified character ``=``.  The unicode text arrives in a new kind of event, and getting it requires explicitly checking that the event is the new text event, and then looking for the value in the relevant ``text`` field for the language being used.

.. code-block:: c
   :caption: C / C++

   if (key->vk == TCODK_TEXT)
       if (key.text[0] == '+') {
           ; /* Handle any key that displays a plus. */
       }

.. code-block:: python
   :caption: Python

   if key.vk == libtcod.KEY_TEXT:
       if key.text == "+":
           pass # Handle any key that displays a plus.

Still confused?
^^^^^^^^^^^^^^^

Run your code from a terminal or DOS window and print out the event attributes/fields and look at what is going on.  Have your code print out the modifiers, the keycode, the character, the text, and then run it and try pressing some keys. It will be much faster than posting "I don't understand" or "Can someone explain" somewhere and waiting for a response.
