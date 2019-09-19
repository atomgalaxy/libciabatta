#include "ciabatta/ciabatta.hpp"

#define FWD(name) std::forward<decltype(name)>(name)
#include <iostream>

template <typename Base>
struct stdout_logger : Base {
  template <typename... Ts>
  void log(Ts&&... xs) const {
    (std::cout << ... << xs);
  }
};

template <typename Base>
struct ostream_logger : Base {
  ostream_logger(std::ostream& out_, auto&&... rest)
      : Base(FWD(rest)...), _out(&out_) {}

  template <typename... Ts>
  void log(Ts&&... xs) const {
    ((*_out) << ... << xs);
  }

 private:
  std::ostream* _out;
};

template <typename Base>
struct echoer : Base {
  echoer(std::string prefix_, auto&&... rest)
      : Base(FWD(rest)...), _prefix(std::move(prefix_)) {}
  std::string echo(std::string arg) { return _prefix + arg; }

 private:
  std::string _prefix;
};

template <typename Base>
struct frobnicator : Base {
  frobnicator(auto&&... rest) : Base(FWD(rest)...) {}

  void frobnicate() const { this->self().log("frobnicate."); }
};
struct concrete : ciabatta::mixin<concrete, stdout_logger, frobnicator> {};
struct concrete2
    : ciabatta::mixin<concrete2, ostream_logger, frobnicator, echoer> {
  concrete2(std::ostream& out_) : mixin(out_, "my prefix") {}
};

int main() {
  concrete c;
  c.frobnicate();

  concrete2 c2{std::cerr};
  c2.frobnicate();
}