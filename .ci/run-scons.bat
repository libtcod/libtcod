C:/Python27/python.exe -m pip install --upgrade pip distribute setuptools wheel
C:/Python27/python.exe -m pip install scons
cd build/scons
scons develop_all dist %SCONSMODE% %SCONSOPTS% CPPDEFINES=NDEBUG
cd ../..
