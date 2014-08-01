#!/bin/sh

set -e # Exit script immediately on first error.
set -x # Print commands and their arguments as they are executed.

# Update Debian package index.
sudo apt-get update -y

# Install required Debian packages.
sudo apt-get install -y git clang-3.5 libclang-common-3.5-dev libclang1-3.5 libc++-dev libc++-helpers libc++1

# Install our .bashrc
cp /vagrant/bashrc /home/vagrant/.bashrc
