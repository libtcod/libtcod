/* BSD 3-Clause License
 *
 * Copyright © 2008-2021, Jice and the libtcod contributors.
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
#ifndef LIBTCOD_TIMER_H_
#define LIBTCOD_TIMER_H_
#ifdef __cplusplus
#include <SDL_timer.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "config.h"

namespace tcod {
/***************************************************************************
    @brief A timing class based on SDL's high performance time counter.  Used to track delta time or set a framerate.

    This class is based on using `SDL_GetPerformanceCounter` to track the time.
    The time taken between calls to sync() is tracked.  This is used to determine the real framerate if requested.

    You must add ``#include <libtcod/timer.h>`` to include ths class.

    @code{.cpp}
      int desired_fps = 30;
      auto timer = tcod::Timer();
      while (1) {
        float delta_time = timer.sync(desired_fps);  // desired_fps is optional.
        // ...
    @endcode
    \rst
    .. versionadded:: 1.19
    \endrst
 */
class [[nodiscard]] Timer {
 public:
  /***************************************************************************
      @brief Construct a new Timer object.
   */
  Timer() : last_time_{SDL_GetPerformanceCounter()} {}
  /***************************************************************************
      @brief Sync the time to a given framerate (if provided) and return the delta time compared to the previous call.

      If `desired_fps` is non-zero then this function will block until the desired framerate is reached.

      Timing starts once the Timer is constructed.

      @param desired_fps The desired framerate in frames-per-second, or zero to disable framerate limiting.
      @return The delta time in seconds since the last call to sync is returned as a float.
   */
  float sync(int desired_fps = 0) {
    const uint64_t frequency = SDL_GetPerformanceFrequency();
    uint64_t current_time = SDL_GetPerformanceCounter();  // The precise current time.
    int64_t delta_time = static_cast<int64_t>(current_time - last_time_);  // The precise delta time.
    if (desired_fps > 0) {
      const int64_t desired_delta_time = frequency / desired_fps;  // Desired precise delta time.
      const int64_t time_until_next_frame_ms =
          (desired_delta_time - delta_time) * 1000 / static_cast<int64_t>(frequency);
      if (time_until_next_frame_ms > 2) {
        // Sleep until 1 millisecond before the target time.
        SDL_Delay(static_cast<uint32_t>(time_until_next_frame_ms) - 1);
      }
      while (delta_time < desired_delta_time) {  // Spin for the remaining time.
        current_time = SDL_GetPerformanceCounter();
        delta_time = static_cast<int64_t>(current_time - last_time_);
      }
    }
    last_time_ = current_time;
    const float delta_time_s = std::max(0.0f, static_cast<float>(delta_time) / frequency);  // Delta time in seconds.
    // Drop samples as they hit the total time and count limits.
    double total_time = std::accumulate(samples_.begin(), samples_.end(), 0.0);  // Total time of all samples.
    while (!samples_.empty() && (total_time > MAX_SAMPLES_TIME || samples_.size() >= MAX_SAMPLES_COUNT)) {
      total_time -= samples_.front();
      samples_.pop_front();
    }
    samples_.push_back(delta_time_s);
    return delta_time_s;
  }
  /***************************************************************************
      @brief Return the mean framerate.  This is the average of all samples combined.
   */
  [[nodiscard]] float get_mean_fps() const noexcept {
    if (samples_.empty()) return 0;
    const double total_time = std::accumulate(samples_.begin(), samples_.end(), 0.0);
    if (total_time == 0) return 0;
    return 1.0f / static_cast<float>(total_time / static_cast<double>(samples_.size()));
  }
  /***************************************************************************
      @brief Return the framerate of the last call to sync().
   */
  [[nodiscard]] float get_last_fps() const noexcept {
    if (samples_.empty()) return 0;
    if (samples_.back() == 0) return 0;
    return 1.0f / samples_.back();
  }
  /***************************************************************************
      @brief Return the lowest framerate recently sampled.
   */
  [[nodiscard]] float get_min_fps() const noexcept {
    if (samples_.empty()) return 0;
    const float sample = *std::max_element(samples_.begin(), samples_.end());
    if (sample == 0) return 0;
    return 1.0f / sample;
  }
  /***************************************************************************
      @brief Return the highest framerate recently sampled.
   */
  [[nodiscard]] float get_max_fps() const noexcept {
    if (samples_.empty()) return 0;
    const float sample = *std::min_element(samples_.begin(), samples_.end());
    if (sample == 0) return 0;
    return 1.0f / sample;
  }
  /***************************************************************************
      @brief Return the median framerate.  This is the framerate of the middle sample when all samples are sorted.
   */
  [[nodiscard]] float get_median_fps() const noexcept {
    if (samples_.empty()) return 0;
    std::vector<float> samples(samples_.begin(), samples_.end());
    std::sort(samples.begin(), samples.end());
    float median_sample = samples[samples.size() / 2];
    if (samples.size() % 2 == 0 && samples.size() > 2) {
      median_sample = (median_sample + samples[samples.size() / 2 + 1]) / 2.0f;
    }
    if (median_sample == 0) return 0;
    return 1.0f / median_sample;
  }

 private:
  static constexpr size_t MAX_SAMPLES_COUNT = 1024;  // Hard limit on the number of samples held.
  static constexpr double MAX_SAMPLES_TIME = 1.0;  // Hard limit on the total time of samples held.

  uint64_t last_time_;  // The last precise time sampled.
  std::deque<float> samples_;  // The most recent delta time samples in seconds.
};
}  // namespace tcod
#endif  // __cplusplus
#endif  // LIBTCOD_TIMER_H_
