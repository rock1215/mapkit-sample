## Submodules

This repository contains submodules. Clone it with `git clone --recursive`. If you forgot,
run `git submodule update --init --recursive`.

## Translations

If you want to improve app translations or add more search synonyms, please check our [wiki](https://github.com/mapsme/omim/wiki).

## Compilation

To compile the project, you would need to initialize private key files. Run
`configure.sh` and press Enter to create empty files, good enough to build desktop
and Android debug packages.

For detailed installation instructions and Android/iOS building process,
see [INSTALL.md](https://github.com/mapsme/omim/tree/master/docs/INSTALL.md).

## Development

You would need Qt 5 for development, most other libraries are included into the
repository: see `3party` directory. The team uses mostly XCode and Qt Creator,
though these are not mandatory. We have an established
[c++ coding style](https://github.com/mapsme/omim/blob/master/docs/CPP_STYLE.md) and [Objective-C coding style](https://github.com/mapsme/omim/blob/master/docs/OBJC_STYLE.md).

See [CONTRIBUTING.md](https://github.com/mapsme/omim/blob/master/docs/CONTRIBUTING.md)
for the repository initialization process, the description of all the directories
of this repository and other development-related information.

All contributors must sign a [Contributor Agreement](https://github.com/mapsme/omim/blob/master/docs/CLA.md),
so both our and their rights are protected.
