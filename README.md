# Parameters

## Dependencies

- cmake
- Boost

Optional:

- [termcolor](https://github.com/ikalnytskyi/termcolor)

### Install dependencies on macOS

```bash
brew install boost
```

### Install dependencies on Debian 9

```bash
sudo apt-get install --no-install-recommends libboost-filesystem-dev libboost-program-options-dev
```

## Build from Source

This will build the binary.

```bash
./bin/build.sh
```

Then the binary is available in `build_release`.

After building the project, run:

```bash
./bin/install.sh
```

## Install via Debian repository

Only Debian 9 is supported.
