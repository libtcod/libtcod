This directory is the starter project that is included with SDL 2.0.  Also
included with SDL 2.0, is a README.android file which gives general
instructions on how to adapt and extend it.  You may need to refer to that
file to get details this file assumes you already have.

These instructions do not attempt to teach you how to set up the SDK, NDK and
other general Android development aspects.

Instructions
------------

1. Populate the 'jni/' directory.  'ndk-build' does not care what you name
   these directories, although 'jni/src/Android.mk' refers to them by name
   so if you go rogue and do your own naming.. sort that out:
   - Create a 'jni/libtcod/' directory.  A copy of the parent directory.
   - Create a 'jni/SDL2/' directory.  It should contain a copy of the SDL 2.0
     source code.  Perhaps revision efd48af40ec3.
	 - Ensure you copy 'SDL2/include/SDL_config_android.h' over
	   'SDL2/include/SDL_config.h'.
   - Create a 'jni/png/' directory.  Perhaps the 1.4.6beta06 release.
2. Ensure the data files that 'samples_c.c' needs (fonts for instance) are
   bundled in the APK package and accessible by the app:
   - Copy 'jni/libtcod/data' to 'assets/data'.
2. Modify 'local.properties' to point to your local SDL directory.
3. Type 'ndk-build all'.  You should see a successful compilation.

From here, it's up to you.  Perhaps get it set up in Eclipse?

Eclipse Instructions
--------------------

1. File -> Import.
2. Select Android / Existing Android Code Into Workspace.
3. Browse to your 'android-project' directory.
4. This should give you three entries in the lower list pane:
   - The directory you selected.
   - Your 'jni/libtcod/android-project' directory.
   - Your 'jni/SDL2/android-project' directory.
   Deselect all except the first.  The last two are just the template project
   directories.
5. Now you should be able to right click on the newly added
   "org.libsdl.app.SDLActivity" in the "Project Explorer" pane.  Then select
   the "Debug As -> Android Application" submenu and the app should launch
   in the emulator directly.

