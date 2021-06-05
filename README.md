# DotEngine

An overkill way to retrieve (and cache) data from the GitHub API. The powerhouse of my dotfiles autocompletion.

- [DotEngine](#dotengine)
  - [Dependencies](#dependencies)
  - [Building](#building)
    - [Option 1: Convenience Python Script](#option-1-convenience-python-script)
    - [Option 2: Standard CMake Procedure](#option-2-standard-cmake-procedure)
  - [Motivation](#motivation)
    - [ZSH Autocompletion Example](#zsh-autocompletion-example)
  - [Performance](#performance)
  - [Licence](#licence)

## Dependencies

* cmake >= 3.15
* make
* A C compiler with ISO C11 standard support
* cURL

Any other dependencies should be auto downloaded by cmake.

## Building

### Option 1: Convenience Python Script

`setup.py` is a convenience script to setup both development and production builds of dotEngine. The script will automatically create a `build/` folder with the correct cmake configuration.

```
$ ./setup.py dev

-- The C compiler identification is GNU 11.1.0
... Blah blah more cmake stuff
```

You can substitute `dev` for `prod` here, and a release binary (with max optimizations and no debug symbols) will be built.

From here, you can `cd` into the `build` directory and use `make` to compile.

```
$ cd build
$ make
```

This should produce the `dotEngine` binary.

### Option 2: Standard CMake Procedure

Create a build directory and generate the preffered cmake configuration:

```
$ mkdir build
$ cd build
```

```
$ cmake .. -DCMAKE_BUILD_TYPE=DEBUG # Debug build
```
Or:
```
$ cmake .. -DCMAKE_BUILD_TYPE=RELEASE # Production build
```

Then compile with `make`

```
$ make
```

This should produce the `dotEngine` binary.

## Motivation

The primary goal for the project was just to get more aquainted with C. For this reason, it is likely that some of the code is _less than stellar_. However, I do think it is a good demonstration of how a relatively simple task in a high level language such as python translates to a lower level language such as C, even though I am utilizing a library for some of the more complicated stuff. Another positive, this version is a bit faster than my previous python version.

### ZSH Autocompletion Example

I have several functions in my [.dotfiles](https://github.com/jarulsamy/.dotfiles) for quickly cloning my personal repos. This project will power autocompletion (once I figure out some packaging stuff) and hopefully more in the future.

ZSH Autocompletion Sample:
```zsh
#!/usr/bin/env zsh

_clone() {
  _describe 'command' "($(/PATH/TO/dotEngine -z))"
}
compdef _clone clone
```

## Performance

Prior versions of this program where entirely written in Python and thus were
_much_ slower:

Without Cache:
```
Modern (C): ./dotEngine -f  0.04s user 0.01s system 3% cpu 1.204 total
Old (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 7% cpu 1.027 total
```

With Cache:
```
Modern (C): ./dotEngine -z  0.00s user 0.01s system 96% cpu 0.007 total
Old (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 99% cpu 0.081 total
```

Yup. As you can see, absolutely __MASSIVE__ performance improvements. :wink:.

In reality this doesn't really translate to much of a real-world improvement, but I'll take what I can get.

## Licence

More power to the FOSS community! Refer to [LICENSE](LICENSE).
