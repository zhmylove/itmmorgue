# ITMMORGUE

Cooperative multiplayer roguelike game.

## Dependencies

`ncursesw` library is required for graphics support.
To get this library you can either:
* install `libncursesw5-dev` package for Debian-like GNU/Linux;
* build `devel/ncurses` port for FreeBSD;
* install `CSWncurses` package for Solaris;
* build `libncursesw` from [source](https://www.gnu.org/software/ncurses/) .

## Build

Get the latest version of the game and submodules:
```
$ git clone https://github.com/zhmylove/itmmorgue.git && cd itmmorgue
$ git submodule update --init --remote
```

Prepare source tree for build:
```
$ ./configure
```

In case of successful execution it prints the name of utility to build sources (`make/gmake`) or explains errors otherwise.

Compile the main file of the game (if `make` was recommended on the previous step):
```
$ make
```

`bin/itmmorgue` binary will be produced.
Also you can run the game instantly with `(g)make run`.

## Bugs

If you find some bugs or if you are not able to build the game, please contact authors or create an issue.
