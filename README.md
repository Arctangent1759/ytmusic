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

User Guide
----------
