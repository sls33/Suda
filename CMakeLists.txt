cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_COMPILER /usr/local/gcc/bin/g++)
set(CMAKE_C_COMPILER /usr/local/gcc/bin/gcc)
project(mpc_project)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -O3 -Wfatal-errors -pthread -Wno-ignored-attributes -Wno-int-in-bool-context -Wno-sign-compare -Wno-catch-value -fopenmp")

Set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")
Set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

IF(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
ENDIF()

find_library(NTL_LIB ntl src/third_party/ntl/src/.libs NO_DEFAULT_PATH)
if(NOT NTL_LIB)
  message(FATAL_ERROR "ntl library not found.  Rerun cmake with -DCMAKE_PREFIX_PATH=\"<path to lib1>;<path to lib2>\"")
endif()



message(STATUS "CMAKE_BUILD_TYPE " ${CMAKE_BUILD_TYPE})

# add_subdirectory(pybind11)
add_subdirectory(src/third_party/pailliercryptolib)
include_directories(src/third_party/ntl/include)
include_directories(src/third_party/SEAL/seal_install/include/SEAL-4.1)
include_directories(src/third_party/volepsi/out/install/linux/include)
# include_directories(${CMAKE_SOURCE_DIR}/../volepsi/out/install/linux/include)


# find_library(SODIUM_LIB sodium ${CMAKE_SOURCE_DIR}/../volepsi/out/install/linux/lib NO_DEFAULT_PATH)
find_library(SODIUM_LIB sodium src/third_party/libsodium/src/libsodium/.libs NO_DEFAULT_PATH)

find_library(SEAL_LIB seal-4.1 src/third_party/SEAL/build/lib NO_DEFAULT_PATH)
if(NOT SEAL_LIB)
  message(FATAL_ERROR "seal library not found.  Rerun cmake with -DCMAKE_PREFIX_PATH=\"<path to lib1>;<path to lib2>\"")
endif()

find_library(COPROTO_LIB coproto src/third_party/volepsi/out/install/linux/lib NO_DEFAULT_PATH)
find_library(MACORO_LIB macoro src/third_party/volepsi/out/install/linux/lib NO_DEFAULT_PATH)
# find_library(COPROTO_LIB coproto ${CMAKE_SOURCE_DIR}/../volepsi/out/install/linux/lib NO_DEFAULT_PATH)
# find_library(MACORO_LIB macoro ${CMAKE_SOURCE_DIR}/../volepsi/out/install/linux/lib NO_DEFAULT_PATH)

add_executable(ntl_test src/ntl_test.cpp)
target_link_libraries(ntl_test PUBLIC ${NTL_LIB})


add_executable(poly_test src/poly_test.cpp)
target_link_libraries(poly_test PUBLIC ${NTL_LIB} ${SEAL_LIB}  IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)

add_executable(batch_pir_basic_test src/batch_pir_basic_test.cpp)
target_link_libraries(batch_pir_basic_test PUBLIC ${NTL_LIB} ${SEAL_LIB}  IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)

add_executable(batch_pir_protocol_test src/batch_pir_protocol_test.cpp)
target_link_libraries(batch_pir_protocol_test PUBLIC ${NTL_LIB} ${SEAL_LIB}  IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)

add_executable(batch_pir_to_share_test src/batch_pir_to_share_test.cpp)
target_link_libraries(batch_pir_to_share_test PUBLIC ${NTL_LIB} ${SEAL_LIB}  IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)


add_executable(seal_test src/seal_test.cpp)
target_link_libraries(seal_test PUBLIC ${SEAL_LIB})


add_executable(ec_psi_test src/ec_psi_test.cpp)
target_link_libraries(ec_psi_test PUBLIC ${SODIUM_LIB} ${SEAL_LIB} IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)



add_executable(psi_to_share_test src/psi_to_share_test.cpp)
target_link_libraries(psi_to_share_test PUBLIC ${SODIUM_LIB} ${NTL_LIB} ${SEAL_LIB} ${COPROTO_LIB} ${MACORO_LIB} IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)



add_executable(psi_to_share_paillier_test src/psi_to_share_paillier_test.cpp)
target_link_libraries(psi_to_share_paillier_test PUBLIC ${SODIUM_LIB} ${NTL_LIB} ${SEAL_LIB} ${COPROTO_LIB} ${MACORO_LIB} IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)


add_executable(netio_test src/netio_test.cpp)
target_link_libraries(netio_test PUBLIC ${COPROTO_LIB} ${MACORO_LIB} IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)

add_executable(read_bin_test src/read_bin_test.cpp)

add_executable(mem_usage_test src/mem_usage_test.cpp)

add_executable(pailier_test src/pailier_test.cpp)
target_link_libraries(pailier_test IPCL::ipcl IPPCP::ippcp IPPCP::crypto_mb)