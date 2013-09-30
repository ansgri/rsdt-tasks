# --- architecture ---
if(WIN32)
  set(RSDT_TASKS_OPERATING_SYSTEM "win")
elseif(UNIX)
  set(RSDT_TASKS_OPERATING_SYSTEM "linux")
else()
  message(FATAL_ERROR "Unknown operating system")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL "4")
  set(RSDT_TASKS_CPU_BITNESS "32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL "8")
  set(RSDT_TASKS_CPU_BITNESS "64")
else()
  message(FATAL_ERROR "Unsupported CPU bitness")
endif()

set(RSDT_TASKS_ARCH ${RSDT_TASKS_OPERATING_SYSTEM}${RSDT_TASKS_CPU_BITNESS})
message(STATUS "Target architecture: ${RSDT_TASKS_ARCH}")

# --- directories ---
set(RSDT_TASKS_ROOT ${rsdt_tasks_SOURCE_DIR})
set(RSDT_TASKS_INCLUDE_ROOT ${RSDT_TASKS_ROOT}/include)
set(RSDT_TASKS_LIB_ROOT ${RSDT_TASKS_ROOT}/lib/${RSDT_TASKS_ARCH})
if(WIN32 AND NOT MSVC_IDE)
  set(RSDT_TASKS_LIB_ROOT ${RSDT_TASKS_LIB_ROOT} ${RSDT_TASKS_LIB_ROOT}/${CMAKE_BUILD_TYPE})
endif()



# --- lists of libraries for linking ---
if(WIN32)
  set(RSDT_TASKS_OPENCV_LIBS 
    optimized opencv_core242
    optimized opencv_highgui242
    optimized opencv_imgproc242
    optimized opencv_video242
    debug opencv_core242d
    debug opencv_highgui242d
    debug opencv_imgproc242d
    debug opencv_video242d
  )
elseif(UNIX)
  set(RSDT_TASKS_OPENCV_LIBS 
    opencv_core
    opencv_highgui
    opencv_imgproc
    opencv_video
  )
endif()

if(WIN32)
  set(RSDT_TASKS_JRTP_LIBS 
    optimized jrtplib
    debug jrtplib_d
  )
elseif(UNIX)
  set(RSDT_TASKS_JRTP_LIBS
    jrtp
  )
endif()

set(RSDT_TASKS_FFMPEG_LIBS
  avcodec
  avcore
  avdevice
  avfilter
  avformat
  avutil
  swscale
)
