#include "networking/Connection.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

namespace networking {
Connection::Connection(int fd) : m_fd{fd} {}

Connection::~Connection() noexcept { close(m_fd); }

std::vector<std::byte> Connection::getData(std::size_t bytes_requested) {
  std::vector<std::byte> data;
  data.resize(bytes_requested);
  auto recvdDataSize{recv(m_fd, data.data(), bytes_requested, 0)};
  if (recvdDataSize < 0) {
    using namespace std::string_literals;
    throw std::runtime_error{"Could not receive data from connection "s + std::to_string(m_fd)};
  }
  data.erase(data.begin() + recvdDataSize, data.end());
  data.shrink_to_fit();
  return data;
}
} // namespace networking
