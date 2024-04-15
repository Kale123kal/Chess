file(GLOB_RECURSE WalnutApp_SRC LIST_DIRECTORIES false src/*.cpp)

add_executable(Chess ${WalnutApp_SRC})
target_include_directories(Chess PRIVATE src)
target_link_libraries(Chess PRIVATE Walnut)

include_directories("../vendor/stb_image")

# setup internal project compile definition
if(WIN32)
    target_compile_definitions(Walnut PRIVATE WL_PLATFORM_WINDOWS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(Chess PRIVATE WL_DEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  target_compile_definitions(Chess PRIVATE WL_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_definitions(Chess PRIVATE WL_DIST)
endif()

configure_file(res/pieces.png ${CMAKE_SOURCE_DIR}/res/pieces.png)

install(TARGETS Chess DESTINATION bin)
