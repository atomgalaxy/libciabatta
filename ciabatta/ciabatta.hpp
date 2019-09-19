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

namespace ciabatta {

template <typename MostDerived>
struct top { /* not a mixin */
  using self_type = MostDerived;
  decltype(auto) self() & { return static_cast<self_type&>(*this); }
  decltype(auto) self() && { return static_cast<self_type&&>(*this); }
  decltype(auto) self() const& { return static_cast<self_type const&>(*this); }
  decltype(auto) self() const&& {
    return static_cast<self_type const&&>(*this);
  }
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
using mixin_impl = typename chain_inherit<top<Concrete>, Mixins...>::type;

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

}  // namespace ciabatta

#endif
