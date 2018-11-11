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

Add the public GPG key to the apt sources keyring:

```bash
wget -qO - https://fox21.at/christian_mayer.asc | sudo apt-key add -
```

Verify key on the keyring. This should print out informations about the key:

```bash
apt-key list 2814B4288C4594FF0BA75571F96ED997C9382D47
```

Install the package:

```bash
sudo apt-get install parameters
```

Now you can run the program:

```bash
parameters --help
```
