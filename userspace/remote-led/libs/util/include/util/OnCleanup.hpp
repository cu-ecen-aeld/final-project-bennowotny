#ifndef UTIL_ONCLEANUP_HPP
#define UTIL_ONCLEANUP_HPP

#include <concepts>
#include <utility>

template <typename Callable>
  requires std::invocable<Callable>
class OnCleanup {
public:
  explicit OnCleanup(Callable callable) : m_callable{std::move(callable)} {}

  ~OnCleanup() {
    if (m_canCleanup)
      m_callable();
  }

  void cancelCleanup() { m_canCleanup = false; }

private:
  Callable m_callable;
  bool m_canCleanup{true};
};

#endif
