#ifndef NETWORKING_SOCKET_HPP
#define NETWORKING_SOCKET_HPP

#include "networking/Connection.hpp"
#include <future>
#include <latch>
#include <thread>
#include <util/LockableWrapper.hpp>
#include <util/StrongType.hpp>
#include <vector>

namespace networking {

using PortNumber = StrongType<uint16_t, struct NetworkingPortNumber>;
using ConnectionCallback = std::function<void(const std::stop_token &, Connection &, std::promise<void>)>;

class Socket {
public:
  Socket(PortNumber port, std::size_t pendingConnections, ConnectionCallback onConnection);
  ~Socket() noexcept;
  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&) = delete;
  Socket &operator=(Socket &&) = delete;

private:
  void handleNewConnections(std::stop_token stopToken);

  // main thread and connection thread need to synchronize - 2 arrivals
  std::latch m_waitToListenForConnections{2};
  std::jthread m_handleNewConnections;
  ConnectionCallback m_onConnection;

  struct ConnectionThreadStatus {
    std::jthread connectionThread;
    std::future<ConnectionCallback::result_type> completionSignal;
  };

  LockableWrapper<std::vector<ConnectionThreadStatus>> m_connectionThreads{};

  int m_socketFd;
};

} // namespace networking

#endif
