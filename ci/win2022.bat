@echo off

choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK --add Microsoft.VisualStudio.Component.Windows10SDK.18362 --add Microsoft.VisualStudio.Component.Windows10SDK.19041 --add Microsoft.VisualStudio.Component.Windows10SDK.20348 --add Microsoft.VisualStudio.Component.Windows11SDK.22000" -y
refreshenv

exit 0