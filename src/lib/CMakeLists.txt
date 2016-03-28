include_directories(
)

set(satip_srcs
  channel.cpp 
  rtsp.cpp
  rtp.cpp
  socket.cpp
  confparser.cpp
  device/satip.cpp
)

if(WINDOWS)
  list(APPEND ${satip_srcs} windows.h)
endif()

add_library(satip SHARED ${satip_srcs})
add_library(satip-static STATIC ${satip_srcs})

target_link_libraries(satip pthread)
