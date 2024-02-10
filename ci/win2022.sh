@echo off

# Assume chocolatey is already installed
choco install cmake visualstudio2019buildtools --package-parameters "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK"
call build.bat

exit /B 0