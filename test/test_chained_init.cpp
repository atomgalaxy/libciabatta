#include "ciabatta/ciabatta.hpp"

#include <utility>

namespace test_chained_init {

struct arg1 {
  int a1;
};
struct arg2 {
  int a2;
};

struct arg3 {
  int a3;
};
struct arg4 {
  int a4;
};

template <typename B>
struct fragment_a : B {
  template <typename... Args>
  fragment_a(arg1 a1, arg2 a2, Args&&... args)
      : B(std::forward<Args>(args)...), x{a1.a1}, y{a2.a2} {}
  int x;
  int y;
};

template <typename B>
struct fragment_b : B {
  template <typename... Args>
  fragment_b(arg3 a3, arg4 a4, Args&&... args)
      : B(std::forward<Args>(args)...), z{a3.a3}, w{a4.a4} {}
  int z;
  int w;
};

struct composite : ciabatta::mixin<composite, fragment_a, fragment_b> {
  constexpr composite(arg1 a1, arg2 a2, arg3 a3, arg4 a4)
      : mixin(a1, a2, a3, a4) {}
};

}  // namespace test_chained_init

int main(int argc, char** argv) {
  using namespace test_chained_init;
  auto x = composite{arg1{1}, arg2{2}, arg3{3}, arg4{4}};
  return x.x + x.y + x.z + x.w - 10;
}