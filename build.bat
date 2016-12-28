<# :
@IF "!CI!" NEQ "True" ECHO OFF
setlocal EnableDelayedExpansion

REM Copyright 2015 Richard Tew
REM This script is intended to automate building of libtcod on Windows.

REM TODO: appveyor publish build dependencies
REM TODO: replace branch name with libtcod version in release package file name
REM TODO: Put commit id in release package file name.

REM Divert to the internal setup code, it will return to the user setup.
goto internal_function_setup

REM --- FUNCTION: user_function_setup ----------------------------------------
:user_function_setup

REM This function is provided for the user to define what resources this script should
REM fetch and help prepare.  How they are actually prepared, the user specifies by
REM adding support below in 'user_function_prepare_dependency'.
REM
REM __ LINKS entries are either URLs to ZIP archives, or a collection of version control (vcs) information.
REM      LINKS[n]=<url to zip>
REM      LINKS[n]=vcs <vcs-system> <name> <revision-id> <repo-path> <snapshot-url>
REM        <revision-id>: This can be a URL, or it can be a filesystem path to an existing local clone.

REM TODO(rmtew): Comment out local git repositories and replace with remote ones before committing.

REM set LINKS[0]=vcs hg SDL2 007dfe83abf8 http://hg.libsdl.org/SDL http://hg.libsdl.org/SDL/archive/REV.zip
set LINKS[0]=vcs hg SDL2 007dfe83abf8 C:\RMT\VCS\HG\libraries\SDL http://hg.libsdl.org/SDL/archive/
set LINKS[1]=

if "!SDL2LINK!" NEQ "" set LINKS[0]=!SDL2LINK!

REM __ MD5CHECKSUMS entries are the MD5 checksum for the download in the matching LINKS position
REM      To get the checksum for a newly added download to add to an entry here, simply run the script and when
REM      the download is processed, the calculated checksum will be printed and noted as unchecked.

REM e.g. set MD5CHECKSUMS[0]=BB-C5-10-F0-91-34-2F-CF-5B-A7-A2-BC-A3-97-FD-A1

set UV_INCLUDE_PATH=!DEPENDENCY_PATH!\include

set UV_INCLUDE_COMMANDS[0]=
set /A UV_INCLUDE_COMMAND_COUNT=0

set UV_PACKAGES_DIRNAME=packages
set UV_PACKAGES_PATH=!BUILD_PATH!\packages

REM Allow the user to specify the path to their Git 'git' executable.  It may have to be accessed via absolute path.
if not defined PYTHON_EXE (
    set "PATH=C:\Python27;%PATH%"
    python.exe --help >nul 2>&1 && (set PYTHON_EXE=python.exe) || (set PYTHON_EXE=)
)

REM Get the date in a form YYYYMMDD
call :internal_function_get_date
set UV_DATE=!V_RESULT!

if not defined UV_BUILD_ID (
	for /F "usebackq tokens=*" %%A in (`hg id`) do (
		set UV_BUILD_ID=%%A
	)
)

if not defined UV_PACKAGE_RELEASE_DIRNAME (
	if defined APPVEYOR (
		if defined APPVEYOR_REPO_TAG_NAME (
			set UV_PACKAGE_RELEASE_DIRNAME=!APPVEYOR_PROJECT_NAME!-msvs-!APPVEYOR_REPO_TAG_NAME!
		) else (
			set UV_PACKAGE_RELEASE_DIRNAME=!APPVEYOR_PROJECT_NAME!-msvs-!APPVEYOR_REPO_BRANCH!-!UV_DATE!
		)
	) else (
		set UV_PACKAGE_RELEASE_DIRNAME=libtcod-msvs
	)
)

REM Process the user data, calling event functions when applicable.
goto:eof REM return

REM --- FUNCTION: user_function_prepare_dependency --------------------------------
:user_function_prepare_dependency
REM variable: %V_LINK_PARTS% - The link data.
REM variable: %DEPENDENCY_PATH% - The absolute path of the dependencies directory.
REM variable: %V_DIRNAME% - The relative directory name the dependency can be found in.
REM variable: %V_SKIPPED% - 'yes' or 'no', depending on whether the archive was already extracted.

set /A L_SDL2_ATTEMPTS=0

:user_function_prepare_dependency_retry
cd "!DEPENDENCY_PATH!"

if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "http" (
	echo ERROR.. !V_LINK_PARTS[%HTTP_FILENAME%]! not handled by user who wrote the build amendments.
	goto internal_function_exit
) else if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "vcs" (
    set L_VCS_NAME=!V_LINK_PARTS[%VCS_NAME%]!
    if "!L_VCS_NAME!" EQU "SDL2" (
        REM This variable is required by the libtcod makefile.
        set SDL2PATH=!DEPENDENCY_PATH!\!V_DIRNAME!

        cd !V_DIRNAME!

		REM TODO(rmtew): Allow user filtering for specific configurations and platforms.
		for %%P in (Win32 x64) do (
			if not exist "!DEPENDENCY_PATH!\%%P" mkdir "!DEPENDENCY_PATH!\%%P"
			for %%C in (Debug Release) do (
				set "SDL2BUILDPATH=!DEPENDENCY_PATH!\%%P\%%C"
				if not exist "!SDL2BUILDPATH!" mkdir "!SDL2BUILDPATH!"

				if exist "!SDL2BUILDPATH!\SDL2.dll" (
					echo Building: [SDL2^|%%C^|%%P] .. skipped
				) else (
					echo Building: [SDL2^|%%C^|%%P]

					REM SDL2 in theory requires the DirectX SDK to be installed, but in practice it's absence
					REM can be worked around by putting a dummy include file in place, and then only XAudio2
					REM support is lost.
					set L_ERROR_MSG=
					devenv /upgrade VisualC\SDL.sln
					for /F "usebackq tokens=*" %%i in (`msbuild /nologo VisualC\SDL.sln /p:Configuration^=%%C /p:Platform^=%%P /t:SDL2^,SDL2main`) do (
						set L_LINE=%%i
						if "!CI!" EQU "True" echo %%i
						if "!L_LINE:fatal error=!" NEQ "!L_LINE!" set L_ERROR_MSG=%%i
					)
					set /A L_SDL2_ATTEMPTS=!L_SDL2_ATTEMPTS!+1

					if exist VisualC\%%P\%%C\SDL2.dll (
						echo Copying: [SDL2^|%%C^|%%P]
						copy >nul VisualC\%%P\%%C\SDL2.dll "!SDL2BUILDPATH!\"
						copy >nul VisualC\%%P\%%C\SDL2.lib "!SDL2BUILDPATH!\"
						copy >nul VisualC\%%P\%%C\SDL2.pdb "!SDL2BUILDPATH!\"
					) else (
						REM Only try and recover from the DirectX problem, and if that don't work, give up.
						if "!L_ERROR_MSG!" NEQ "" (
							if !L_SDL2_ATTEMPTS! EQU 1 (
								if "!L_ERROR_MSG:dxsdkver=!" NEQ "!L_ERROR_MSG!" (
									echo WARNING: Attempting workaround for missing DirectX SDK [disabling XAudio2]
									type nul > include\dxsdkver.h

									goto user_function_prepare_dependency_retry
								)
							)
						)

						echo ERROR.. SDL2.dll did not successfully build for some reason.
						goto internal_function_exit
					)
				)
			)
		)

        set UV_INCLUDE_COMMANDS[%UV_INCLUDE_COMMAND_COUNT%]=!V_DIRNAME!\include\*.h
        set /A UV_INCLUDE_COMMAND_COUNT=%UV_INCLUDE_COMMAND_COUNT%+1
    ) else (
        echo ERROR.. !V_LINK_PARTS[%VCS_NAME%]! not handled by user who wrote the build amendments.
        goto internal_function_exit
    )
)

goto:eof REM return

REM --- FUNCTION: user_function_prepare_dependencies -------------------------
:user_function_prepare_dependencies
REM description: This is called as a final step after all dependencies have been prepared.
REM variable: %DEPENDENCY_PATH% - The absolute path of the dependencies directory.

if exist "!UV_INCLUDE_PATH!" goto exit_from_user_function_prepare_dependencies

set /A L_COUNT=0
:loop_user_function_prepare_dependencies

if %L_COUNT% LSS %UV_INCLUDE_COMMAND_COUNT% (
    echo Copying includes: !UV_INCLUDE_COMMANDS[%L_COUNT%]!
    xcopy >nul /Q /Y "!DEPENDENCY_PATH!\!UV_INCLUDE_COMMANDS[%L_COUNT%]!" "%UV_INCLUDE_PATH%\"

    set /A L_COUNT=%L_COUNT%+1
    goto loop_user_function_prepare_dependencies
)

:exit_from_user_function_prepare_dependencies
goto:eof

REM --- FUNCTION: user_function_build_project --------------------------------
:user_function_build_project

set TCOD_SLN_PATH=!BUILD_PATH!\msvs

for %%P in (Win32 x64) do (
	for %%C in (Debug Release) do (
		set L_ERROR_MSG=
		for /F "usebackq tokens=*" %%i in (`msbuild /nologo msvs\libtcod.sln /p:Configuration^=%%C /p:Platform^=%%P`) do (
			set L_LINE=%%i
			if "!CI!" EQU "True" echo %%i
			if "!L_LINE:fatal error=!" NEQ "!L_LINE!" set L_ERROR_MSG=%%i
		)
		if "!L_ERROR_MSG!" NEQ "" (
			echo ERROR.. libtcod did not successfully build for some reason.
			goto internal_function_exit
		)
	)
)

goto:eof REM return

REM --- FUNCTION: user_function_make_release ---------------------------------
:user_function_make_release

cd "!BUILD_PATH!"

REM Windows defender or something holds onto a file handle.  Retry..
set /A L_COUNT=0
for /L %%I in (1,1,5) do (
	if exist "!UV_PACKAGES_DIRNAME!" (
		rmdir /S /Q "!UV_PACKAGES_DIRNAME!"
		set /A L_COUNT=!L_COUNT!+1
	)
)

if exist "!UV_PACKAGES_DIRNAME!" (
	echo Failed to purge old release directory: !UV_PACKAGES_DIRNAME!
	goto internal_function_exit
)

mkdir "!UV_PACKAGES_DIRNAME!"
cd "!UV_PACKAGES_DIRNAME!"

REM Do binary releases for all platforms.
for %%P in (Win32 x64) do (
	echo Making release: !UV_PACKAGE_RELEASE_DIRNAME!-%%P
	set "L_RELEASE_PATH=!UV_PACKAGE_RELEASE_DIRNAME!-%%P"

	REM Start with a verbatim copy of the repository.
	hg archive -t files !L_RELEASE_PATH!
	del !L_RELEASE_PATH!\.hg*

	REM Copy the dependencies into place for compilation.
	xcopy /I /E  >nul "!BUILD_PATH!\dependencies\include" "!L_RELEASE_PATH!\dependencies\include\"
	xcopy /I /E  >nul "!BUILD_PATH!\dependencies\%%P" "!L_RELEASE_PATH!\dependencies\%%P\"

	REM Copy release dlls into the top level directory.
	copy >nul "!BUILD_PATH!\dependencies\%%P\Release\SDL2.dll" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\libtcod\%%P\Release\libtcod.dll" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\libtcod-gui\%%P\Release\libtcod-gui.dll" "!L_RELEASE_PATH!\"

	REM Copy samples.
	copy >nul "!BUILD_PATH!\msvs\samples_c\%%P\Release\samples_c.exe" "!L_RELEASE_PATH!\samples.exe"
	copy >nul "!BUILD_PATH!\msvs\doctcod\%%P\Release\doctcod.exe" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\frost\%%P\Release\frost.exe" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\navier\%%P\Release\navier.exe" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\rad\%%P\Release\rad.exe" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\ripples\%%P\Release\ripples.exe" "!L_RELEASE_PATH!\"
	copy >nul "!BUILD_PATH!\msvs\weather\%%P\Release\weather.exe" "!L_RELEASE_PATH!\"

	REM Record the release path, so the packaging stage can be rerun.
	echo !L_RELEASE_PATH!>>index.txt
)

echo Making dependency snapshot: !UV_PACKAGE_RELEASE_DIRNAME!-dependencies
set "L_RELEASE_PATH=!UV_PACKAGE_RELEASE_DIRNAME!-dependencies"
mkdir !L_RELEASE_PATH!

xcopy /I /E  >nul "!BUILD_PATH!\dependencies\include" "!L_RELEASE_PATH!\dependencies\include\"
for %%P in (Win32 x64) do (
	xcopy /I /E  >nul "!BUILD_PATH!\dependencies\%%P" "!L_RELEASE_PATH!\dependencies\%%P\"
	for %%C in (Release Debug) do (
		for %%N in (libtcod libtcod-gui samples_c samples_cpp frost navier rad ripples weather doctcod) do (
			set "L_RELPATH=%%N\%%P\%%C"
			mkdir "!L_RELEASE_PATH!\!L_RELPATH!"

			for %%X in (exe dll) do (
				if exist "!BUILD_PATH!\msvs\!L_RELPATH!\%%N.%%X" (
					copy >nul "!BUILD_PATH!\msvs\!L_RELPATH!\%%N.%%X" "!L_RELEASE_PATH!\!L_RELPATH!\"
					if "%%X" EQU "dll" (
						copy >nul "!BUILD_PATH!\msvs\!L_RELPATH!\%%N.lib" "!L_RELEASE_PATH!\!L_RELPATH!\"
					)
				)
			)
			copy >nul "!BUILD_PATH!\msvs\!L_RELPATH!\%%N.pdb" "!L_RELEASE_PATH!\!L_RELPATH!\"
		)
	)
)
REM Record the release path, so the packaging stage can be rerun.
echo !L_RELEASE_PATH!>>index.txt

cd "!BUILD_PATH!"

:exit_from_user_function_make_release
goto:eof REM return

REM --- FUNCTION: user_function_package_release ------------------------------
:user_function_package_release

cd "!BUILD_PATH!\!UV_PACKAGES_DIRNAME!"

REM The make release stage will have recorded release directories in 'index.txt'.
for /F "usebackq tokens=*" %%I in (index.txt) do (
    echo Packaging release: %%I

	!7Z_EXE! a -r -t7z -mx9 %%I.7z %%I\*

	if defined APPVEYOR (
		appveyor PushArtifact %%I.7z
	)
)

cd "!BUILD_PATH!"

goto:eof REM return

REM --- FUNCTION: user_function_teardown -------------------------------------
:user_function_teardown
REM description: This is called just before a non-error exit.

goto internal_function_exit_clean

REM --- FUNCTION: internal_function_setup ------------------------------------
:internal_function_setup

REM Ensure that we have a properly set up developer console with access to things like msbuild and devenv.
if "%BYPASS_VS_CHECK%" NEQ "yes" (
	REM Ensure that we have a properly set up developer console with access to things like msbuild and devenv.
	if not exist "%VS140COMNTOOLS%VsDevCmd.bat" (
		echo You do not appear to have Visual Studio 2015 installed.
		echo The community edition is free, download it and install it.
		pause & exit /b
	)

	if "%VisualStudioVersion%" NEQ "" (
		echo Your console window has already run the setup for Visual Studio %VisualStudioVersion%.
		echo Open a fresh window and run this script there.  It will run the correct setup.
		pause & exit /b
	)
	CALL "%VS140COMNTOOLS%VsDevCmd.bat"
)

if "%VisualStudioVersion%" NEQ "14.0" (
	echo Visual Studio incorrect.
	echo Expected: "14.0"
	echo Got: "%VisualStudioVersion%"
	pause & exit /b
)

REM The top-level directory.
set BUILD_SCRIPT_PATH=%~dp0
set BUILD_SCRIPT_FILENAME=%~nx0
REM The build sub-directory.
set BUILD_DIRNAME=build
set BUILD_PATH=%BUILD_SCRIPT_PATH%%BUILD_DIRNAME%
REM The dependencies sub-directory.
set DEPENDENCY_DIRNAME=%BUILD_DIRNAME%\dependencies
set DEPENDENCY_PATH=%BUILD_SCRIPT_PATH%%DEPENDENCY_DIRNAME%

REM Allow the user to specify the path to their Mercurial 'hg' executable.  It may have to be accessed via absolute path.
if not defined HG_EXE (
    hg.exe >nul 2>&1 && (set HG_EXE=hg.exe) || (set HG_EXE=)
)

REM Allow the user to specify the path to their Git 'git' executable.  It may have to be accessed via absolute path.
if not defined GIT_EXE (
    REM This doesn't work directly, like hg.exe, so need where.
    where git.exe >nul 2>&1 && (set GIT_EXE=git.exe) || (set GIT_EXE=)
)

REM Allow the user to specify the path to their 7zip '7z' executable.  It may have to be accessed via absolute path.
if not defined 7Z_EXE (
    7z.exe >nul 2>&1 && (set 7Z_EXE=7z.exe) || (set 7Z_EXE=)
    if "!7Z_EXE!" EQU "" (
        if exist "c:\Program Files\7-Zip\7z.exe" set 7Z_EXE="c:\Program Files\7-Zip\7z.exe"
    )
)

if not exist "%DEPENDENCY_PATH%" mkdir "%DEPENDENCY_PATH%"
cd "%DEPENDENCY_PATH%"

REM These variables are used to index the LINKS array entries.
REM
REM               0    1     2      3          4          5
REM HTTP link:   HTTP <url> <NAME> <FILENAME>
REM VCS link:    VCS  <hg>  <NAME> <REVISION> <CLONEURL> <ZIPDLURL>
REM ............ Attempt 1: Identify presence of hg.exe or git.exe, and clone or pull <CLONEURL> <REVISION>
REM ............ Attempt 2: Download <ZIPDLURL><REVISION>.zip as a normal link.

set LINK_CLASSIFIER=0
set HTTP_URL=1
set HTTP_NAME=2
set HTTP_FILENAME=3
set VCS_SYSTEM=1
set VCS_NAME=2
set VCS_REVISION=3
set VCS_CLONEURL=4
set VCS_ZIPDLURL=5

call :user_function_setup

REM --- FUNCTION: internal_function_main -------------------------------------
REM input argument:  V_LINKS   - The user defined links.

if "%1" EQU "" (
    echo Usage: !BUILD_SCRIPT_FILENAME! [OPTION]
    echo.
    echo     -fd, fetch-dependencies    download if necessary
    echo     -pd, prepare-dependencies  extract and/or build, ready for project build
    echo     -mr, make-release          construct release directory for packaging
    echo     -pr, package-release       compress and archive release directory
    echo.
    echo     -d, dependencies           fetch and prepare the dependencies
    echo     -p, project                build this project using the dependencies
    echo     -r, release                construct/compress/archive built project
    echo.
    if not defined HG_EXE (
    echo WARNING: 'hg.exe' cannot be located.  Mercurial may not be installed.
    echo   This script can operate without it, but that mode is less supported.
    echo   If Mercurial is not in PATH, you can set HG_EXE to full filename of 'hg.exe'
    echo.
    )
    if not defined GIT_EXE (
    echo WARNING: 'git.exe' cannot be located.  Git may not be installed.
    echo   This script can operate without it, but that mode is less supported.
    echo   If Git is not in PATH, you can set GIT_EXE to full filename of 'git.exe'
    echo.
    )

    goto internal_function_teardown
)

set V_COMMANDS[fetch-dependencies]=fetch-dependencies
set V_COMMANDS[prepare-dependencies]=prepare-dependencies
set V_COMMANDS[make-release]=make-release
set V_COMMANDS[package-release]=package-release
set V_COMMANDS[dependencies]=dependencies
set V_COMMANDS[project]=project
set V_COMMANDS[release]=release
set V_COMMANDS[-fd]=fetch-dependencies
set V_COMMANDS[-pd]=prepare-dependencies
set V_COMMANDS[-mr]=make-release
set V_COMMANDS[-pr]=package-release
set V_COMMANDS[-d]=dependencies
set V_COMMANDS[-p]=project
set V_COMMANDS[-r]=release

:parse_args
if "%~1" EQU "" goto parse_args_end
set L_COMMAND=!V_COMMANDS[%~1]!
if "!L_COMMAND!" EQU "" (
    echo !BUILD_SCRIPT_FILENAME!: command %1 unrecognised.
    echo.
    echo Type '!BUILD_SCRIPT_FILENAME!' to see valid options.
    goto internal_function_teardown
)
set V_COMMAND[!L_COMMAND!]=yes
shift
goto parse_args

:parse_args_end

REM Ensure sub-steps are selected for the more general commands.
if "!V_COMMAND[dependencies]!" EQU "yes" (
    set V_COMMAND[fetch-dependencies]=yes
    set V_COMMAND[prepare-dependencies]=yes
)

if "!V_COMMAND[release]!" EQU "yes" (
    set V_COMMAND[make-release]=yes
    set V_COMMAND[package-release]=yes
)

if "!V_COMMAND[project]!" EQU "yes" (
    set V_COMMAND[build-project]=yes
)

REM Do the selected general commands.
if "!V_COMMAND[fetch-dependencies]!" EQU "yes" (
    call :internal_function_fetch_dependencies
)

if "!V_COMMAND[prepare-dependencies]!" EQU "yes" (
    call :internal_function_prepare_dependencies
)

if "!V_COMMAND[build-project]!" EQU "yes" (
    call :internal_function_build_project
)

if "!V_COMMAND[make-release]!" EQU "yes" (
    call :internal_function_make_release
)

if "!V_COMMAND[package-release]!" EQU "yes" (
    call :internal_function_package_release
)

goto internal_function_teardown

REM --- FUNCTION: internal_function_build_project ----------------------------
:internal_function_build_project

cd "!BUILD_PATH!"
call :user_function_build_project

goto:eof REM return

REM --- FUNCTION: internal_function_make_release -----------------------------
:internal_function_make_release

cd "%DEPENDENCY_PATH%"
call :user_function_make_release

goto:eof REM return

REM --- FUNCTION: internal_function_package_release --------------------------
:internal_function_package_release

if "!7Z_EXE!" EQU "" (
    echo ERROR: Packaging a release requires 7zip to be installed.
    goto internal_function_teardown
)

call :user_function_package_release

goto:eof REM return

REM --- FUNCTION: internal_function_prepare_dependencies ---------------------
:internal_function_prepare_dependencies
REM input argument:  V_LINKS   - The user defined links.

set /A IDX_PDS=0
:loop_internal_function_prepare_dependencies
set V_LINK=!LINKS[%IDX_PDS%]!
if "!V_LINK!" EQU "" goto loop_internal_function_prepare_dependencies_break

REM function call: V_LINK_PARTS = split_link(V_LINK)
call :internal_function_split_link

call :internal_function_prepare_dependency

set /A IDX_PDS=!IDX_PDS!+1
goto loop_internal_function_prepare_dependencies

:loop_internal_function_prepare_dependencies_break
call :user_function_prepare_dependencies

goto:eof REM return

REM --- FUNCTION: internal_function_prepare_dependency -----------------------
:internal_function_prepare_dependency
REM input argument:  V_LINK_PARTS   - The processed link parts to make use of.
REM output argument: V_LINK_PARTS   - The array of elements that make up the given link text.

REM               0    1     2      3          4          5
REM HTTP link:   HTTP <url> <NAME> <FILENAME>
REM VCS link:    VCS  <hg>  <NAME> <REVISION> <CLONEURL> <ZIPDLURL>
REM ............ Attempt 1: Identify presence of hg.exe or git.exe, and clone or pull <CLONEURL> <REVISION>
REM ............ Attempt 2: Download <ZIPDLURL><REVISION>.zip as a normal link.

if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "http" (
    REM Environment variables for powershell script.
    set fn=Archive-Extract
    set fnp0=%DEPENDENCY_PATH%\!V_LINK_PARTS[%HTTP_FILENAME%]!
    set fnp1=%DEPENDENCY_PATH%
    REM Environment variables for function 'user_function_prepare_dependency'.
    set V_DIRNAME=
    set V_SKIPPED=no

    REM Iterate over the lines of output.
    for /F "usebackq tokens=*" %%i in (`more "%BUILD_SCRIPT_PATH%%BUILD_SCRIPT_FILENAME%" ^| powershell -c -`) do (
        set L_LINE=%%i
        if "!L_LINE:~0,12!" EQU "EXTRACTPATH:" (
            set V_DIRNAME=!L_LINE:~13!
        ) else if "!L_LINE:~0,4!" EQU "MSG:" (
            if "!L_LINE:~5!" EQU "EXTRACTED" (
                echo Decompressing: [!V_LINK_PARTS[%HTTP_FILENAME%]!] .. skipped
                set V_SKIPPED=yes
            ) else if "!L_LINE:~5!" EQU "EXTRACTING" (
                echo Decompressing: [!V_LINK_PARTS[%HTTP_FILENAME%]!]
            )
        ) else (
            echo Unexpected result: !L_LINE!
        )
    )
    REM Do we process the output as it arrives, or at the end?
    REM echo Decompressed?: [!V_LINK_PARTS[%HTTP_FILENAME%]!]
) else if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "vcs" (
    set V_DIRNAME=!V_LINK_PARTS[%VCS_NAME%]!
    REM Stop errors from nothing being here.
)

call :user_function_prepare_dependency

cd "%DEPENDENCY_PATH%"

goto:eof REM return

REM --- FUNCTION: internal_function_fetch_dependencies --------------------------
:internal_function_fetch_dependencies
REM input argument:  V_LINKS        - The processed link parts to make use of.

set /A V_IDX_FD=0
:loop_internal_function_fetch_dependencies
set V_LINK=!LINKS[%V_IDX_FD%]!
if "!V_LINK!" EQU "" goto exit_from_internal_function_fetch_dependencies

REM function call: V_LINK_PARTS = split_link(V_LINK)
call :internal_function_split_link

REM function call: download_link(V_LINK_PARTS)
call :internal_function_fetch_dependency

set /A V_IDX_FD=!V_IDX_FD!+1
goto loop_internal_function_fetch_dependencies

:exit_from_internal_function_fetch_dependencies
goto:eof REM return

REM --- FUNCTION: internal_function_fetch_dependency ---------------------------
:internal_function_fetch_dependency
REM input argument:  V_LINK_PARTS   - The processed link parts to make use of.
REM input argument:  V_IDX_FD       - The index into the links array of the current dependency.
REM output argument: V_LINK_PARTS   - The array of elements that make up the given link text.

:loop_internal_function_fetch_dependency
cd "%DEPENDENCY_PATH%"

if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "vcs" (
    set L_VCS_DESC=
    if "!V_LINK_PARTS[%VCS_SYSTEM%]!" EQU "hg" (
        set L_VCS_DESC=Mercurial
        set L_VCS_TEST_NAME=.hg
        set L_VCS_EXE=!HG_EXE!
        set L_VCS_CMD_CLONE=!HG_EXE! clone "!V_LINK_PARTS[%VCS_CLONEURL%]!" !V_LINK_PARTS[%VCS_NAME%]!
        set L_VCS_CMD_PULL=!HG_EXE! pull
        set L_VCS_CMD_UPDATE=!HG_EXE! update -r "!V_LINK_PARTS[%VCS_REVISION%]!" -C
    ) else if "!V_LINK_PARTS[%VCS_SYSTEM%]!" EQU "git" (
        set L_VCS_DESC=Git
        set L_VCS_TEST_NAME=.git
        set L_VCS_EXE=!GIT_EXE!
        set L_VCS_CMD_CLONE=!GIT_EXE! clone "!V_LINK_PARTS[%VCS_CLONEURL%]!" !V_LINK_PARTS[%VCS_NAME%]! --no-checkout -b master
        set L_VCS_CMD_PULL=!GIT_EXE! pull origin master
        set L_VCS_CMD_UPDATE=!GIT_EXE! checkout !V_LINK_PARTS[%VCS_REVISION%]! -q
    )

    if "!L_VCS_DESC!" NEQ "" (
        REM Compute whether to use the VCS system or not.
        if "!L_VCS_EXE!" NEQ "" (
            set L_USE_VCS=yes
            REM Presence of directory, but lack of tested sub-directory, indicates zip archive source.
            if exist "!V_LINK_PARTS[%VCS_NAME%]!" (
                if not exist "!V_LINK_PARTS[%VCS_NAME%]!\!L_VCS_TEST_NAME!" (
                    echo WARNING: While !L_VCS_DESC! is now installed, existing [!V_LINK_PARTS[%VCS_NAME%]!] directory does not use it.
                    set L_USE_VCS=no
                )
            )
        ) else (
            set L_USE_VCS=no
        )
        if "!L_USE_VCS!" EQU "yes" (
            REM Has the repository already been cloned?  If so, reuse it.
            echo Fetching: [!V_LINK_PARTS[%VCS_NAME%]!] !L_VCS_DESC! repository.
            if not exist "!V_LINK_PARTS[%VCS_NAME%]!" (
                REM Does not exist, fetch it.
                for /F "usebackq tokens=*" %%i in (`!L_VCS_CMD_CLONE!`) do (
                    echo .. %L_VCS_EXE%: %%i
                )
                REM The subsequent VCS update needs to be within the repository directory.
                cd !V_LINK_PARTS[%VCS_NAME%]!
            ) else (
                REM The VCS pull and subsequent update needs to be within the repository directory.
                cd !V_LINK_PARTS[%VCS_NAME%]!
                for /F "usebackq tokens=*" %%i in (`!L_VCS_CMD_PULL!`) do (
                    echo .. %L_VCS_EXE%: %%i
                )
            )
            echo Updating: [!V_LINK_PARTS[%VCS_NAME%]!] !L_VCS_DESC! repository to revision [!V_LINK_PARTS[%VCS_REVISION%]!].
            for /F "usebackq tokens=*" %%i in (`!L_VCS_CMD_UPDATE!`) do (
                echo .. %L_VCS_EXE%: %%i
            )
            goto exit_from_internal_function_fetch_dependency
        ) else (
            if exist "!V_LINK_PARTS[%VCS_NAME%]!" (
                if exist "!V_LINK_PARTS[%VCS_NAME%]!\!L_VCS_TEST_NAME!" (
                    echo WARNING: No !L_VCS_DESC! client installed yet [!V_LINK_PARTS[%VCS_NAME%]!] was originally created using one.
                    echo WARNING: Therefore [!V_LINK_PARTS[%VCS_NAME%]!] cannot be updated.
                    goto exit_from_internal_function_fetch_dependency
                )
            )
            REM Fall back to ZIP archive downloading.  Ordering of these assignments is important as some overwrite others.
            echo WARNING: This is untested code.  If it does not work, report it.
            set V_LINK_PARTS[%HTTP_NAME%]=!V_LINK_PARTS[%VCS_NAME%]!
            set V_LINK_PARTS[%LINK_CLASSIFIER%]=http
            echo Download of VCS repo snapshots not fully implemented, giving up.
            goto internal_function_exit
            REM TODO: Set this to !V_LINK_PARTS[%VCS_ZIPDLURL%]! with 'REV' replaced with !V_LINK_PARTS[%VCS_REVISION%]!
            set V_LINK_PARTS[%HTTP_URL%]=!V_LINK_PARTS[%VCS_ZIPDLURL%]!
            REM TODO: Set this to the extracted file name from the HTTP_URL we just constructed.
            set V_LINK_PARTS[%HTTP_FILENAME%]=
        )
    )
)

REM Skip logic to keep script flat, as not possible to have label inside if statement block.
if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" NEQ "http" goto exit_from_internal_function_fetch_dependency

set /A L_ATTEMPTS=0
:internal_function_fetch_dependency_retry

if not exist "!V_LINK_PARTS[%HTTP_NAME%]!" (
    echo Downloading: [!V_LINK_PARTS[%HTTP_NAME%]!]
    powershell -c "Start-BitsTransfer -source !V_LINK_PARTS[%HTTP_URL%]!"

    if not exist !V_LINK_PARTS[%HTTP_FILENAME%]! (
        echo Failed to download !V_LINK_PARTS[%HTTP_FILENAME%]!
        goto internal_function_exit
    )
) else (
    echo Downloading: [!V_LINK_PARTS[%HTTP_NAME%]!] .. skipped
)
set /A L_ATTEMPTS=!L_ATTEMPTS!+1

call :internal_function_checksum

if "!V_PASSED!" EQU "yes" (
    echo .. MD5 checksum [!V_CHECKSUM!] correct
) else if "!V_PASSED!" EQU "no" (
    echo .. MD5 checksum [!V_CHECKSUM!] incorrect
    echo .. Expected: [!MD5CHECKSUMS[%V_IDX_FD%]!]

    if !L_ATTEMPTS! EQU 1 (
        set /p L_RESULT="Attempt to re-download the file [Y/n]?"
        if /I "!L_RESULT!" NEQ "n" (
            del !V_LINK_PARTS[%HTTP_FILENAME%]!
            goto internal_function_fetch_dependency_retry
        )
    )

    echo ERROR: Failed to obtain valid copy of [!V_LINK_PARTS[%HTTP_FILENAME%]!]
    goto internal_function_exit
) else (
    echo .. MD5 checksum [!V_CHECKSUM!] unknown
)

:exit_from_internal_function_fetch_dependency
goto:eof REM return

REM --- FUNCTION: internal_function_checksum ---------------------------------

:internal_function_checksum
REM input argument:  V_LINK_PARTS   - The array of elements relating to the link in question.
REM output argument: V_CHECKSUM     - The calculated checksum for the given file.
REM output argument: V_PASSED       - If there is one to match, "yes" for correct and "no" for incorrect.
REM                                   If there is not one to match, "".

set fn=MD5-Checksum
set fnp0=!V_LINK_PARTS[%HTTP_FILENAME%]!
set fnp1=discard

set V_PASSED=

REM Iterate over the lines of output.
for /F "usebackq tokens=*" %%i in (`more "%BUILD_SCRIPT_PATH%%BUILD_SCRIPT_FILENAME%" ^| powershell -c -`) do (
    set L_LINE=%%i
    if "!L_LINE:~0,4!" EQU "MSG:" (
        set V_CHECKSUM=!L_LINE:~5!
        set L_CHECKSUM=!MD5CHECKSUMS[%V_IDX_FD%]!
        if "!L_CHECKSUM!" NEQ "" (
            if "!L_CHECKSUM!" EQU "!V_CHECKSUM!" (
                set V_PASSED=yes
            ) else (
                set V_PASSED=no
            )
        )
    ) else (
        echo Unexpected result in checksum function: !L_LINE!
        goto internal_function_exit
    )
)

:exit_from_internal_function_checksum
goto:eof REM return

REM --- FUNCTION: internal_function_verify_link ------------------------------

:internal_function_verify_link
REM input argument:  V_LINK         - The link text to verify.
REM output argument: V_LINK_PARTS   - The array of elements that make up the given link text.

REM function call: V_LINK_PARTS = split_link(V_LINK)
call :internal_function_split_link

if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "http" (
    if /I "!V_LINK_PARTS[%HTTP_URL%]:~0,4!" NEQ "http" goto internal_function_verify_link__valid
    if /I "!V_LINK_PARTS[%HTTP_URL%]:~-4,4!" NEQ ".zip" goto internal_function_verify_link__valid
) else if "!V_LINK_PARTS[%LINK_CLASSIFIER%]!" EQU "vcs" (
    if "!V_LINK_PARTS[%VCS_SYSTEM%]!" EQU "hg" goto internal_function_verify_link__valid
)

REM emergency exit with error message.
echo Invalid link: %V_LINK%
goto internal_function_exit

:internal_function_verify_link__valid
goto:eof REM return

REM --- FUNCTION: internal_function_split_link -------------------------------
:internal_function_split_link
REM input argument:  V_LINK         - The link text to verify.
REM output argument: V_LINK_PARTS   - The array of elements that make up the given link text.

set V_LINK_PARTS[%LINK_CLASSIFIER%]=
for /F "tokens=1,2,3,4,5,6" %%A in ("!V_LINK!") do (
    set L_FIRST=%%A
    if /I "!L_FIRST:~0,7!" EQU "http://" (
        set V_LINK_PARTS[%LINK_CLASSIFIER%]=http
        set V_LINK_PARTS[%HTTP_URL%]=!V_LINK_PARTS[%%A]!

        REM .. V_LINK_PARTS[%HTTP_NAME%], V_LINK_PARTS[%HTTP_FILENAME%] = get_urlfilename(V_LINK)
        call :internal_function_get_urlfilename

		set V_LINK_PARTS[%HTTP_NAME%]=!V_RESULT!
		set V_LINK_PARTS[%HTTP_FILENAME%]=!V_RESULT!
    ) else (
        set V_LINK_PARTS[%LINK_CLASSIFIER%]=%%A
        set V_LINK_PARTS[%VCS_SYSTEM%]=%%B
        set V_LINK_PARTS[%VCS_NAME%]=%%C
        set V_LINK_PARTS[%VCS_REVISION%]=%%D
        set V_LINK_PARTS[%VCS_CLONEURL%]=%%E
        set V_LINK_PARTS[%VCS_ZIPDLURL%]=%%F
    )
)

:exit_from_internal_function_split_link
goto:eof REM return

REM --- FUNCTION: internal_function_get_urlfilename ---------------------------
:internal_function_get_urlfilename
REM input argument:  V_LINK         - The link text to verify.
REM output argument: V_RESULT       - The base filename the URL exposes for download.

set L_LINK=!V_LINK!
:loop_internal_function_get_urlfilename_1
if "!L_LINK!" EQU "" goto loop0continue
REM Select the substring up to the first path separator.
for /f "delims=/" %%M in ("!L_LINK!") do set L_SUBSTRING=%%M

:loop_internal_function_get_urlfilename_2
REM Skip until the next path separator.
set L_CHAR=!L_LINK:~0,1!
set L_LINK=!L_LINK:~1!
if "!L_LINK!" EQU "" goto loop_internal_function_get_urlfilename_3
if "!L_CHAR!" NEQ "/" goto loop_internal_function_get_urlfilename_2
goto loop_internal_function_get_urlfilename_1

:loop_internal_function_get_urlfilename_3
REM We have the trailing string after the last path separator, or the file name.
set V_RESULT=!L_SUBSTRING!

goto:eof REM return

REM --- FUNCTION: internal_function_get_date ---------------------------------
:internal_function_get_date
REM output argument: V_RESULT       - The date in form YYYYMMDD

FOR /F "tokens=3" %%A IN ('REG QUERY "HKCU\Control Panel\International" /v sShortDate 2^>NUL') DO (
	SET sShortDate=%%A
)

for /f "tokens=2-4 delims=/ " %%a in ('date /t') do (
	if "!sShortDate:~0,1!" EQU "d" (
		set V_RESULT=%%c%%b%%a
	) else (
		set V_RESULT=%%c%%a%%b
	)
)

goto:eof REM return

REM --- Everything is done, exit back to the user ----------------------------

:internal_function_teardown
REM Now that processing is done, allow the user to do some steps before exiting.
set EXIT_CODE=0
goto user_function_teardown

:internal_function_exit
set EXIT_CODE=1
:internal_function_exit_clean
REM Leave the user in the directory they were in to begin with.
cd %BUILD_SCRIPT_PATH%

REM endlocal: Ensure environment variables are left the same as when the script started.
REM exit /b:  Exit the script, but do not close any DOS window it was run from within.
endlocal & exit /b !EXIT_CODE!
#>

function MD5-Checksum([string]$path, [string]$discard) {
    # $fullPath = Resolve-Path $path;
    $md5 = new-object -TypeName System.Security.Cryptography.MD5CryptoServiceProvider;
    $file = [System.IO.File]::Open($path, [System.IO.Filemode]::Open, [System.IO.FileAccess]::Read);
    $hash = [System.BitConverter]::ToString($md5.ComputeHash($file))
    Write-Host "MSG: $hash";
    $file.Dispose();
    return 0;
}

function Archive-Extract([string]$zipFilePath, [string]$destinationPath) {
    # This will get added when paths are joined, and path comparison will need it to be absent.
    $destinationPath = $destinationPath.TrimEnd("\");

    [Reflection.Assembly]::LoadWithPartialName('System.IO.Compression.FileSystem') > $null;
    $zipfile = [IO.Compression.ZipFile]::OpenRead($zipFilePath);

    # Determine how many top level entries there are.
    $ziplevel0files = @{};
    $zipfile.Entries | foreach {
        $s = ($_.FullName.TrimEnd("/") -split "/")[0];
        if ($ziplevel0files.ContainsKey($s)) {
            $ziplevel0files[$s] = $ziplevel0files[$s] + 1;
        } else {
            $ziplevel0files[$s] = 0;
        }
    }

    if ($ziplevel0files.count -ne 1) {
        $zipDirName = [io.path]::GetFileNameWithoutExtension($zipFilePath)
        $zipDirPath = $destinationPath = Join-Path -Path $destinationPath -ChildPath $zipDirName
    } else {
        $zipDirName = $ziplevel0files.Keys[0]
        $zipDirPath = Join-Path -Path $destinationPath -ChildPath $zipDirName;
    }

    Write-Host "EXTRACTPATH: $zipDirName";
    if (Test-Path -LiteralPath $zipDirPath) {
        Write-Host "MSG: EXTRACTED";
        return 2; # Failure
    }
    Write-Host "MSG: EXTRACTING";

    $zipfile.Entries | foreach {
        $extractFilePath = Join-Path -Path $destinationPath -ChildPath $_.FullName;
        $extractFileDirPath = Split-Path -Parent $extractFilePath;

        if (-not (Test-Path -LiteralPath $extractFileDirPath -PathType Container)) {
            New-Item -Path $extractFileDirPath -Type Directory | Out-Null;
        }

        # Sometimes a directory comes after a file within the directory (the latter causes it to be created implicitly above).
        if (-not $extractFilePath.EndsWith("\")) {
            try {
                [IO.Compression.ZipFileExtensions]::ExtractToFile($_, $extractFilePath, $true);
            } catch {
                Write-Host "MSG: Failed to extract file:" $extractFilePath;
                return 3; # Failure
            }
        }
    }
    return 0; # Success
}

# Anything that calls should execute the powershell and set the parameters.

$fn = (Get-ChildItem Env:fn).Value;
$arg0 = (Get-ChildItem Env:fnp0).Value;
$arg1 = (Get-ChildItem Env:fnp1).Value;
$err = & $fn $arg0 $arg1;
exit $err;
