#include "networking/Connection.hpp"
#include "util/LockableWrapper.hpp"
#include <csignal>
#include <networking/Socket.hpp>
#include <ostream>
#include <rgb-color/RGBColor.hpp>

#include <csignal>
#include <fstream>
#include <iostream>
#include <stop_token>
#include <unistd.h>

int main(int argc, char **argv) {

  if (daemon(0, 0)) {
    std::cerr << "ERR: Could not daemonize" << std::endl;
  }

  sigset_t blockedSignals;
  sigemptyset(&blockedSignals);
  sigaddset(&blockedSignals, SIGINT);
  sigaddset(&blockedSignals, SIGTERM);
  pthread_sigmask(SIG_BLOCK, &blockedSignals, nullptr);

  static constexpr auto RGB_COLOR_ENDPOINT{"/dev/rgb-led"};
  LockableWrapper<std::ofstream> rgbOutput{std::ofstream{RGB_COLOR_ENDPOINT, std::ios::binary}};

  static constexpr networking::PortNumber PORT_NUMBER{18658};
  static constexpr std::size_t MAX_PENDING_CONNECTIONS{20};
  networking::Socket socket{
      PORT_NUMBER,
      MAX_PENDING_CONNECTIONS,
      [&](const std::stop_token & /* token */, networking::Connection &connection, std::promise<void> complete) {
        complete.set_value_at_thread_exit();
        const auto data{connection.getData(3)};
        if (data.size() != 3)
          return;
        const auto color{rgb::fromBytes(data)};
        (*rgbOutput.acquire()) << color << std::flush;
      }};

  int terminatingSignal{0};
  sigwait(&blockedSignals, &terminatingSignal);

  return 0;
}
