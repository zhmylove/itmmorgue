# ITMMORGUE

Cooperative multiplayer roguelike game.

## Dependencies

`ncursesw` library is required for graphics support.
To get this library you can either:
* install `libncursesw5-dev` package for Debian-like GNU/Linux;
* install `ncurses-devel ncurses-libs` packages for RedHat-like GNU/Linux;
* build `devel/ncurses` port for FreeBSD;
* install `CSWncurses` package for Solaris;
* build `libncursesw` from [source](https://www.gnu.org/software/ncurses/) .

## Build

Get the latest version of the game and submodules:
```
$ git clone https://github.com/zhmylove/itmmorgue.git && cd itmmorgue
$ git submodule update --init
```
If you have git version>=1.8.0.2 you can optionally add `--remote` parameter for latter command to checkout the latest version of the submodules.

Prepare source tree for build:
```
$ ./configure
```

Carefully read the bottom line of the output. It'll describe errors, if any.

Compile the main executable file of the game:
```
$ make
```

This step results in `bin/itmmorgue` binary will be compiled.
And you can run the game instantly with `make run`.

## Bugs

If you find some bugs or if you are not able to build the game, please contact authors or create an issue.
