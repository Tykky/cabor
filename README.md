# Cabor C


## How to build

The build dependencies are
- CMake 3.22 or newer
- C99 compiler

### Building on Windows
```
build.bat
```
and after the build is completed successfully the program can be run with
```
cabor.bat
```
alternatively, the executables can be found at `build/bin/Debug` or `build/bin/Release`. The `cabor.bat` script runs the release build by default.
### Building on Linux/macOS
```
./build.sh
```
and after the build is completed successfully the program can be run with
```
./cabor.sh
```
alternatively, the executables can be found at `build/release/bin` and `build/debug/bin`. The script `cabor.sh` runs the release build by default.

## Building on fresh Ubuntu 2022.04 LTS
There is a script for Ubuntu 2022.04 LTS that installs all the required build dependencies and then builds the program, this can be run with:
```
sudo ./ci/ubuntu202204LTS.sh
```
As the folder name implies this is used in CI (github actions) builds.