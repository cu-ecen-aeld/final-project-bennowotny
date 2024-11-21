#ifndef NETWORKING_SOCKET_HPP
#define NETWORKING_SOCKET_HPP

#include "networking/Connection.hpp"
#include <future>
#include <thread>
#include <util/StrongType.hpp>
#include <vector>

namespace networking {

using PortNumber = StrongType<int, struct NetworkingPortNumber>;
using ConnectionCallback = std::function<void(const std::stop_token &, Connection)>;

class Socket {
public:
  Socket(PortNumber port, ConnectionCallback onConnection);
  ~Socket() noexcept;
  Socket(const Socket &) = delete;
  Socket &operator=(const Socket &) = delete;
  Socket(Socket &&) = delete;
  Socket &operator=(Socket &&) = delete;

private:
  void handleNewConnections(std::stop_token stopToken);

  std::jthread m_handleNewConnections;
  ConnectionCallback m_onConnection;

  struct ConnectionThreadStatus {
    std::jthread connectionThread;
    std::future<ConnectionCallback::result_type> completionSignal;
  };

  std::vector<ConnectionThreadStatus> m_connectionThreadCompletion{};
};

} // namespace networking

#endif
