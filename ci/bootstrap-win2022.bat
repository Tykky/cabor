@"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))" && SET "PATH=%PATH%;%ALLUSERSPROFILE%\chocolatey\bin"

choco install git -y
choco install cmake --installargs 'ADD_CMAKE_TO_PATH=System' -y
choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.VC.CMake.Project --add Microsoft.VisualStudio.Component.Windows10SDK --add Microsoft.VisualStudio.Component.Windows11SDK.22000 --includeRecommended --quiet --norestart" -y

refreshenv
git clone https://github.com/Tykky/cabor

cd cabor