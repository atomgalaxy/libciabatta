libciabatta
===========

libciabatta is the sandwich mixin support library. It's really small (51 lines of code!) and it's the most composable.

Header-only.

Example:
--------

Simple include:
```cpp
#include "ciabatta.hpp"
```

Include the stuff the mixins will need:
```cpp
#include <iostream>
#include <utility>
#include <string>

#define FWD(name) std::forward<decltype(name)>(name)
```

Make the example easy: (don't do this at home)

A mixin is a class template that derives from its ownly template parameter:
```cpp
template <typename Base>
struct my_mixin : Base {
    // forwarding constructor - add your params at the front
    my_mixin(auto&&... rest) : Base(FWD(rest)...) {}
    /* your code here */
};
```

A few mixins:
```cpp

using namespace ciabatta;
template <typename Base>
struct stdout_logger : Base {
    template <typename... Ts>
    void log(Ts&&... xs) const { (std::cout << ... << xs); }
};


template <typename Base>
struct ostream_logger : Base {
    ostream_logger(std::ostream& out_, auto&&... rest)
        : Base(FWD(rest)...)
        , _out(&out_) {}

    template <typename... Ts>
    void log(Ts&&... xs) const { ((*_out) << ... << xs); }
  private:
    std::ostream* _out;
};

template <typename Base>
struct echoer : Base {
    echoer(std::string prefix_, auto&&... rest) 
        : Base(FWD(rest)...)
        ,_prefix(std::move(prefix_)) { }
    std::string echo(std::string arg) { return _prefix + arg; }
    private:
    std::string _prefix;
};

template <typename Base>
struct frobnicator : Base {
    frobnicator(auto&&... rest) : Base(FWD(rest)...) {}

    void frobnicate() const { this->self().log("frobnicate."); }
};
```

And this is how you use the library:

Making an easy concrete class that doesn't need to initialize things:

```cpp
struct concrete : mixin<concrete, stdout_logger, frobnicator> {
};
```

Or maybe something more complicated that does initialize things:
```cpp
struct concrete2 : mixin<concrete2, ostream_logger, frobnicator, echoer> {
    concrete2(std::ostream& out_) : mixin(out_, "my prefix") {}
};
```

And this is the test driver.
```cpp
int main() {
    concrete c;
    c.frobnicate();

    concrete2 c2{std::cerr};
    c2.frobnicate();
}
```
