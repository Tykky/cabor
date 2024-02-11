@echo off

choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools -y
refreshenv
call build.bat

exit 0