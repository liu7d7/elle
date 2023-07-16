#pragma once

#include "engine/usings.h"
#include "engine/layout.h"
#include "engine/box.h"
#include "elle/state.h"

namespace elle {
  using namespace engine;

  struct entity {
    vec3 pos;
    box box;

    virtual void quick_update(state& s) = 0;
    virtual void draw(state& s) = 0;
  };
}