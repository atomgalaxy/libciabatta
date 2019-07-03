#ifndef INLCUDED_CIABATTA_HPP
#define INLCUDED_CIABATTA_HPP

#include <utility>

#define CIABATTA_FWD(name) std::forward<decltype(name)>(name)

namespace ciabatta {

template <typename MostDerived>
struct top { /* not a mixin */
    using self_type = MostDerived;
    decltype(auto) self() &       { return static_cast<self_type&>(*this);        }
    decltype(auto) self() &&      { return static_cast<self_type&&>(*this);       }
    decltype(auto) self() const&  { return static_cast<self_type const&>(*this);  }
    decltype(auto) self() const&& { return static_cast<self_type const&&>(*this); }
};

namespace detail {
template <typename Concrete, template <class> class H, template <class> class... Tail>
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

} // namespace detail

template <typename Concrete, template <class> class... Mixins>
struct mixin : ::ciabatta::detail::mixin_impl<Concrete, Mixins...> {
    mixin(auto&&... rest) 
        : ::ciabatta::detail::mixin_impl<Concrete, Mixins...>(CIABATTA_FWD(rest)...) {}
    mixin() = default;
    mixin(mixin const&) = default;
    mixin(mixin&&) = default;
    mixin& operator=(mixin const&) = default;
    mixin& operator=(mixin&&) = default;
    ~mixin() = default;
};

} // namespace ciabatta

#undef CIABATTA_FWD
#endif
