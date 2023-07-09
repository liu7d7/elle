#pragma once

#include "usings.h"

namespace engine {
  template<typename T, typename... Args>
  struct lazy {
    using value_type = T;
    using tuple_type = tuple<Args...>;

    template <size_t... Indices>
    struct indices {};

    template <std::size_t N, std::size_t... Is>
    struct build_indices
      : build_indices<N-1, N-1, Is...> {};

    template <std::size_t... Is>
    struct build_indices<0, Is...> : indices<Is...> {};

    template <typename Tuple>
    using indices_for = build_indices<tuple_size_v<tuple_type>>;

    template <size_t... Indices>
    void construct(indices<Indices...>) {
      if (!value)
        value.emplace(std::get<Indices>(std::forward<tuple_type>(tup))...);
    }

    explicit lazy(Args&&... args) : tup(std::forward<Args>(args)...) {}

    value_type& operator*() {
      construct(indices_for<tuple_type>{});
      return *value;
    }

    value_type* operator->() {
      construct(indices_for<tuple_type>{});
      return &(*value);
    }

    value_type const& operator*() const {
      construct(indices_for<tuple_type>{});
      return *value;
    }

    value_type const* operator->() const {
      construct(indices_for<tuple_type>{});
      return &(*value);
    }

  private:
    optional<value_type> value;
    tuple_type tup;
  };
}