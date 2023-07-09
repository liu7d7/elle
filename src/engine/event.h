#pragma once

#include <memory>
#include "usings.h"
#include "event_args.h"

namespace engine {
  template<typename S, typename T>
  struct event {
    using handler_type = void (*)(S&, T const&);
    vector<handler_type> handlers;

    inline void operator +=(handler_type handler) {
      handlers.push_back(handler);
    }

    inline void operator ()(S& state) const {
      for (auto it : handlers) {
        it(state, T());
      }
    }

    inline void operator ()(S& state, T const& args) const {
      for (auto it : handlers) {
        it(state, args);
      }
    }
  };
}