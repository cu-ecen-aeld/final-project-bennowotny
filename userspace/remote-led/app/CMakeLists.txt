add_executable(remote-led)
target_sources(remote-led PRIVATE main.cpp)

install(TARGETS remote-led RUNTIME)
