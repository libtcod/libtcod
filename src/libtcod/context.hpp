/* BSD 3-Clause License
 *
 * Copyright © 2008-2023, Jice and the libtcod contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LIBTCOD_CONTEXT_HPP_
#define LIBTCOD_CONTEXT_HPP_
#include <array>
#include <filesystem>
#include <stdexcept>

#include "config.h"
#include "console.h"
#include "console_types.h"
#include "context.h"
#include "context_init.h"
#include "context_viewport.h"
#include "error.hpp"
#include "mouse_types.h"
#include "tileset.hpp"

namespace tcod {
/***************************************************************************
    @brief A C++ managed tcod context.

    @details
    \rst
    See :ref:`getting-started` for complete examples of how to setup libtcod projects.
    \endrst

    @code{.cpp}
    // The absolute minimum needed to create a new context in C++.
    int main(int argc, char* argv[]) {
      auto params = TCOD_ContextParams{};
      params.tcod_version = TCOD_COMPILEDVERSION;
      auto context = tcod::Context(params);
    }
    @endcode

    \rst
    .. versionadded:: 1.21
    \endrst
 */
class Context {
 public:
  Context() = default;
  /***************************************************************************
      @brief Construct a new Context object using the provided parameters.

      Requires SDL support enabled, otherwise this will throw.
   */
  explicit Context(const TCOD_ContextParams& params) {
#ifndef NO_SDL
    struct TCOD_Context* context = nullptr;
    check_throw_error(TCOD_context_new(&params, &context));
    context_ = ContextPtr{context};
#else
    throw std::logic_error("Libtcod not compiled with SDL support, so it can not create its own context.");
#endif  // NO_SDL
  };
  /// Take ownsership of a smart pointer to TCOD_Context.
  explicit Context(ContextPtr&& ptr) : context_{std::move(ptr)} {}
  /// Take ownsership of a raw TCOD_Context pointer.
  explicit Context(TCOD_Context* ptr) : context_{ptr} {}

  // Copy disabled, move allowed.
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
  Context(Context&&) = default;
  Context& operator=(Context&&) = default;

  /***************************************************************************
      @brief Return the TCOD_renderer_t value of this context which may be different than the one requested.
   */
  [[nodiscard]] auto get_renderer_type() noexcept -> TCOD_renderer_t {
    return static_cast<TCOD_renderer_t>(TCOD_context_get_renderer_type(context_.get()));
  }
  /***************************************************************************
      @brief Present a console to the display with the provided viewport options.

      @param console The TCOD_Console to render.  This console can be any size.
      @param viewport The viewport options, which can change the way the console is scaled.

      @details
      @code{.cpp}
        // tcod::Context context;
        while (1) {
          auto console = context.new_console();  // This can be done as an alternative to clearing the console.
          tcod::print(console, {0, 0}, "Hello world", nullptr, nullptr);
          auto viewport_options = TCOD_ViewportOptions{}
          viewport_options.tcod_version = TCOD_COMPILEDVERSION;
          viewport_options.keep_aspect = true;  // Adds letterboxing to keep aspect.
          viewport_options.integer_scaling = true;  // Prevent scaling artifacts with pixelated or low-res glyphs.
          viewport_options.clear_color = {0, 0, 0, 255};
          viewport_options.align_x = 0.5f;
          viewport_options.align_y = 0.5f;
          context.present(console, viewport_options);
          SDL_Event event;
          while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) std::exit(EXIT_SUCCESS);
          }
        }
      @endcode
   */
  void present(const TCOD_Console& console, const TCOD_ViewportOptions& viewport) {
    tcod::check_throw_error(TCOD_context_present(context_.get(), &console, &viewport));
  }
  /***************************************************************************
      @brief Present a console to the display.

      @param console The TCOD_Console to render.  This console can be any size and will be stretched to fit the window.
   */
  void present(const TCOD_Console& console) {
    tcod::check_throw_error(TCOD_context_present(context_.get(), &console, nullptr));
  }
  /***************************************************************************
      @brief Return a non-owning pointer to the SDL_Window used by this context.

      @return A ``struct SDL_Window*`` pointer.  This will be nullptr if this context does not use an SDL window.

      @details
      @code{.cpp}
        // tcod::Context context;
        if (SDL_Window* sdl_window = context.get_sdl_window(); sdl_window) {
          // Do anything with an SDL window, for example:
          uint32_t flags = SDL_GetWindowFlags(sdl_window);
        }
      @endcode
   */
  [[nodiscard]] auto get_sdl_window() noexcept -> struct SDL_Window* {
    return TCOD_context_get_sdl_window(context_.get());
  }
  /***************************************************************************
      @brief Return a non-owning pointer to the SDL_Renderer used by this context.

      @return A ``struct SDL_Renderer*`` pointer.  This will be nullptr if this context does not use SDL's renderer.

      @details
      @code{.cpp}
        // tcod::Context context;
        if (SDL_Renderer* sdl_renderer = context.get_sdl_renderer(); sdl_renderer) {
          // Do anything with an SDL renderer, for example:
          SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
          SDL_RenderClear(sdl_renderer);
          SDL_RenderPresent(sdl_renderer);
        }
      @endcode
   */
  [[nodiscard]] auto get_sdl_renderer() noexcept -> struct SDL_Renderer* {
    return TCOD_context_get_sdl_renderer(context_.get());
  }
  /***************************************************************************
      @brief Convert pixel coordinates to this contexts integer tile coordinates.
   */
  [[nodiscard]] auto pixel_to_tile_coordinates(const std::array<int, 2>& xy) -> std::array<int, 2> {
    std::array<int, 2> out{xy[0], xy[1]};
    tcod::check_throw_error(TCOD_context_screen_pixel_to_tile_i(context_.get(), &out[0], &out[1]));
    return out;
  }
  /***************************************************************************
      @brief Convert pixel coordinates to this contexts sub-tile coordinates.
   */
  [[nodiscard]] auto pixel_to_tile_coordinates(const std::array<double, 2>& xy) -> std::array<double, 2> {
    std::array<double, 2> out{xy[0], xy[1]};
    tcod::check_throw_error(TCOD_context_screen_pixel_to_tile_d(context_.get(), &out[0], &out[1]));
    return out;
  }
  /***************************************************************************
      @brief Convert the pixel coordinates of SDL mouse events to the tile coordinates of the current context.

      @param event Any SDL_Event event.
          If the event type is compatible then its coordinates will be converted into tile coordinates.

      @details
      @code{.cpp}
        // tcod::Context context;
        while (1) {
          SDL_Event event;
          while (SDL_PollEvent(&event)) {
            SDL_Event event_tile = event;  // A copy of `event` using tile coordinates.
            context.convert_event_coordinates(event_tile);
            switch (event.type) {
              case SDL_QUIT:
                std::exit(EXIT_SUCCESS);
              case SDL_MOUSEMOTION:
                event.motion.xrel; // Relative motion in pixels.
                event_tile.motion.xrel; // Relative motion in tiles.
                break;
            }
          }
        }
      @endcode
      \rst
      .. versionadded:: 1.19
      \endrst
   */
  void convert_event_coordinates(SDL_Event& event) {
    tcod::check_throw_error(TCOD_context_convert_event_coordinates(context_.get(), &event));
  }
  /***************************************************************************
      @brief Save a screenshot to `path`.

      @param path The file path to save the screenshot to.
   */
  void save_screenshot(const std::filesystem::path& path) {
    tcod::check_throw_error(TCOD_context_save_screenshot(context_.get(), path.string().c_str()));
  }
  /***************************************************************************
      @brief Save a screenshot with a unique filename in the working directly.
   */
  void save_screenshot() { tcod::check_throw_error(TCOD_context_save_screenshot(context_.get(), nullptr)); }
  /***************************************************************************
      @brief Return a new console with a size automatically determined by the context.

      @param min_columns The minimum width to use for the new console, in tiles.
      @param min_rows The minimum height to use for the new console, in tiles.
      @param magnification Determines the apparent size of the tiles that will be rendered by a console created with
        the output values.
        A `magnification` larger then 1.0f will output smaller console parameters, which will show as larger tiles when
        presented.
        Only values larger than zero are allowed.
      @return Returns a tcod::Console of a dynamic size.

      @details
      @code{.cpp}
        // tcod::Context context;
        while (1) {
          auto console = context.new_console();  // Can be an alternative to clearing the console.
          tcod::print(console, {0, 0}, "Hello world", std::nullopt, std::nullopt);
          context.present(console);
          SDL_Event event;
          while (SDL_PollEvent(&event)) {  // SDL_PollEvent may resize the window.
            if (event.type == SDL_QUIT) std::exit(EXIT_SUCCESS);
          }
        }
      @endcode
   */
  [[nodiscard]] auto new_console(int min_columns = 1, int min_rows = 1, float magnification = 1.0f) -> tcod::Console {
    int columns;
    int rows;
    if (magnification <= 0.0f) {
      throw std::invalid_argument(
          std::string("Magnification must be greater than zero. Got ") + std::to_string(magnification));
    }
    tcod::check_throw_error(TCOD_context_recommended_console_size(context_.get(), magnification, &columns, &rows));
    return tcod::Console{std::max(columns, min_columns), std::max(rows, min_rows)};
  }
  /***************************************************************************
      @brief Replace this contexts tileset with a different one.
   */
  auto change_tileset(tcod::Tileset& new_tileset) -> void {
    check_throw_error(TCOD_context_change_tileset(context_.get(), new_tileset.get()));
  }
  /***************************************************************************
      @brief Manually set the pixel-to-tile mouse position transformation.

      @param transform The transform to assign to the context.

      @versionadded{1.24}
   */
  auto set_mouse_transform(const TCOD_MouseTransform& transform) -> void {
    check_throw_error(TCOD_context_set_mouse_transform(context_.get(), &transform));
  }

  /***************************************************************************
      @brief Access the context pointer.  Modifying this pointer may make the class invalid.
   */
  [[nodiscard]] auto get_ptr() noexcept -> ContextPtr& { return context_; }
  /***************************************************************************
      @brief Access the const context pointer.
   */
  [[nodiscard]] auto get_ptr() const noexcept -> const ContextPtr& { return context_; }
  /***************************************************************************
      @brief Close and delete the objects managed by this context.  This object will no longer be valid unless reset.
   */
  auto close() -> void { *this = Context(); }

 private:
  ContextPtr context_ = nullptr;
};
}  // namespace tcod
#endif  // LIBTCOD_CONTEXT_HPP_
