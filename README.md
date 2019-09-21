libciabatta
===========

Ciabatta is the sandwich mixin support library. It's really small and most
composable.

Initially written by Gašper Ažman for a talk at C++London.

Header-only.

License
-------

This material is offered free of charge under the Apache 2 license. See
[LICENSE](LICENSE) for details. Apache 2 is a permissive, GPL-compatible,
non-copyleft license.


Table of Contents <!-- omit in toc -->
-----------------
- [libciabatta](#libciabatta)
  - [License](#license)
  - [Introduction](#introduction)
  - [Example:](#example)
  - [Using With `bazel`](#using-with-bazel)
  - [Using With `cmake`](#using-with-cmake)


Introduction
------------

We all know the guideline: don't write large classes. But sometimes you need
to. Or sometimes, inheritance is the only form of composition that will do,
because you need interface composition.

This is where *ciabatta* comes in.

Composed classes are like a sandwitch. The final composed class is the bottom
slice. The sandwich rests on it. All the toppings are the mixins. And
finally, there is the top slice, which finishes up a sandwich, and makes it a
sandwich - the CRTP provider.

Ciabatta provides this top slice, plus the tools to compose mixins without
much boilerplate.

Example:
--------

Simple include:
```cpp
#include "ciabatta/ciabatta.hpp"
```

Include the stuff the mixins will need:
```cpp
#include <iostream>
#include <utility>
#include <string>
```

Make the example easy: (don't do this in headers)
```cpp
#define FWD(name) std::forward<decltype(name)>(name)
```


A mixin is a class template that derives from its ownly template parameter:
```cpp
template <typename Base>
struct minimal_mixin : Base {
    // A mixin must forward the constructor parameters it does not consume
    // add the parameters to your mixin at the front, before `rest`
    minimal_mixin(auto&&... rest) : Base(FWD(rest)...) {}
    // the rest of the implementation here
};
```

Not a whole lot of boilerplate, right?

A mixin is fundamentally a class fragment - it's useless if it can't refer to
other parts of the class. The top slice provides this capability through
providing the four overloads of `self()`:

```cpp
template <typename Base>
struct frobnicator : Base {
    frobnicator(auto&&... rest) : Base(FWD(rest)...) {}

    void frobnicate() {
        // self() is provided by Base, so we need to mark it dependent.
        // this->self() is short and obvious.
        this->self().log("frobnicate.");
    }
};
```

Of course, `log()` needs to be provided by the concrete class somehow,
perhaps through a separate mixin:

```cpp
template <typename Base>
struct stdout_logger : Base {
    void log(auto&&... xs) const { (std::cout << ... << xs); }
};
```

However, if we wanted to log through `std::ostream`, we would need some
state, which needs to be initialized. We do this by peeling off parameters
from the constructor call, and forwarding the rest:

```cpp
template <typename Base>
struct ostream_logger : Base {
    ostream_logger(std::ostream& out_, auto&&... rest)
        : Base(FWD(rest)...)
        , _out(&out_) {}

    void log(auto&&... xs) const { ((*_out) << ... << xs); }
  private: // note the private - we don't want another mixin touching this!
    std::ostream* _out;
};
```

We can now compose several different concrete classes based on this library
of mixins.

Making an easy concrete class that doesn't need to initialize things:
```cpp
struct concrete : ciabatta::mixin<concrete, stdout_logger, frobnicator> {
};
```

Or maybe something more complicated that does initialize things:
```cpp
struct concrete2 : ciabatta::mixin<concrete2, ostream_logger, frobnicator, echoer> {
    concrete2(std::ostream& out_) : mixin(out_, "my prefix") {}
};
```

For completeness, the test driver:
```cpp
int main() {
    concrete c;
    c.frobnicate();

    concrete2 c2{std::cerr};
    c2.frobnicate();
}
```

Using With `bazel`
------------------

Put this into your `WORKSPACE`:
```py
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
git_repository(
    name = "ciabatta",
    remote = "git@github.com:atomgalaxy/libciabatta.git",
    tag = "release", # or version-N, look at the repo for tags
)
```

For using with a target:
```py
cc_binary(
    name = "my_program",
    srcs = ["my_program.cpp"],
    deps = [
        "@ciabatta//",  # for "@ciabatta//:ciabatta"
    ]
)
```

The tags will be updated when/if the repo changes.


Using With `cmake`
------------------

TBD