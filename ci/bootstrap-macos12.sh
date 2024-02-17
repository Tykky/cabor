#!/bin/sh

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

brew install git cmake rbenv

rbenv init
rbenv install 3.1.2
rbenv global 3.1.2

gem install xcode-install
xcversion install '15.2'
sudo xcodebuild -license accept
sudo xcode-select -s /Applications/Xcode-15.2.app