# DotEngine

An overkill way to retrieve (and cache) data from the GitHub API. The powerhouse of my dotfiles autocompletion.

- [DotEngine](#dotengine)
  - [Dependencies](#dependencies)
  - [Building](#building)
  - [Motivation](#motivation)
    - [ZSH Auto-completion Example](#zsh-auto-completion-example)
  - [Performance](#performance)
  - [License](#license)

## Dependencies

- cmake >= 3.15
- A C++ compiler with C++17 support
- cURL
- Boost::JSON v1.75+

Any other dependencies should be auto downloaded by cmake.

## Building

Build with `cmake`. From the root of the repository:

```
$ cmake -B ./build -DCMAKE_BUILD_TYPE=Release # Or -DCMAKE_BUILD_TYPE=Debug for a debug build.
$ cmake --build ./build
```

This should produce the `build/dotEngine` binary.

## Motivation

The primary goal for this project was initially just to get more acquainted with C. I am much better at C++ than C, and since I use this program almost daily (auto-completion for `clone` and other utilities I also wrote), I decided to rewrite it from scratch in C++. I spent some more time, and statically linked everything I could, so distribution across various Linux distributions is much easier.

### ZSH Auto-completion Example

I have several functions in my [.dotfiles](https://github.com/jarulsamy/.dotfiles) for quickly cloning my personal repos. This project will power auto-completion (once I figure out some packaging stuff) and hopefully more in the future.

ZSH Auto-completion Sample:

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
Modern (C++): ./dotEngine -fz  0.01s user 0.01s system 4% cpu 0.345 total
Old (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 7% cpu 1.027 total
```

With Cache:

```
Modern (C): ./dotEngine -z  0.00s user 0.01s system 95% cpu 0.007 total
Old (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 99% cpu 0.081 total
```

Yup. As you can see, absolutely **MASSIVE** performance improvements.

In reality this doesn't really translate to much of a real-world improvement, but I'll take what I can get.

## License

More power to the FOSS community! Refer to [LICENSE](LICENSE).
