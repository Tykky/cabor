@echo off

choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK" -y
refreshenv
call build.bat

exit 0