@echo off

set PATH=%PATH%;C:\"Program Files"\Git\bin
set PATH=%PATH%;C:\"Program Files"\CMake\bin
set PATH=%PATH%;C:\"Program Files (x86)"\"Microsoft Visual Studio"\2019\BuildTools\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64

choco install cmake visualstudio2019buildtools --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK" -y
RefreshEnv.cmd
call build.bat

exit /B 0