#pragma once

#include <cmath>
#include <thread>

/* thread for all */
template<typename Func, typename IndexT>
void nthread_execution(Func& f, IndexT& total, IndexT& nthread) {
  // if nthread == 1, don't even bother creating thread
  if (nthread == 1) {
    f(0, total);
    return;
  }

  // get chunk size and prepare threads
  const IndexT chunk_size = (total + nthread - 1) / nthread;
  std::vector<std::thread> tpool;
  tpool.reserve(nthread);

  for (int i{0}; i < (nthread - 1); i++) {
    tpool.emplace_back(std::thread{f, i * chunk_size, (i + 1) * chunk_size});
  }
  {
    // last one
    tpool.emplace_back(std::thread{f, (nthread - 1) * chunk_size, total});
  }

  for (auto& t : tpool) {
    t.join();
  }
}
