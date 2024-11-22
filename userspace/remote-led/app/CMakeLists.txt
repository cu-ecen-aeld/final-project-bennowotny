add_executable(remote-led)
target_sources(remote-led PRIVATE main.cpp)
target_link_libraries(remote-led PRIVATE rgb-color networking)

install(TARGETS remote-led RUNTIME)
