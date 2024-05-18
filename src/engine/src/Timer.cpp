#include "Timer.h"
using namespace std::chrono;
Timer::Timer() : recorded_time(steady_clock::now()) {}

float Timer::Mark() {
  const auto last_recorded_time = recorded_time;
  recorded_time = steady_clock::now();
  const duration<float> passing_time = recorded_time - last_recorded_time;
  return passing_time.count();
}

float Timer::Peek() const {
  return duration<float>(steady_clock::now() - recorded_time).count();
}