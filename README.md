# Prerequisites (unix):

	GCC 4.2 or newer
	CMake 2.6 or newer
	GNU Make 3.81 or newer
	libexecinfo (FreeBSD and OpenBSD only)
	libreadline

----------------------------------
### on ubuntu:
	apt-get install g++
	apt-get install make
	apt-get install cmake
	apt-get install subversion
	apt-get install libreadline-dev

### 32bit on ubuntu:
	apt-get install g++-multilib

----------------------------------
### on fedora:
	yum install gcc-c++
	yum install libstdc++-static
	yum install make
	yum install cmake
	yum install subversion
	yum install readline-devel

----------------------------------
### on osx:
	brew install cmake

----------------------------------
### on freebsd (8,9):
	pkg_add -r cmake
	pkg_add -r subversion
	pkg_add -r libexecinfo

----------------------------------
### on freebsd 10+:
	pkg install cmake
	pkg install subversion
	pkg install libexecinfo

# Build:

### on linux/freebsd/osx:
	sh build

----------------------------------
### on Windows:
	Start
	  All Programs
	    Microsoft Visual Studio 2012
	      Visual Studio Tools
	        Developer Command Prompt for VS2012
	        
	build

# Test:

### on linux/freebsd/osx:
	cd bin
	Release/fibjs main.js

----------------------------------
### on Windows:
	cd bin
	x64_Release/fibjs main.js

# Install:

### on linux/freebsd/osx:
	sudo sh bin/Release/installer.sh  // x86_64
	sudo sh bin/Release32/installer.sh // ia32

----------------------------------
### on Windows(run as administrator):
	bin/x64_Release/installer  // x86_64
	bin/Win32_Release/installer // ia32

# Run:

	js
	js stuff.js
