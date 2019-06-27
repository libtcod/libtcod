pip install --upgrade setuptools wheel pywin32
pip install "scons==3.0.5"
cd build/scons
scons develop_all -s dist %SCONSMODE% %SCONSOPTS% MODE=DEBUG_RELEASE
cd ../..
