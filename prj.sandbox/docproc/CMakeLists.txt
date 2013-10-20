project(task3-docproc)
find_package(OpenCV REQUIRED)
add_executable(docproc
  src/utils.h
  src/utils.cpp
  src/docproc.h
  src/docproc.cpp
  src/main.cpp
)

target_link_libraries(docproc
  ${OpenCV_LIBS}
)
