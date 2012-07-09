# WoW Map Viewer

The main repository can be <a href="http://code.google.com/p/wowmapviewer/">found here</a>.
However, this one actually builds correctly in Visual Studio 2010.

The build WoWMapViewer you must first build stormlib (included in `src/stormlib`).
Take a look <a href="https://github.com/iamcal/StormLib/blob/master/README.md">here</a> to see how.


## Building on Windows with Visual Studio

* Copy `StormLibDAS.lib` (which you built earlier) into `vc2008/` or `vc2010/`
* Build the solution in that folder
* You may need to change the debugging command line options to point to your own game data directory


## Building on Windows with dev-c++

* Open `wowmapviewer.dev` in dev-c++ and compile normaly
* Copy `StormLib.dll` to `bin/`
* Run `WowMapViewer.exe` in `bin/`


## Building on Linux

* Compile wowmapviewer in `src/` with a plain old `make`
* Run `./wowmapview -gamepath ...../Program\ Files/World\ of\ Warcraft/Data/` in `bin/`
