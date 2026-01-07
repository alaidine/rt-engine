add_library(networking Networking.cpp Networking.h)

target_link_libraries(networking PRIVATE asio)
