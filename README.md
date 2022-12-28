# HTML5 environment for OpenIVI Mobility

This provides a HTML5 runtime for OpenIVI Mobility applications. Documentation is at [the main OpenIVI docs site](https://openivimobility.github.io/developing/setting-up-an-html5-development-environment.html).

This fork aims to integrate Linux SocketCAN interface to read data from CAN bus instead of a demo file.

The interface is optimized for a 800x480 display, but can be easily adapted to different resolution acting on `mainwindow.ui` and HTML/CSS part.

## Contributing

### Style

Use the google style:
http://google-styleguide.googlecode.com/svn/trunk/cppguide.html

Reformat code using the following before checkin:

    clang-format -style google -i *.cc *.h

### Linting

To use `clang-check` to lint the source tree:

    clang-check -analyze -p build *.cc
