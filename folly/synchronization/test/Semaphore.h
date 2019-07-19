/*
 * Copyright 2019-present Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <boost/intrusive/list.hpp>

#include <folly/ScopeGuard.h>
#include <folly/lang/Exception.h>

namespace folly {
namespace test {

enum class SemaphoreWakePolicy { Fifo, Lifo };

//  Semaphore
//
//  A basic portable semaphore, primarily intended for testing scenarios. Likely
//  to be much less performant than better-optimized semaphore implementations.
//
//  In the interest of portability, uses only synchronization mechanisms shipped
//  with all implementations of C++: std::mutex and std::condition_variable.
template <SemaphoreWakePolicy WakePolicy>
class Semaphore {
 public:
  Semaphore() : value_(0) {}

  explicit Semaphore(std::size_t value) : value_(value) {}

  bool try_wait() {
    std::unique_lock<std::mutex> lock{mutex_};
    if (value_) {
      --value_;
      return true;
    } else {
      return false;
    }
  }

  template <typename PreWait, typename PostWait>
  void wait(PreWait pre_wait, PostWait post_wait) {
    std::unique_lock<std::mutex> lock{mutex_};
    pre_wait();
    if (value_) {
      --value_;
      post_wait();
    } else {
      Waiter w;
      waiters_.push_back(w);
      w.wake_waiter.wait(lock);
      auto wake_poster_guard = makeGuard([&] { w.wake_poster->post(); });
      post_wait();
    }
  }

  void wait() {
    wait([] {}, [] {});
  }

  template <typename PrePost>
  void post(PrePost pre_post) {
    std::unique_lock<std::mutex> lock{mutex_};
    if (value_ == -size_t(1)) {
      throw_exception<std::logic_error>("overflow");
    }
    pre_post();
    if (waiters_.empty()) {
      ++value_;
    } else {
      auto& w = pull();
      waiters_.erase(waiters_.iterator_to(w));
      Event wake_poster;
      w.wake_poster = &wake_poster;
      w.wake_waiter.post();
      wake_poster.wait(lock);
    }
  }

  void post() {
    post([] {});
  }

 private:
  class Event {
   public:
    void post() {
      signaled = true;
      cv.notify_one();
    }
    void wait(std::unique_lock<std::mutex>& lock) {
      cv.wait(lock, [&] { return signaled; });
    }

   private:
    bool signaled = false;
    std::condition_variable cv;
  };

  struct Waiter : boost::intrusive::list_base_hook<> {
    Event wake_waiter;
    Event* wake_poster;
  };
  using WaiterList = boost::intrusive::list<Waiter>;

  Waiter& pull() {
    switch (WakePolicy) {
      case SemaphoreWakePolicy::Fifo:
        return waiters_.front();
      case SemaphoreWakePolicy::Lifo:
        return waiters_.back();
    }
    terminate_with<std::invalid_argument>("wake-policy");
  }

  std::size_t value_;
  std::mutex mutex_;
  WaiterList waiters_;
};

using FifoSemaphore = Semaphore<SemaphoreWakePolicy::Fifo>;
using LifoSemaphore = Semaphore<SemaphoreWakePolicy::Lifo>;

} // namespace test
} // namespace folly
