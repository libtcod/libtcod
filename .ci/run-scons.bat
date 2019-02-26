C:/Python27/python.exe -m pip install --upgrade pip distribute setuptools wheel pywin32
C:/Python27/python.exe -m pip install "scons==3.0.1"
cd build/scons
scons develop_all -s dist %SCONSMODE% %SCONSOPTS% MODE=DEBUG_RELEASE
cd ../..
