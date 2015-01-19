YTMusic
=======

A simple streaming audio player for YouTube media.

Introduction
------------

YTMusic is a way to play your favorite audio media on YouTube without having to
open a browser, and store references to this media in a compact, offline form.
It operates by publishing a RPC service to a client, which interacts with the
user. A simple (badly written) client is shipped with this version.

Installation
------------

YTMusic depends on the following:

gstreamer0.10-alsa

libgstreamer0.10-dev

libncurses5-dev

libprotobuf-dev

libre2-dev

livestreamer

protobuf-compiler

They can be installed on Ubuntu 14.1 and onwards as follows:

```
$ apt-get install gstreamer0.10-alsa
$ apt-get install libgstreamer0.10-dev
$ apt-get install libncurses5-dev
$ apt-get install libprotobuf-dev
$ apt-get install libre2-dev
$ apt-get install livestreamer
$ apt-get install protobuf-compiler
```

To compile the project, run:

```
$ make
```

This will generate all executables for this project.

Since we're not on the chrome store yet, you can install our chrome extension
(located in chrome/ytm-chrome) by going to chrome://extensions/ enabling
developer mode (upper right corner), and loading an unpacked extension.

User Guide
----------

Run the RPC server, located in bin/server after compilation.
Run the user client, located in bin/client after compilation.

Until we implement custom keybindings, the controls are as follows:

- j: Move down.
- k: Move up.
- e: Enqueue song.
- p: Discard current queue and play song.
- s: Stop.
- P: Toggle play/pause.
- h: Prev song.
- l: Next song.
- o: Edit song details. Opens new menu:
  - j: Move down.
  - k: Move up.
  - l: Change song field.
  - h: Exit menu and commit changes.
  - q: Exit menu and don't commit changes.
  - d: Exit menu and delete song.
