#ifndef NETWORKING_CONNECTION_HPP
#define NETWORKING_CONNECTION_HPP

#include <cstddef>
#include <vector>

namespace networking {
class Connection {
public:
  explicit Connection(int fd);
  ~Connection() noexcept;
  Connection(const Connection &) = delete;
  Connection &operator=(const Connection &) = delete;
  Connection(Connection &&) = delete;
  Connection &operator=(Connection &&) = delete;

  std::vector<std::byte> getData(std::size_t bytes_requested);

private:
  int m_fd;
};
} // namespace networking

#endif
