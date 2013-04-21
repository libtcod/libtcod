/*
* libtcod 1.6.0
* Copyright (c) 2008,2009,2010,2012,2013 Jice & Mingos
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of Jice or Mingos may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY JICE AND MINGOS ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL JICE OR MINGOS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* This file contains no code. It's only an input for doctcod */

/**
	@PageName compile_libtcod
	@PageCategory Howtos
	@PageTitle How to compile libtcod
	@PageDesc This page contains howtos about how to get the latest libtcod source code and compile it.
*/

/**
	@PageName compile_libtcod_mingw
	@PageFather compile_libtcod
	@PageTitle On Windows with Mingw
	@PageDesc <h4>Mingw installation</h4>
	Download the latest version of Mingw from this adress :
	<a href="http://sourceforge.net/projects/mingw/files/">http://sourceforge.net/projects/mingw/files/</a>

	The latest installer should be at the top of the page with a name starting with mingw-get-inst..

	<img src="mingw1.png" />

	Download and run the program. Follow the installation steps. Be sure to check the "Use pre-packaged repository" option :

	<img src="mingw2.png" />

	The latest version might be less stable and might not work with a precompiled libtcod.

	When you arrive at the component selection screen, check C compiler, C++ compiler and MSys system :

	<img src="mingw3.png" />

	Keep on following the installation steps until the installation is finished. Now you have a "Mingw Shell" program in your start menu. This is the terminal you will use to compile and debug your game.

	<h4>TortoiseHg installation</h4>
	In order to get the latest version of libtcod, you need a mercurial client.
	Go to the download page and grab the client corresponding to your version of Windows :
	<a href="http://tortoisehg.bitbucket.org/download/index.html">http://tortoisehg.bitbucket.org/download/index.html</a>

	Follow the installation wizard using the default configuration. Once the installation is finished, restart your computer.

	Now you should be able to use mercurial (hg) from the Mingw Shell. To check if everything is ok, start a shell and type "which hg" :

	<img src="mingw4.png" />

	<h4>Getting libtcod source code</h4>
	In Mingw Shell, type :
	<pre name="code" class="brush: bash">hg clone https://bitbucket.org/jice/libtcod</pre>

	This might take some time so grab a beer. Once it's finished, a libtcod directory has been created.
	You can check the documentation (the same you're currently reading) in libtcod/doc/index2.html.
	The headers are in libtcod/include.
	The source code in libtcod/src.

	<h4>Compiling libtcod</h4>
	Go in libtcod's main directory :
	<pre name="code" class="brush: bash">cd libtcod</pre>
	And start the compilation :
	<pre name="code" class="brush: bash">make -f makefiles/makefile-mingw</pre>
	The compilation make take a few seconds depending on your CPU speed. Once it's finished, compile the samples :
	<pre name="code" class="brush: bash">make -f makefiles/makefile-samples-mingw</pre>
	Check that everything is ok by running the samples :
	<pre name="code" class="brush: bash">./samples_cpp</pre>
*/

/**
	@PageName compile_libtcod_linux
	@PageFather compile_libtcod
	@PageTitle On Linux
	@PageDesc  <h4>Linux compilation</h4>
	On a freshly installed Ubuntu :
	Get the tools :
	<pre name="code" class="brush: bash">sudo apt-get install gcc g++ make upx electric-fence libsdl1.2-dev mercurial</pre>

	Get the latest sources :
	<pre name="code" class="brush: bash">hg clone https://bitbucket.org/jice/libtcod</pre>

	Compile the library :
	<pre name="code" class="brush: bash">cd libtcod/</pre>
	<pre name="code" class="brush: bash">make -f makefiles/makefile-linux clean all</pre>

	Compile the samples :
	<pre name="code" class="brush: bash">make -f makefiles/makefile-samples-linux</pre>

	Enjoy :
	<pre name="code" class="brush: bash">./samples_cpp</pre>
*/

/**
	@PageName compile_libtcod_codelite
	@PageFather compile_libtcod
	@PageTitle Using CodeLite
	@PageDesc TODO
*/

/**
	@PageName compile_libtcod_haiku
	@PageFather compile_libtcod
	@PageTitle On Haiku
	@PageDesc TODO
*/

/**
	@PageName start_project
	@PageCategory Howtos
	@PageTitle How to start a project
	@PageDesc This page contains howtos about how to create a project from scratch
*/

/**
	@PageName start_mingw
	@PageFather start_project
	@PageTitle On Windows with Mingw
	@PageDesc TODO
*/

/**
	@PageName start_linux
	@PageFather start_project
	@PageTitle On Linux
	@PageDesc TODO
*/

/**
	@PageName start_codelite
	@PageFather start_project
	@PageTitle Using CodeLite
	@PageDesc TODO
*/
