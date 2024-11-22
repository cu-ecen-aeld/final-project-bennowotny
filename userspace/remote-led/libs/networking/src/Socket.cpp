#include "networking/Socket.hpp"
#include "networking/Connection.hpp"
#include "util/OnCleanup.hpp"
#include <algorithm>
#include <future>
#include <netdb.h>
#include <poll.h>
#include <stdexcept>
#include <stop_token>
#include <string>
#include <strings.h>
#include <sys/socket.h>

using namespace std::chrono_literals;

namespace networking {

Socket::Socket(PortNumber port, std::size_t pendingConnections, ConnectionCallback onConnection) : m_onConnection{std::move(onConnection)},
                                                                                                   m_handleNewConnections{
                                                                                                       [this](std::stop_token token) { handleNewConnections(token); }} {

  const auto socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFd == -1) {
    throw std::runtime_error{"Could not create socket"};
  }
  OnCleanup cleanupSocket{[&socketFd] { close(socketFd); }};

  // allow socket reuse
  const int optionValue{1};
  setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));

  {
    addrinfo hints;
    // Scoped in to destroy when finished using this
    addrinfo *addrResult;
    OnCleanup cleanupAddrInfo{[&addrResult] { freeaddrinfo(addrResult); }};

    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(nullptr, std::to_string(port.get()).c_str(), &hints, &addrResult) != 0) {
      throw std::runtime_error{"Could not get address to bind to"};
    }

    if (bind(socketFd, addrResult->ai_addr, sizeof(sockaddr)) != 0) {
      throw std::runtime_error{"Could not bind to address"};
    }
  }

  if (listen(socketFd, pendingConnections)) {
    throw std::runtime_error{"Could not start listening for connections"};
  }

  m_socketFd = socketFd;
  // we succeeded in setting up the socket, don't close it here
  cleanupSocket.cancelCleanup();

  // start a thread to process incoming connections
  m_waitToListenForConnections.arrive_and_wait();
}

Socket::~Socket() noexcept {
  close(m_socketFd);
}

void Socket::handleNewConnections(std::stop_token stopToken) {
  m_waitToListenForConnections.arrive_and_wait();

  while (true) {
    // wait for a connection to be available before accept()
    // otherwise, we can't terminate this thread, since signals need to be blocked
    while (!stopToken.stop_requested()) {
      struct pollfd socketPoll;
      socketPoll.fd = m_socketFd;
      socketPoll.events = POLLIN;
      const auto pollStatus{poll(&socketPoll, 1, 1000)};
      if (pollStatus < 0) {
        throw std::runtime_error{"Could not wait for a connection"};
      } else if (pollStatus > 0)
        break; // we have a connection
      // ignore pollStatus == 0, that means there was a timeout and we should check again
    }

    struct sockaddr connectedAddr;
    socklen_t addrLen{sizeof(connectedAddr)};
    const auto connectionFd{accept(m_socketFd, &connectedAddr, &addrLen)};

    if (connectionFd == -1)
      break;

    std::promise<void> threadCompletionPromise;
    auto completionFuture{threadCompletionPromise.get_future()};
    m_connectionThreads.acquire()->emplace_back(ConnectionThreadStatus{.connectionThread{std::jthread{[&](auto token) {
                                                                         Connection connection{connectionFd};
                                                                         m_onConnection(token, connection, std::move(threadCompletionPromise));
                                                                       }}},
                                                                       .completionSignal{std::move(completionFuture)}});

    // sweep for complete threads
    {
      auto threads{m_connectionThreads.acquire()};
      std::sort(threads->begin(), threads->end(), [](const ConnectionThreadStatus &lhs, const ConnectionThreadStatus &rhs) {
        return lhs.completionSignal.wait_for(0s) == std::future_status::ready && rhs.completionSignal.wait_for(0s) == std::future_status::timeout;
      });
      // running threads are now first in the list
      // erase from the first complete thread to the end
      const auto firstComplete{std::find_if(threads->begin(), threads->end(),
                                            [](const auto &x) {
                                              return x.completionSignal.wait_for(0s) == std::future_status::timeout;
                                            })};
      threads->erase(firstComplete, threads->end());
    }
  }
}

} // namespace networking
