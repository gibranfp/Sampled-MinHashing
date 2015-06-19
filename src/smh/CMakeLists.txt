set (MHCLUS_SRCS mhclus array_lists listdb ifindex topics minhash sampledmh mhlink)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
include_directories( ${PROJECT_SOURCE_DIR}/include )
<<<<<<< HEAD:src/smh/CMakeLists.txt
add_library(array_lists array_lists)
add_library(listdb listdb)
add_library(weights weights)
=======
add_library(array_lists SHARED array_lists)
add_library(listdb SHARED listdb)
>>>>>>> e936c974f467cd89bbb44dbc6dac5d96a35e9372:src/mhclus/CMakeLists.txt
add_library(ifindex ifindex)
add_library(textio textio)
add_library(minhash minhash)
add_library(mhsearch mhsearch)
add_library(sampledmh sampledmh)
add_library(mhlink mhlink)
add_executable( mhclus mhclus )
target_link_libraries( mhclus mhlink sampledmh minhash ifindex weights listdb array_lists m)
