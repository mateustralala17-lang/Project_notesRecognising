#include "AppController.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>


#ifdef _MSC_VER
#include <intrin.h>
#pragma optimize("", off)
template <typename T> void escapeMemory(T *p) {
  _ReadWriteBarrier();
  (void)p;
}
#pragma optimize("", on)
#endif

template <typename Func>
void runBenchmark(const std::string &label, int warmupIters, int measureIters,
                  Func &&fn) {
  for (int i = 0; i < warmupIters; ++i) {
    fn();
  }

  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < measureIters; ++i) {
    fn();
  }
  auto end = std::chrono::high_resolution_clock::now();

  double totalNs =
      std::chrono::duration<double, std::nano>(end - start).count();
  double avgNs = totalNs / static_cast<double>(measureIters);
  double avgUs = avgNs / 1000.0;
  double opsPerSec = 1e9 / avgNs;

  std::cout << "| " << std::left << std::setw(42) << label << " | "
            << std::right << std::setw(10) << std::fixed << std::setprecision(1)
            << avgNs << " ns"
            << " | " << std::setw(8) << std::fixed << std::setprecision(3)
            << avgUs << " us"
            << " | " << std::setw(12) << std::fixed << std::setprecision(0)
            << opsPerSec << " op/s |\n";
}

int main() {
  const int WARMUP = 2000;
  const int ITERS = 50000;

  std::cout << "+--------------------------------------------+---------------+-"
               "------------+-------------------+\n";
  std::cout << "| Benchmark Scenario                         |   Avg Time    | "
               "  Avg (us)  |    Throughput     |\n";
  std::cout << "+--------------------------------------------+---------------+-"
               "------------+-------------------+\n";

  runBenchmark("1. Single note press + release", WARMUP, ITERS, []() {
    AppController ctrl;
    ctrl.press(60);
    auto s1 = ctrl.getState();
    escapeMemory(&s1);
    ctrl.release(60);
    auto s2 = ctrl.getState();
    escapeMemory(&s2);
  });

  runBenchmark("2. Single chord press (Major triad)", WARMUP, ITERS, []() {
    AppController ctrl;
    ctrl.press(ChordCommand{60, 0, "major"});
    auto s = ctrl.getState();
    escapeMemory(&s);
  });

  runBenchmark("3. Full chromatic cluster (12 notes)", 200, 1000 / 5, []() {
    AppController ctrl;
    for (int pitch = 60; pitch < 72; ++pitch) {
      ctrl.press(pitch);
    }
    auto s = ctrl.getState();
    escapeMemory(&s);
  });

  std::cout << "+--------------------------------------------+---------------+-"
               "------------+-------------------+\n";
  return 0;
}// Останній тест виконується довше ніж інші, бо перераховується велика кількість комбінацій нот.