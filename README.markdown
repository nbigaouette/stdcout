stdcout
================================

Content
-------------------------

C++ library to "overload" the std::cout operator so that everything that is
printed to screen can be saved to an output file.

Compilation
-------------------------
You will need zlib (at least) version 1.2.5 from http://zlib.net/ if you want
compression. To disable compression, comment Makefile's line 30.

To build and install:

``` bash
$ make gcc shared static install
```

By default, it will be installed in ${HOME}/usr. You can install somewhere else
using DESTDIR:

``` bash
$ make gcc shared static install DESTIR=/usr
```

Library name are libstdcout.so and libstdcout.a.

Here's a usage example:

``` C++
    // Open the file. If compression is enabled, ".gz" will be appended at end of file
    std_cout.open("output/log.txt");

    // Save some data
    std_cout << "Call to std_cout <<... #1\n" << std::flush;

    // Similar to printf()
    log("a = %d\nb = %f  c = %g or %.15g\n", 2, 3.14, 3.14159265358979, 3.14159265358979);
```


License
-------------------------

This code is distributed under the terms of the [GNU General Public License v3 (GPLv3)](http://www.gnu.org/licenses/gpl.html) and is Copyright 2011 Nicolas Bigaouette.

