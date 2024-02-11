@echo off

choco install cmake visualstudio2019buildtools --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK" -y
RefreshEnv.cmd
call build.bat

exit /B 0