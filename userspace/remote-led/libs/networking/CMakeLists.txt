add_library(networking)
target_sources(networking PRIVATE src/Socket.cpp src/Connection.cpp)
target_include_directories(networking PUBLIC include)
target_link_libraries(networking PUBLIC util)
