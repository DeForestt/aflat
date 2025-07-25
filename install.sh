#!/bin/bash

install_completion() {
  local comp_script="$HOME/.aflat/aflat/completions/aflat.bash"
  if [ -f "$comp_script" ]; then
    if [ -f "$HOME/.bashrc" ] && ! grep -q "source $comp_script" "$HOME/.bashrc"; then
      echo "source $comp_script" >> "$HOME/.bashrc"
    fi
    if [ -f "$HOME/.zshrc" ] && ! grep -q "source $comp_script" "$HOME/.zshrc"; then
      echo "source $comp_script" >> "$HOME/.zshrc"
    fi
  fi
}

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

  echo 'Exporting PATH'
  PATH=$PATH:~/.aflat/aflat/bin

  echo 'Adding aflat to PATH'
  # Check if .bashrc exists
  if [ -f ~/.bashrc ]; then
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.bashrc
  fi
  # Check if .zshrc exists
  if [ -f ~/.zshrc ]; then
    echo 'export PATH=$PATH:~/.aflat/aflat/bin' >> ~/.zshrc
  fi
fi

# Check for ~/.aflat directory
if [ -d ~/.aflat ]; then
  (cd ~/.aflat/aflat && git pull)
  (cd ~/.aflat/aflat && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release)
  (cd ~/.aflat/aflat && cmake --build build --target aflat)
  (cd ~/.aflat/aflat && bash rebuild-libs.sh)
  install_completion
  echo 'aflat updated'
  exit
fi

# Cd into the aflat directory
mkdir ~/.aflat
(cd ~/.aflat && git clone https://github.com/DeForestt/aflat.git)

# Check for boost
if ! [ -x "$(command -v boost)" ]; then
  echo 'Error: boost is not installed. Please install boost' >&2
fi

# Build aflat
(cd ~/.aflat/aflat && mkdir bin && make ../. && cd .. && cmake --build build > buildLog.log)

# Build libs
(cd ~/.aflat/aflat && bash rebuild-libs.sh)
install_completion

echo 'Done Please restart your terminal or run source ~/.bashrc or source ~/.zshrc'
