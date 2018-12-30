# Parameters

Automatic replace variables in configuration file templates from environment variables.

The parameters program takes an input file template and a given regular expression for searching environment variables. The variables in the template file will be replaced by the values of found environment variables by the exact same name.

## Examples

### Simple example

First set the environment variables. This can also come from GitLab CI variables:

```bash
export SYMF_DB_USER=my_user
export SYMF_DB_PASS=my_super_secret_password
```

Then set up a dotenv template file (`.env.dist`):

```bash
DB_USER=@SYMF_DB_USER@
DB_PASS=@SYMF_DB_PASS@
```

Now parameters will replace the variables:

```bash
parameters --input .env.dist --regexp ^SYMF_ > .env
```

This will take `.env.dist` as input file and `^SYMF_` as regular expression to search for variables in your shell environment. Searching the environment variables by a given regular expression is the same as running `env | grep ^SYMF_` in your shell.

The result in `.env` file:

```bash
DB_USER=my_user
DB_PASS=my_super_secret_password
```

### Example using different envrionments

You can also use different environments like Testing, Staging, Production, etc.

```bash
parameters --input .env.dist --regexp ^SYMF_ --env testing > .env
```

`testing` will be converted to `TESTING`.

### Example using different instances

```bash
parameters --input .env.dist --regexp ^SYMF_ --instance shopa > .env
```

`shopa` will be converted to `SHOPA`.

Also a combination is possible.

```bash
parameters --input .env.dist --regexp ^SYMF_ --env testing --instance shopa > .env
```

Run `parameters --help` to see more parameters.

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

You need a modern C++ compiler that supports C++14 and CMake 3.7 or later.

This will build the binary:

```bash
./bin/build.sh
```

Then the binary is available in `build_release/bin`.

After building the project, run:

```bash
./bin/install.sh
```

## Install via Debian repository

Only Debian 9 (stretch) is supported.

Add the public GPG key to the apt sources keyring:

```bash
wget -qO - https://fox21.at/christian_mayer.asc | sudo apt-key add -
```

Verify key on the keyring. This should print out informations about the key:

```bash
apt-key list 2814B4288C4594FF0BA75571F96ED997C9382D47
```

Add apt source file and update:

```bash
echo 'deb https://debian.fox21.at/ stretch main' > /etc/apt/sources.list.d/fox21at.list
apt-get update
```

Install the package:

```bash
sudo apt-get install parameters
```

Now you can run the program:

```bash
parameters --help
```

## Install under Debian via .deb file

Go to the [GitHub releases page](https://github.com/TheFox/parameters/releases) and download the desired version file. Then run:

```bash
sudo dpkg --install parameters.deb
```

## Install via Homebrew

1. Add the [`thefox/brewery`](https://github.com/TheFox/homebrew-brewery) tap to brew.

	```
	brew tap thefox/brewery
	```

2. Actual installation

	```
	brew install parameters
	```

## Tested under

- macOS 10.13 High Sierra
- macOS 10.14 Mojave
- Debian 8 (jessie)
- Debian 9 (stretch)

## License

Copyright (C) 2018 Christian Mayer <https://fox21.at>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.
