set(CMAKE_CXX_STANDARD 11)
file(GLOB_RECURSE UTIL_SRC CONFIGURE_DEPENDS src/*.cc include/*.h)
add_library(util ${UTIL_SRC})
target_include_directories(util PUBLIC include PRIVATE src)