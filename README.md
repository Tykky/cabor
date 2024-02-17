# Cabor C


## How to build

The build dependencies are
- CMake 3.22 or newer
- C99 compiler

### Building on Windows
In order to build the project on windows you'll need
* [git](https://git-scm.com/download/win)
* [cmake](https://cmake.org/download/)
* [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) (make sure to select Desktop development with C++ in the installer)

if you have all these then you should be able to build the project by running

```
build.bat
```
and after the build is completed successfully the program can be run with
```
cabor.bat
```
alternatively, the executables can be found at `build/bin/Debug` or `build/bin/Release`. The `cabor.bat` script runs the release build by default.
### Building on Linux/macOS
For Linux/macOS just make sure you have Git, CMake and Xcode/GCC installed using your package manager of choice. For macOS the easiest options is probably to use [homebrew](https://brew.sh/) to install the said packages. On macOS you could also use [Xcode](https://developer.apple.com/Xcode/) (Apple Clang) to build the project. CMake can generate XCode project for you.
```
./build.sh
```
and after the build is completed successfully the program can be run with
```
./cabor.sh
```
alternatively, the executables can be found at `build/release/bin` and `build/debug/bin`. The script `cabor.sh` runs the release build by default.

## Building on fresh Ubuntu 2022.04 LTS
For Ubuntu 2022.04 LTS it's possible to run this script to build the project without having any of the dependencies. It's not recommended that you run this as it has the side effect of doing full system upgrade but in case you're running a vm this could be used.
```
curl --output b.sh --url https://raw.githubusercontent.com/Tykky/cabor/main/ci/bootsrap-ubuntu202204LTS.sh && sudo chmod +x b.sh && sudo ./b.sh && cd cabor && sudo chmod +x build.sh && sudo chmod +x cabor.sh && sudo ./build.sh
```
after doing that the program can be run as usual
```
./cabor.sh
```

## Building on fresh Windows Server 2022 Datacenter core (20240111)
For Windows (using cmd.exe) there is a similar script that does everything for you. Again it's only really recommended if you're building this on a VM. The script will download [chocolatey](https://chocolatey.org/) and install git, cmake and visual studio build tools using the package manager.
```
curl.exe --output b.bat --url https://raw.githubusercontent.com/Tykky/cabor/main/ci/bootstrap-win2022.bat & b.bat & build.bat
```
Afer running that you should be able to run the program as usual:
```
cabor.bat
```