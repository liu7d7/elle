#pragma once

#include <iostream>
#include "stb/stb_image.h"
#include "GLFW/glfw3.h"
#include "event.h"
#include "event_args.h"

namespace engine {
  template<typename S>
  struct app;

  template<typename T, typename S>
  concept is_state_initializer = std::same_as<S, std::invoke_result_t<T, app<S>&>>;

  template<typename S>
  static auto no_init = [](app<S>& self) {
    return S();
  };

  template<typename S>
  struct app {
    int width, height;
    float widthf, heightf;
    string name;

    event<S, move_args> mouse_move;
    event<S, click_args> mouse_click;
    event<S, click_args> mouse_release;
    event<S, click_args> mouse_hold;
    event<S, scroll_args> mouse_scroll;

    event<S, key_args> key_hold;
    event<S, key_args> key_press;
    event<S, key_args> key_release;
    event<S, char_args> char_press;

    event<S, draw_args> draw;
    event<S, update_args> update;
    event<S, no_args> exit;
    event<S, resize_args> resize;

    optional<S> state;

    template<typename I>
    requires is_state_initializer<I, S>
    app(int width, int height, string name, I&& init) :
      width(width), height(height), widthf(float(width)), heightf(float(height)), name(std::move(name)), m_handle(nullptr),
      m_last_x(), m_last_y(), m_mouse_state(GLFW_CURSOR_NORMAL) {
      glfw_init();

      glfw_window_hint(GLFW_VERSION_MAJOR, 3);
      glfw_window_hint(GLFW_VERSION_MINOR, 3);
      glfw_window_hint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfw_window_hint(GLFW_CONTEXT_VERSION_MINOR, 6);
      glfw_window_hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfw_window_hint(GLFW_RESIZABLE, GL_TRUE);
      m_handle = glfw_create_window(width, height, this->name.c_str(), nullptr, nullptr);
      if (!m_handle) {
        throw std::runtime_error("failed to create GLFW window!");
      }

      glfw_make_context_current(m_handle);
      if (!glad_load_gl_loader((GLADloadproc) glfw_get_proc_address)) {
        throw std::runtime_error("failed to load OpenGL!");
      }

      glfw_set_window_user_pointer(m_handle, this);
      glfw_swap_interval(1);
      stbi_set_flip_vertically_on_load(true);

      glfw_set_window_size_callback(m_handle, [](GLFWwindow* active_handle, int width, int height) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        self->width = width;
        self->height = height;
        self->widthf = float(width);
        self->heightf = float(height);
        self->resize(self->state.value(), {width, height});
      });

      glfw_set_char_callback(m_handle, [](GLFWwindow* active_handle, uint codepoint) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        self->char_press(self->state.value(), {codepoint});
      });

      glfw_set_key_callback(m_handle, [](GLFWwindow* active_handle, int keycode, int scancode, int action, int mods) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        switch (action) {
          case GLFW_PRESS:
            return self->key_press(self->state.value(), {keycode, scancode, mods});
          case GLFW_REPEAT:
            return self->key_hold(self->state.value(), {keycode, scancode, mods});
          case GLFW_RELEASE:
            return self->key_release(self->state.value(), {keycode, scancode, mods});
          default:
            throw std::runtime_error(
              "got " + std::to_string(action) + " instead of GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE.");
        }
      });

      glfw_set_cursor_pos_callback(m_handle, [](GLFWwindow* active_handle, double x, double y) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        self->mouse_move(self->state.value(), {
          static_cast<float>(x),
          static_cast<float>(y),
          static_cast<float>(x - self->m_last_x),
          static_cast<float>(y - self->m_last_y)
        });
        self->m_last_x = x;
        self->m_last_y = y;
      });

      glfw_set_mouse_button_callback(m_handle, [](GLFWwindow* active_handle, int button, int action, int mods) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        switch (action) {
          case GLFW_PRESS:
            return self->mouse_click(self->state.value(),
                                     {static_cast<float>(self->m_last_x), static_cast<float>(self->m_last_y), button,
                                      mods});
          case GLFW_REPEAT:
            return self->mouse_hold(self->state.value(),
                                    {static_cast<float>(self->m_last_x), static_cast<float>(self->m_last_y), button, mods});
          case GLFW_RELEASE:
            return self->mouse_release(self->state.value(),
                                       {static_cast<float>(self->m_last_x), static_cast<float>(self->m_last_y), button,
                                        mods});
          default:
            throw std::runtime_error(
              "got " + std::to_string(action) + " instead of GLFW_PRESS, GLFW_REPEAT, or GLFW_RELEASE.");
        }
      });

      glfw_set_scroll_callback(m_handle, [](GLFWwindow* active_handle, double x_off, double y_off) {
        auto self = static_cast<app*>(glfw_get_window_user_pointer(active_handle));
        self->mouse_scroll(self->state.value(), {
          static_cast<float>(self->m_last_x),
          static_cast<float>(self->m_last_y),
          static_cast<float>(x_off),
          static_cast<float>(y_off)
        });
      });

      state.emplace(std::move(init(*this)));
    }

    app(int width, int height, string name) : app(width, height, std::move(name), no_init<S>) {}

    void run() {
      static int age = 0;
      static double last_time = glfw_get_time();
      while (!glfw_window_should_close(m_handle)) {
        glfw_poll_events();
        auto [j, tick_delta] = update_ticks();
        for (int i = 0; i < std::min(10, j); i++) {
          update(state.value(), update_args{.age = age});
          age++;
        }
        draw(state.value(), {glfw_get_time() - last_time});
        last_time = glfw_get_time();
        glfw_swap_buffers(m_handle);
      }

      exit(state.value());

      glfw_set_window_user_pointer(m_handle, nullptr);
      glfw_terminate();
    }

    /* implicit */ inline operator GLFWwindow*() {
      return m_handle;
    }

    int mouse_state() {
      return m_mouse_state;
    }

    int mouse_state(int mouse_state) {
      if (m_mouse_state != mouse_state) {
        glfwSetInputMode(m_handle, GLFW_CURSOR, mouse_state);
      }
      m_mouse_state = mouse_state;
      return mouse_state;
    }

  private:
    GLFWwindow* m_handle;
    double m_last_x, m_last_y;
    int m_mouse_state;

    std::pair<int, float> update_ticks() {
      const float tick_time = 50.f;
      static double last_frame = 0.f;
      static double prev_time = 0.f;
      static double tick_delta = 0.f;
      auto time = glfw_get_time() * 1000.;
      last_frame = (time - prev_time) / tick_time;
      prev_time = time;
      tick_delta += last_frame;
      int i = (int) tick_delta;
      tick_delta -= (float) i;
      return {i, (float) tick_delta};
    }
  };
}