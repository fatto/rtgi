# cg-lab

## Setup

### Windows
Require Visual Studio Express 2013 [download](http://www.microsoft.com/it-it/download/details.aspx?id=40787).

Download [lib-include-windows.zip](https://dl.dropboxusercontent.com/u/6801332/lib-include-windows.zip) and copy `include` and `lib` folders into `C:/Program Files(x86)/Microsoft Visual Studio 12.0/VC`.

### OS X
Require Xcode from the App Store

Download [lib-include.zip](https://dl.dropboxusercontent.com/u/6801332/lib-include.zip) and copy `include` and `lib` folders into `/usr/local`.

Open the Finder and press `CMD+Shift+G` to use "Go to Folder...".

### Linux i386/x86_64
Copy and paste `sudo apt-get install build-essential libgl1-mesa-dev` in the terminal and press enter.


## Run

### Windows
Open the Visual Solution into `cg-lab-win` and you are ready to go.

### OS X
Open the Xcode solution into `cg-lab-osx` and you are ready to go.

### Linux i386
Open the terminal, change directory to `cg-lab-linux-i386` and launch `make run`.

### Linux x86_64
Open the terminal, change directory to `cg-lab-linux` and launch `make run`.