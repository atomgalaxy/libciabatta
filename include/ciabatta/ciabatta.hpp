#ifndef INCLUDED_CIABATTA_HPP
#define INCLUDED_CIABATTA_HPP
/*
 * Copyright 2019 Gašper Ažman
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file ciabatta.hpp
 *
 * The minimal mixin support library. See the README.md for a tutorial.
 */

#include <type_traits>

namespace ciabatta {

template <typename MostDerived>
struct ciabatta_top { /* not a mixin */
  using self_type = MostDerived;
  decltype(auto) self() & { return static_cast<self_type&>(*this); }
  decltype(auto) self() && { return static_cast<self_type&&>(*this); }
  decltype(auto) self() const& { return static_cast<self_type const&>(*this); }
  decltype(auto) self() const&& {
    return static_cast<self_type const&&>(*this);
  }
};

struct deferred {
  deferred() = delete;
};

namespace detail {

template <typename Concrete,
          template <class>
          class H,
          template <class>
          class... Tail>
struct chain_inherit {
  using result = typename chain_inherit<Concrete, Tail...>::type;
  using type = H<result>;
};
// base-case
template <typename Concrete, template <class> class H>
struct chain_inherit<Concrete, H> {
  using type = H<Concrete>;
};

template <typename Concrete, template <class> class... Mixins>
using mixin_impl =
    typename chain_inherit<ciabatta_top<Concrete>, Mixins...>::type;

}  // namespace detail

template <typename Concrete, template <class> class... Mixins>
struct mixin : ::ciabatta::detail::mixin_impl<Concrete, Mixins...> {
  template <typename... Rest>
  constexpr mixin(Rest&&... rest)
      : ::ciabatta::detail::mixin_impl<Concrete, Mixins...>(
            static_cast<decltype(rest)>(rest)...) {}

  mixin() = default;
  mixin(mixin const&) = default;
  mixin(mixin&&) = default;
  mixin& operator=(mixin const&) = default;
  mixin& operator=(mixin&&) = default;
  ~mixin() = default;
};

/**
 * Allows currying of mixin classes with multiple template parameters.
 *
 * For an example, see the `provides` mixin.
 *
 * Basic usage:
 * ```
 * template <typename Arg1, typename Arg2, typename Base>
 * struct my_mixin : Base { CIABATTA_DEFAULT_MIXIN_CTOR(my_mixin, Base); };
 * struct myclass
 *   : ciabatta::mixin<myclass,
 *                     ciabatta::curry<my_mixin, int, float>::mixin
 *                    > {};
 * ```
 */
template <template <class...> class Mixin, typename... Args>
struct curry {
  template <typename Base>
  using mixin = Mixin<Args..., Base>;
};

}  // namespace ciabatta

/**
 * The empty forwarding constructor is so common that automating the pattern
 * through a macro is a good idea.
 *
 * The only parameter to the macro is the class name, therefore, the
 * constructor.
 */
#define CIABATTA_DEFAULT_MIXIN_CTOR(CLS, BASE)        \
  template <typename... Args>                         \
  constexpr CLS(Args&&... args)                       \
      : BASE(static_cast<decltype(args)>(args)...) {} \
  static_assert(true, "require semicolon")

namespace ciabatta::mixins {

/**
 * The "provides" mixin.
 *
 * @param Interface - the abstract base class to add to the interface. The rest
 * are implementation details.
 * 
 * Example:
 * ```cpp
 * struct concrete : ciabatta::mixin<concrete, ciabatta::mixins::provides<MyAbc>::mixin> {
 * };
 * ```
 */
template <typename Interface, typename Base = ::ciabatta::deferred>
struct provides : Base, Interface {
  template <typename Base_>
  using mixin = typename curry<provides, Interface>::template mixin<Base_>;

  CIABATTA_DEFAULT_MIXIN_CTOR(provides, Base);
};

}  // namespace ciabatta::mixins

#endif
