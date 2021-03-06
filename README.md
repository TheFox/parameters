# Parameters

Automatic replace variables in configuration file templates from environment variables.

The parameters program takes an input file template and a given regular expression for searching environment variables. The variables in the template file will be replaced by the values of found environment variables by the exact same name.

## Project Outlines

The project outlines as described in my blog post about [Open Source Software Collaboration](https://blog.fox21.at/2019/02/21/open-source-software-collaboration.html).

- The main purpose of this software is to handle one template file and generate another file out of that.
- This list is open. Feel free to request features.

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
$ parameters --input .env.dist --regexp ^SYMF_ > .env
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
$ parameters --input .env.dist --regexp ^SYMF_ --env testing > .env
```

`testing` will be converted to `TESTING`.

### Example using different instances

```bash
$ parameters --input .env.dist --regexp ^SYMF_ --instance shopa > .env
```

`shopa` will be converted to `SHOPA`.

Also a combination is possible.

```bash
$ parameters --input .env.dist --regexp ^SYMF_ --env testing --instance shopa > .env
```

Run `parameters --help` to see more parameters.

## Dependencies

- [CMake](https://cmake.org/) (For building)
- [Boost](https://www.boost.org/) (Filesystem / Program Options)

### Optional Dependencies

- [termcolor](https://github.com/ikalnytskyi/termcolor)

### Install dependencies on macOS

```bash
$ brew install boost
```

### Install dependencies on Debian 9

```bash
$ sudo apt-get install --no-install-recommends libboost-filesystem-dev libboost-program-options-dev
```

## Build from Source

You need a modern C++ compiler that supports C++14 and CMake 3.8 or later.

This will build the binary:

```bash
$ ./bin/build.sh
```

Then the binary is available in `build_release/bin`.

After building the project, run:

```bash
$ ./bin/install.sh
```

## Install via Debian repository

Only Debian 9 (stretch) is supported.

Add the public GPG key to the apt sources keyring:

```bash
$ wget -qO - 'https://bintray.com/user/downloadSubjectPublicKey?username=bintray' | sudo apt-key add -
```

Verify key on the keyring. This should print out informations about the key:

```bash
$ apt-key list bintray@bintray.com
```

Add apt source file and update:

```bash
$ echo "deb https://dl.bintray.com/thefox/jar stretch main" > /etc/apt/sources.list.d/fox21at.list
$ apt-get update
```

Install the package:

```bash
$ sudo apt-get install parameters
```

Now you can run the program:

```bash
$ parameters --help
```

## Install under Debian via .deb file

Go to the [GitHub releases page](https://github.com/TheFox/parameters/releases) and download the desired version file. Then run:

```bash
$ sudo dpkg --install parameters.deb
```

## Install via Homebrew

1. Add the [`thefox/brewery`](https://github.com/TheFox/homebrew-brewery) tap to brew.

	```bash
	$ brew tap thefox/brewery
	```

2. Actual installation

	```bash
	$ brew install parameters
	```

## Tested under

- macOS 10.13 High Sierra
- macOS 10.14 Mojave
- Debian 8 (jessie)
- Debian 9 (stretch)
