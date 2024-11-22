#ifndef UTIL_STRONGTYPE_HPP
#define UTIL_STRONGTYPE_HPP

#include <utility>

template <typename T, typename Token>
class StrongType {
public:
  explicit constexpr StrongType(T val) : m_val{std::move(val)} {}

  const T &get() const noexcept { return m_val; }

  T &get() noexcept { return m_val; }

private:
  T m_val;
};

#endif
