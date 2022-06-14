#!/bin/bash

# Check for git 
if ! [ -x "$(command -v git)" ]; then
  echo 'Error: git is not installed.' >&2
  exit 1
fi

# Check for ~/.aflat directory
if [ -d ~/.aflat ]; then
  rm -rf ~/.aflat
fi

# Cd into the aflat directory
mkdir ~/.aflat
(cd ~/.aflat && git clone https://github.com/DeForestt/aflat.git)

# Build aflat
(cd ~/.aflat/aflat && mkdir bin && make)

# check if aflat is added to PATH
if ! [ -x "$(command -v aflat)" ]; then
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.bashrc
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.zshrc
fi



# remove any aflat aliases
echo 'Do you want to remove any existing aflat aliases (y/N)?'
read answer
if [ "$answer" == "y" ]; then
    echo 'Removing existing aflat aliases...'
    sed -i '/aflat/d' ~/.bashrc
    sed -i '/aflat/d' ~/.zshrc
fi

echo "Successfully installed aflat!"

# Ascci art
echo '
  _____ _ _ _ _  _ _____ 
 |  ___| | | | |/ /|  ___|
    | |_  | | | | ' / | |_
    |  _| | | | | <  |  _|
    |_|   |_|_|_|\_\|_|
'