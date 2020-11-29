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
  - [Using With `bazel`](#using-with-bazel)
  - [Using With `cmake`](#using-with-cmake)
    - [As an external dependency: `find_package`](#as-an-external-dependency-findpackage)
      - [Installing under `$PREFIX`:](#installing-under-prefix)
    - [As a subproject: `add_subdirectory`](#as-a-subproject-addsubdirectory)
  - [Tutorial](#tutorial)
    - [Includes](#includes)
    - [Ciabatta Mixin Concept Requirements](#ciabatta-mixin-concept-requirements)
    - [Our First Simple Mixin](#our-first-simple-mixin)
    - [Mixins with Data](#mixins-with-data)
    - [Putting Mixins Together](#putting-mixins-together)
  - [Advanced Techniques](#advanced-techniques)
    - [Providing an Abstract Interface](#providing-an-abstract-interface)
    - [Templated Mixins](#templated-mixins)
  - [Testing this package](#testing-this-package)
    - [Testing the build/installation](#testing-the-buildinstallation)


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

Ciabatta provides:

- The top slice that lets your mixins refer to each other
- The tools to compose mixins with minimal boilerplate
- The mixin concept: following this concept will make your mixins interoperable


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

### As an external dependency: `find_package`

If *ciabatta* is installed:

```cmake
find_package(ciabatta REQUIRED)

add_executable(test_example example.cpp)
target_link_libraries(test_example PUBLIC ciabatta::ciabatta)
```

If installed under `$PREFIX`: (`$PREFIX` is a unix dir-tree base which
includes `include/`, `share/` etc.)

```sh
cd build
cmake -DCMAKE_PREFIX_PATH=$PREFIX ../
```

should do the trick.

#### Installing under `$PREFIX`:

Run something akin to the following:

```sh
mkdir build/  # don't build in the same directory
cd build
cmake -DCMAKE_INSTALL_PREFIX=$PREFIX ../ -GNinja
ninja
ninja test
ninja install
```

### As a subproject: `add_subdirectory`

Check out this repository as a submodule in your `thirdparty/` folder in your
repo, then add this to your `CMakeLists.txt`:

```cmake
add_subdirectory(thirdparty/ciabatta)
```

Tutorial
--------

### Includes

Simple include:

```cpp
#include "ciabatta/ciabatta.hpp"
```

Include the stuff your mixins will need (ciabatta doesn't need any of these):

```cpp
#include <iostream>
#include <utility>
#include <string>
```

Make the example easy: (don't do this in headers)

```cpp
#define FWD(name) std::forward<decltype(name)>(name)
```

### Ciabatta Mixin Concept Requirements

Ciabatta doesn't place much restriction on mixins. You can do whatever you
like, pretty much, as long as you conform to the two rules:

1. A **ciabatta-compatible mixin** is a class template that inherits
    publically from its _only_ template parameter. (It is allowed to *also*
    inherit from other things.)
2. It must forward all constructor arguments that it doesn't use to its base.

```cpp
template <typename Base>
struct minimal_mixin : Base {
    // A mixin must forward the constructor parameters it does not consume
    // add the parameters to your mixin at the front, before `rest`
    minimal_mixin(auto&&... rest) : Base(FWD(rest)...) {}
    // the rest of the implementation here
};
```

That's it!

**Note:** Before C++20, the constructor needs to be written as

```cpp
template <typename...Args>
minimal_mixin(Args&&...rest) : Base(std::forward<Args>(rest)...) {}
```

Because this is so common, *ciabatta* has a macro for you:

```cpp
CIABATTA_DEFAULT_MIXIN_CTOR(minimal_mixin, Base);
```

### Our First Simple Mixin

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

### Mixins with Data

However, if we wanted to log through `std::ostream`, we would need some
state, which needs to be initialized. We do this by peeling off parameters
from the constructor call, and forwarding the rest:

```cpp
template <typename Base>
struct ostream_logger : Base {
    ostream_logger(std::ostream& out_, auto&&... rest)
        : Base(FWD(rest)...)
        , _out(&out_) {}

    void log(auto&&... xs) { ((*_out) << ... << xs); }
  private:
    std::ostream* _out;
};
```

**Note the private data**: *nothing* in the composed class can touch `_out`.
This means that you get back your encapsulation, instead of having your
entire class inline, with its 150+ members, and every function having
promiscuous access to everything.

### Putting Mixins Together

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

Advanced Techniques
-------------------

Here are some more advanced techniques one can do with mixins:

### Providing an Abstract Interface

Oftentimes, composed classes need to implement some abstract interface, with
the various mixins providing the implementation of it. But how can we expose
the fact that we are, in fact, implementing one?

We multiply-inherit from `Base` and the interface, of course :)

**Example**

Our interface will be `abstract_socket`:

```cpp
struct message { /* payload */
};
struct abstract_socket {
  virtual void receive(message) = 0;
  virtual void send(message) = 0;
};
```

We should probably provide some null implementations for mocking:

```cpp
template <typename Base>
struct null_sender : Base {
  CIABATTA_DEFAULT_MIXIN_CTOR(null_sender, Base);
  void send(message m) final {}
};
```

```cpp
template <typename Base>
struct null_receiver : Base {
  CIABATTA_DEFAULT_MIXIN_CTOR(null_receiver, Base);
  void receive(message m) final {}
};
```

Let's make a mixin that adds the vtable and `abstract_socket&` conversion capability:

```cpp
template <typename Base>
struct is_socket : Base, abstract_socket {
  CIABATTA_DEFAULT_MIXIN_CTOR(is_socket, Base);
};
```

And now we can make our concrete `null_socket` class:

```cpp
struct null_socket
    : ciabatta::mixin<null_socket, null_sender, null_receiver, is_socket> {};
```

*Note:* `is_socket` has to be last if you want the ability to mark member
functions `final`.

If you don't want to define a new class just to inject an abstract interface,
*ciabatta* has your back:

```cpp
struct null_socket2
    : ciabatta::mixin<null_socket2,
                      null_sender,
                      null_receiver,
                      ciabatta::mixins::provides<abstract_socket>::mixin> {};
```

### Templated Mixins

Sometimes, like in the `ciabatta::mixins::provides` mixin, we need more than
one template parameter.

This is enabled by the `curry` facility. Let's take a look at a simplistic
`provides` mixin:

```cpp
template <typename Interface, typename Base>
struct provides : Base {
    CIABATTA_DEFAULT_MIXIN_CTOR(provides, Base);
};
```

To use `provides` as-is, we can just supply the `interface` parameter to
`curry`, like so:

```cpp
struct null_socket3
    : ciabatta::mixin<
          null_socket3,
          null_sender,
          null_receiver,
          ciabatta::curry<provides, abstract_socket>::mixin> {
};
```

In fact, this is what `ciabatta::mixins::provides` does with its `mixin`
inner templated type alias.


Testing this package
--------------------

You can run the tests with either `bazel test //...` or build this package
with cmake and use `ctest` in the built test directory.


### Testing the build/installation

the [Makefile](Makefile) at the root of the project automates this process.
It checks that once installed, the package exports the required include paths
etc. by builing the tests with a "foreign" (installed) *libciabatta*.
