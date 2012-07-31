This directory is the starter project that is included with SDL 2.0.  Also
included with SDL 2.0, is a README.android file which gives general
instructions on how to adapt and extend it.  You may need to refer to that
file to get details this file assumes you already have.

These instructions do not attempt to teach you how to set up the SDK, NDK and
other general Android development aspects.

Instructions
------------

1. Populate the jni/ directory.
   - Create a 'jni/libtcod/' directory.  A copy of the parent directory.
   - Create a 'jni/SDL2/' directory.  It should contain a copy of the SDL 2.0
     source code.  Perhaps revision efd48af40ec3.
	 - Ensure you copy 'SDL2/include/SDL_config_android.h' over
	   'SDL2/include/SDL_config.h'.
   - Create a 'jni/png/' directory.  Perhaps the 1.4.6beta06 release.
2. Copy 'jni/libtcod/data' to 'assets/data'.
2. Modify 'local.properties' to point to your local SDL directory.
3. Type 'ndk-build'.  You should see a successful compilation.

From here, it's up to you.

You might get an Android configured Eclipse installation working, then import
this directory.  But be sure not to import the 'android-project' subdirectories
of 'jni/libtcod' and 'jni/SDL2'.

