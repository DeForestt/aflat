#!/bin/bash

# Check for git 
if ! [ -x "$(command -v git)" ]; then
  echo 'Error: git is not installed.' >&2
  exit 1
fi

# Check for cmake
if ! [ -x "$(command -v cmake)" ]; then
  echo 'Error: cmake is not installed.' >&2
  exit 1
fi


# Check if aflat is in the PATH
if ! [ -x "$(command -v aflat)" ]; then
  echo 'Adding aflat to PATH'
  # Check if .bashrc exists
  if [ -f ~/.bashrc ]; then
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.bashrc
  fi
  # Check if .zshrc exists
  if [ -f ~/.zshrc ]; then
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.zshrc
  fi
  echo "Exporting PATH"
  export PATH=$PATH:~/.aflat/aflat/bin
fi

# Check for ~/.aflat directory
if [ -d ~/.aflat ]; then
  echo 'Remove ~/.aflat directory'
  rm -rf ~/.aflat
fi


# Cd into the aflat directory
mkdir ~/.aflat
(cd ~/.aflat && git clone https://github.com/DeForestt/aflat.git)

# Build aflat
(cd ~/.aflat/aflat && mkdir bin && mkdir build && cd build && cmake ../. && cd .. && cmake --build build> buildLog.log)

# Build libs
(cd ~/.aflat/aflat && bash rebuild-libs.sh)

if ! [ -x "$(command aflat)" ]; then
  echo 'Error: aflat is not installed.' >&2
  exit 1
fi
echo "Successfully installed aflat!"
