#pragma once

#include <algorithm>
#include <thread>

namespace napf {

/* thread for all */
template<typename Func, typename IndexT>
void nthread_execution(Func& f, IndexT& total, IndexT& nthread) {
  // if nthread == 1, don't even bother creating thread
  if (nthread == 1 || nthread == 0) {
    f(0, total);
    return;
  }

  int n_usable_threads{nthread};

  // negative input looks for hardware_concurrency
  if (nthread < 0) {
    n_usable_threads = std::max(std::thread::hardware_concurrency(), 1u);
  }

  // thread shouldn't exceed total
  n_usable_threads = std::min(total, n_usable_threads);

  // get chunk size and prepare threads
  const IndexT chunk_size = (total + n_usable_threads - 1) / n_usable_threads;
  std::vector<std::thread> tpool;
  tpool.reserve(n_usable_threads);

  for (int i{0}; i < (n_usable_threads - 1); i++) {
    tpool.emplace_back(std::thread{f, i * chunk_size, (i + 1) * chunk_size});
  }
  {
    // last one
    tpool.emplace_back(
        std::thread{f, (n_usable_threads - 1) * chunk_size, total});
  }

  for (auto& t : tpool) {
    t.join();
  }
}

} // namespace napf
