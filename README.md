# DotEngine

An overkill way to retrieve (and cache) data from the GitHub API. The powerhouse
of my dotfiles autocompletion.

- [DotEngine](#dotengine)
  - [Dependencies](#dependencies)
  - [Building](#building)
  - [Motivation](#motivation)
    - [ZSH Auto-completion Example](#zsh-auto-completion-example)
  - [Performance](#performance)
  - [License](#license)

## Building

Standard Rust `cargo` workflow applies.

```cli
$ cargo build
$ cargo run
```

## Installation

Cargo really is magical.

```cli
$ cargo install --locked --git https://github.com/jarulsamy/dotEngine --profile release
```

## Motivation

The primary goal for this project was initially just to get more acquainted with
C. I initially wrote this project in Python and rewrote it in C. Then I wanted
to get better at C++, so I rewrote the entire thing. After getting bored, and
wanting to learn Rust, I rewrote it from scratch, again. This has served as a
nice, easy, starter project for getting better at a language. and since I use
this program almost daily (auto-completion for `clone` and other utilities I
also wrote).

### ZSH Auto-completion Example

I have several functions in my [.dotfiles](https://github.com/jarulsamy/.dotfiles) for quickly cloning my personal repos. This project will power auto-completion (once I figure out some packaging stuff) and hopefully more in the future.

ZSH Auto-completion Sample:

```zsh
#!/usr/bin/env zsh

_clone() {
  _describe 'command' "($($HOME/.local/bin/dotEngine -z 2>/dev/null))"
}

compdef _clone clone
```

## Performance

Prior versions of this program where entirely written in Python and C++ and thus were
_much_ slower:

Without Cache:

```
v3 (Rust):   target/release/dot-engine -fz  0.05s user 0.02s system 3% cpu 2.171 total
v2 (C++):    ./dotEngine -fz  0.01s user 0.01s system 4% cpu 0.345 total
v1 (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 7% cpu 1.027 total
```

With Cache:

```
v3 (Rust):   target/release/dot-engine -z  0.00s user 0.01s system 154% cpu 0.006 total
v2: (C++):   ./dotEngine -z  0.00s user 0.01s system 95% cpu 0.007 total
v1 (Python): ~/.dotfiles/zfunc/dotEngine/utils.py  0.06s user 0.02s system 99% cpu 0.081 total
```

Yup. As you can see, absolutely **MASSIVE** performance improvements.

In reality this doesn't really translate to much of a real-world improvement,
but I'll take what I can get.

## License

More power to the FOSS community! Refer to [LICENSE](LICENSE).
