# Parameters

## Dependencies

- cmake
- Boost

### Optional

- [termcolor](https://github.com/ikalnytskyi/termcolor)

### Install dependencies on macOS

```bash
brew install boost
```

### Install dependencies on Debian 9

```bash
sudo apt-get install --no-install-recommends libboost-filesystem-dev libboost-program-options-dev
```

## Build

This will only build the binary.

```bash
./bin/build.sh
```

Then the binary is available in `build_release`.

## Install

After building the project, run:

```bash
./bin/install.sh
```
