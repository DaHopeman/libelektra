# 0.8.21 Release

> !!! This release did not happen yet.

Please update this file within PRs accordingly.
For non-trivial changes, you can choose to be
part of the highlighted changes. Please make
sure to add some short tutorial or how-to-use
for highlighted items.
Please add your name to every contribution
syntax: ", thanks to <myname>".

- guid: ea3ade20-0a27-4c4b-a922-6b29462a1fc5
- author: Markus Raab
- pubDate: Thu, 21 Dec 2017 21:17:38 +0100
- shortDesc:

We are proud to release Elektra 0.8.21.

<<`scripts/git-release-stats 0.8.VERSION`>>

## What is Elektra?

Elektra serves as a universal and secure framework to access
configuration settings in a global, hierarchical key database.
For more information, visit [https://libelektra.org](https://libelektra.org).



## Highlights

- Fosdem Talk about Elektra
- CC-licensed book about Elektra published
- Maturing of plugins
- Elektra with encryption
- Preparation for switch to INI as default storage

### Fosdem Talk about Elektra in Main Track

We are happy to announce that there will be a talk about
Elektra in one of the main tracks of [Fosdem 2018](https://fosdem.org/2018):

- Day: Saturday 2018-02-03
- Start time: 15:00:00
- Duration: 50 min
- Room: K.1.105 (La Fontaine)

And a second talk in the Config Management DevRoom:

- Day: Sunday 2018-02-04
- Start time: 30:00
- Duration: 25min
- Room: UA2.114 (Baudoux)

See you in Brussels at 3 and 4 February 2018!

I will also be present in the [Config Management Camp](http://cfgmgmtcamp.eu/)
directly after Fosdem in Gent.

### CC-licenced Book About Vision of Elektra Published

I am proud to release a book describing:

- the last 13 years of Elektra (focus on last 4 years with
  the questionnaire survey and code analysis),
- the current state of Elektra, and
- the long-term goals of Elektra (context-aware configuration).

The Fosdem talk will cover some highlights from the book.

A huge thanks to everyone involved in the questionnaire survey,
without you we would not have been able to collect all the
information that led to the requirements for Elektra.

The LaTeX sources are available [here](https://github.com/ElektraInitiative/book)
and the compiled book can be downloaded from [here](https://github.com/ElektraInitiative/book/raw/master/book/book.pdf).

TODO: https://book.libelektra.org

### Maturing of Plugins

- The new [Directory Value plugin](https://www.libelektra.org/plugins/directoryvalue) supports storage plugins such as [YAJL](https://www.libelektra.org/plugins/yajl) and [YAML CPP ](https://www.libelektra.org/plugins/yamlcpp). It adds extra leaf values for directories (keys with children) that store the data of their parents. This way plugins that normally are only able to store values in leaf keys are able to support arbitrary key sets.
- The [yamlcpp plugin](https://www.libelektra.org/plugins/yamlcpp) TODO
- The [camel plugin](https://www.libelektra.org/plugins/camel)  TODO
- The [mini plugin](https://www.libelektra.org/plugins/mini)  TODO
- The [xerces plugin](https://www.libelektra.org/plugins/xerces)  TODO
- boolean? TODO (is currently described below)
- The [crypto plugin](https://www.libelektra.org/plugins/crypto) and [fcrypt plugin](https://www.libelektra.org/plugins/fcrypt) are described below.

### Elektra With Encryption

The plugins `fcrypt` and `crypto` are now considered stable. They are no longer tagged as `experimental`.
While `crypto` encrypts individual values within configuration files, `fcrypt` encrypts and/or signs the whole configuration file.

For this release Peter Nirschl prepared a demo showing Elektra's cryptographic abilities:

[![asciicast](https://asciinema.org/a/153014.png)](https://asciinema.org/a/153014)

Thanks to Peter Nirschl for this great work!

### Switch to INI

We plan to switch to INI as default storage instead of Elektra's infamous internal dump format.

As preparation work we implemented the `dini` plugin which transparently
converts all `dump` files to `ini` files on any write attempt.
Furthermore, we fixed most of the INI bugs which blocked INI to be the
default storage.

Due to this progress we will likely switch to INI as default starting
with the next release. If you want to, you can switch now by compiling
Elektra with:<br>
`-DKDB_DEFAULT_STORAGE=dini`

Or simply switch for your installation with:<br>
`sudo kdb change-default-storage dini`

If you are already using `ini` as default, changing to `dini` will:

- add some overhead because `dini` always checks if a file uses the `dump`
  format, unless the `dump` plugin is not installed.
- add support for binary values using the `binary` plugin

> NOTE: INI was not completely ready for 0.8.21 thus we kept `dump` as default.

## Other New Features

We added even more functionality, which could not make it to the highlights:

- `kdb rm` now supports `-f` to ignore non-existing keys
- use `%` as profile name to disable reading from any profile

### Libease

- The new function `elektraArrayDecName`:

  ```c
  int elektraArrayDecName (Key * key);
  ```

   decreases the index of an array element by one. It can be used to reverse the effect of `elektraArrayIncName`.

## Documentation

We improved the documentation in the following ways:

- We renamed our beginner friendly issues to "good first issue" as recommended
  by GitHub.
- In many parts of the documentation we already switched to American spelling.
- We extended the ReadMe of the `jni` plugin. The ReadMe now also contains information about the Java prerequisites of the `jni` plugin on Debian Stretch.
- Fixed many spelling mistakes
  thanks to René Schwaiger
- Improved notes about testing
  thanks to Thomas Wahringer
- qt-gui: give hints which package to install
- <<TODO>>

## Compatibility

As always, the ABI and API of kdb.h is fully compatible, i.e. programs
compiled against an older 0.8 version of Elektra will continue to work
(ABI) and you will be able to recompile programs without errors (API).

- added `elektraArrayDecName` and `elektraArrayValidateName` in libease
- fixed `xmlns` and `xsi:schemaLocation` to be `https://www.libelektra.org`

## Notes for Maintainer

These notes are of interest for people maintaining packages of Elektra:

- <<TODO which files added/removed>>
- <<TODO which plugins are now non-experimental>>
- intercept-fs is now marked more clearly as experimental

## Notes for Elektra's Developers

These notes are of interest for people developing Elektra:

- From now on release notes are written as part of PRs
- Elektra Initiative is spelled as two words
- At some more places we switched to use the logger, thanks to René Schwaiger
- Shell Recorder got many improvements, see below.
  Please use it.
- The plugin's template now adds all placements within backends by default
  (must be removed accordingly).
- We now warn if plugins do not have any placement.
- Please prefer -log and -debug builds
- The build server now understands the build phrase `jenkins build all please`
  thanks to René Schwaiger.
  Please use it carefully, since it puts our [build server](https://build.libelektra.org/) under heavy load.
- Markdown Shell Recorder Syntax recommended when reporting bugs.
- Elektra's [Dockerfile](https://master.libelektra.org/doc/docker/Dockerfile) improved, thanks to Thomas Wahringer
- Add more Explanations how to do Fuzz Testing
- Started documenting disabled tests in [doc/todo/TESTING](https://master.libelektra.org/doc/todo/TESTING)

## Testing

- AFL unveiled some crashes in INI code
- fix OCLint problems, thanks to René Schwaiger
- fix ASAN problems, thanks to René Schwaiger
- disabled non-working tests
- Shell recorder
- Benchmark optionally also works with OpenMP, thanks to Kurt Micheli
- <<TODO shell recorder changes?>>

## Refactoring

- Simplify `elektraArrayValidateName`, thanks to René Schwaiger

## Fixes

Many problems were resolved with the following fixes:

- fix use of `dbus_connection_unref(NULL)` API
  thanks to Kai-Uwe Behrmann
- Properly include headers for `std::bind`
  thanks to Nick Sarnie
- qt-gui: assure active focus on appearance selection window
  thanks to Raffael Pancheri
- René Schwaiger repaired the `boolean` plugin:
  - wrong metadata was used
  - plugin configuration was missing
  - documentation was missing
  - logging code was added
- René Schwaiger repaired many problems different build agents had
- `kdb info -l` does not open `KDB` anymore.
- `change-resolver-symlink` and `change-storage-symlink` now correctly use
  `@TARGET_PLUGIN_FOLDER@`
- date plugin will be removed on attempts to compile it with gcc 4.7, thanks to René Schwaiger
- C plugin: storage/c metadata added
- fix disabling documentation in CMake, thanks to Kurt Micheli
- <<TODO>>

## Outlook

The Order Preserving Minimal Perfect Hash Map (OPMPHM) is ready to extend `ksLookup`. The implementation of the randomized Las Vegas hash map
algorithm is in a final stage and the heuristic functions that ensure time and space optimality are backed up by benchmarks.
Thanks to Kurt Micheli, the next release will include the OPMPHM!

## Get It!

You can download the release from [here](https://www.libelektra.org/ftp/elektra/releases/elektra-0.8.21.tar.gz)
or [GitHub](https://github.com/ElektraInitiative/ftp/blob/master/releases/elektra-0.8.21.tar.gz?raw=true)


The [hashsums are:](https://github.com/ElektraInitiative/ftp/blob/master/releases/elektra-0.8.21.tar.gz.hashsum?raw=true)

<<`scripts/generate-hashsums`>>

The release tarball is also available signed by me using GnuPG from
[here](https://www.libelektra.org/ftp/elektra/releases/elektra-0.8.21.tar.gz.gpg) or
[GitHub](https://github.com/ElektraInitiative/ftp/blob/master/releases//elektra-0.8.21.tar.gz.gpg?raw=true)

Already built API-Docu can be found [online](https://doc.libelektra.org/api/0.8.21/html/)
or [GitHub](https://github.com/ElektraInitiative/doc/tree/master/api/0.8.21).


## Stay tuned!

Subscribe to the
[RSS feed](https://www.libelektra.org/news/feed.rss)
to always get the release notifications.

For any questions and comments, please contact the
issue tracker [on GitHub](http://issues.libelektra.org)
or me by email using elektra@markus-raab.org.

[Permalink to this NEWS entry](https://doc.libelektra.org/news/0.8.21-release)


For more information, see [https://libelektra.org](https://libelektra.org)

Best regards,
Markus Raab for the [Elektra Initiative](https://www.libelektra.org/developers/authors)

