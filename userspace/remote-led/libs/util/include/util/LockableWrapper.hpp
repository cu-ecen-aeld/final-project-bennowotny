#ifndef UTIL_LOCKABLEWRAPPER_HPP
#define UTIL_LOCKABLEWRAPPER_HPP

#include <mutex>

template <typename T>
class Locked {
public:
  Locked(T *m_data, std::mutex &mutex) : m_data{m_data}, m_guard{mutex} {}

  T &operator*() { return *m_data; }

  const T &operator*() const { return *m_data; }

  T *operator->() { return m_data; }

  const T *operator->() const { return m_data; }

private:
  T *m_data;
  std::lock_guard<std::mutex> m_guard;
};

template <typename T>
class LockableWrapper {
public:
  explicit LockableWrapper(T data) : m_data{std::move(data)} {}

  ~LockableWrapper() noexcept {
    // ensure object isn't Locked on destruction
    std::lock_guard guard{m_mutex};
  }

  Locked<T> acquire() {
    return Locked<T>{&m_data, m_mutex};
  }

private:
  T m_data;
  std::mutex m_mutex;
};

#endif
