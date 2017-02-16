# set common optimization flags

find_package(CUDA 6.5)

if (CUDA_FOUND)
  set(USE_CUDA 1 CACHE BOOL "Use CUDA to speed up some functions")
else ()
  set(USE_CUDA 0 CACHE BOOL "Use CUDA to speed up some functions" FORCE)
endif ()

EXECUTE_PROCESS(COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE)
message(STATUS "Detected architecture ${ARCHITECTURE}")

if (${ARCHITECTURE} STREQUAL "armv7l") # Preselect settings for Odroid or Tegra K1
    set(USE_SSE2 0 CACHE BOOL "Compile for SSE2 (x86) using -msse2")
    set(USE_SSE4.2 0 CACHE BOOL "Compile for SSE4.2 (x86) using -mpopcnt -msse4.2")
    set(USE_AVX 0 CACHE BOOL "Compile for AVX (x86) using -mavx")
    set(USE_AVX2 0 CACHE BOOL "Compile for AVX2 (x86) using -mavx2")
    set(USE_ARMV7_CA15 1 CACHE BOOL "Compile for ARM V7 Cortex-A15 using -mcpu=cortex-a15 -mfpu=neon")
    set(USE_ARMV8_AARCH64 0 CACHE BOOL "Compile for ARM V8 64 bit using -mcpu=armv8-a -mtune=cortex-a57")
elseif (${ARCHITECTURE} STREQUAL "aarch64") # Preselect settings for Tegra X1
    set(USE_SSE2 0 CACHE BOOL "Compile for SSE2 (x86) using -msse2")
    set(USE_SSE4.2 0 CACHE BOOL "Compile for SSE4.2 (x86) using -mpopcnt -msse4.2")
    set(USE_AVX 0 CACHE BOOL "Compile for AVX (x86) using -mavx")
    set(USE_AVX2 0 CACHE BOOL "Compile for AVX2 (x86) using -mavx2")
    set(USE_ARMV7_CA15 0 CACHE BOOL "Compile for ARM V7 Cortex-A15 using -mcpu=cortex-a15 -mfpu=neon")
    set(USE_ARMV8_AARCH64 1 CACHE BOOL "Compile for ARM V8 64 bit using -mcpu=armv8-a -mtune=cortex-a57")
else () # Preselect settings for intel architecture
    set(USE_SSE2 1 CACHE BOOL "Compile for SSE2 (x86) using -msse2")
    set(USE_SSE4.2 1 CACHE BOOL "Compile for SSE4.2 (x86) using -mpopcnt -msse4.2")
    set(USE_AVX 1 CACHE BOOL "Compile for AVX (x86) using -mavx")
    set(USE_AVX2 1 CACHE BOOL "Compile for AVX2 (x86) using -mavx2")
    set(USE_ARMV7_CA15 0 CACHE BOOL "Compile for ARM V7 Cortex-A15 using -mcpu=cortex-a15 -mfpu=neon")
    set(USE_ARMV8_AARCH64 0 CACHE BOOL "Compile for ARM V8 64 bit using -mcpu=armv8-a -mtune=cortex-a57")
endif ()

set(UNROLL_LOOPS 1 CACHE BOOL "Compile using -funroll-loops")

if (USE_CUDA)
    include_directories(${CUDA_INCLUDE_DIRS})

    if (${ARCHITECTURE} STREQUAL "armv7l")
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -gencode=arch=compute_32,code=sm_32 -O3)
    elseif (${ARCHITECTURE} STREQUAL "aarch64")
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -gencode=arch=compute_53,code=sm_53 -O3)
    else ()
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -gencode=arch=compute_30,code=sm_30 -gencode=arch=compute_35,code=sm_35 -gencode=arch=compute_50,code=sm_50 -gencode=arch=compute_52,code=sm_52 -gencode=arch=compute_52,code=compute_52 -O3)
    endif ()

    add_definitions(-DINCLUDE_CUDA)

    get_filename_component(CUDA_LIB_FOLDER ${CUDA_LIBRARIES} DIRECTORY)
    set(CUDA_STATIC_LIBRARIES "${CUDA_LIB_FOLDER}/libcudart_static.a" -ldl -lrt)
endif ()

if (USE_SSE2)
    add_definitions(-msse2)
endif ()

if (USE_SSE4.2)
    add_definitions(-mpopcnt)
    add_definitions(-msse4.2)
endif ()

if (USE_AVX)
    add_definitions(-mavx)
endif ()

if (USE_AVX2)
    add_definitions(-mavx2)
endif ()

if (USE_ARMV7_CA15)
    add_definitions(-mcpu=cortex-a15 -mfpu=neon)
endif ()

if (USE_ARMV8_AARCH64)
#    add_definitions(-march=armv8-a -mtune=cortex-a57 -mfpu=neon)
    add_definitions(-march=armv8-a -mfpu=neon)
endif ()

if (UNROLL_LOOPS)
    add_definitions(-funroll-loops)
endif ()
