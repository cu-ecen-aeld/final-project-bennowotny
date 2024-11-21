#include "networking/Socket.hpp"
#include <stop_token>

namespace networking {

Socket::Socket(PortNumber port, ConnectionCallback onConnection) : m_onConnection{std::move(onConnection)},
                                                                   m_handleNewConnections{
                                                                       [this](std::stop_token token) { handleNewConnections(token); }} {}

Socket::~Socket() noexcept {}

void Socket::handleNewConnections(std::stop_token stopToken) {}

} // namespace networking
