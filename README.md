# ITMMORGUE

Cooperative multiplayer rogue-like game.

## Dependencies

`ncursesw` is required for graphics.

In Debian-like system it is `libncursesw5-dev` package. Look for something like this in other distributives/OSs.

## Build

Run
```
$ git clone https://github.com/zhmylove/itmmorgue.git && cd itmmorgue
```
to clone this repo, then
```
$ ./configure
```
It will tell you `make` utility name to use or will say that something is going wrong.

Running suitable `make` will create `bin/itmmorgue` binary in the success case. The `run` target will tell `make` to build and run the game.

## Bugs

If you find some bugs or if you are not able to build the game, please contact to authors or create an issue.
