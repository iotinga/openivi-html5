# HTML5 environment for OpenIVI Mobility

This provides a HTML5 runtime for OpenIVI Mobility applications. Documentation is at [the main OpenIVI docs site](https://openivimobility.github.io/developing/setting-up-an-html5-development-environment.html).

This fork aims to integrate some more features:

- [ofono](https://git.kernel.org/pub/scm/network/ofono/ofono.git/) interface to control connected phone via bluetooth.

The interface is optimized for a 800x480 display, but can be easily adapted to different resolution acting on `mainwindow.ui` and HTML/CSS part.

# Ofono

When developing on local machine you may see that Ofono is not fetching modems correctly. That could be caused by configuration of the ofono dbus service:
you need to check the `/etc/dbus-1/system-d/ofono.conf` file and see what policy is being used. If the policy allows operations for `root` user only you can modify that
to match your user or group.

## Contributing

### Style

Use the google style:
http://google-styleguide.googlecode.com/svn/trunk/cppguide.html

Reformat code using the following before checkin:

    clang-format -style google -i *.cc *.h

### Linting

To use `clang-check` to lint the source tree:

    clang-check -analyze -p build *.cc
