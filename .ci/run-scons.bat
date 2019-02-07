C:/Python27/python.exe -m pip install --upgrade pip distribute setuptools wheel pywin32
C:/Python27/python.exe -m pip install "scons==3.0.1"
cd build/scons
set BUILDTYPE=MODE=RELEASE
if %APPVEYOR_REPO_TAG%==false set BUILDTYPE=MODE=DEBUG_RELEASE
scons develop_all -s dist %SCONSMODE% %SCONSOPTS% %BUILDTYPE%
cd ../..
