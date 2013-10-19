project(task3-docproc)
find_package(OpenCV REQUIRED)
add_executable(docproc
  src/docproc.cpp
)

target_link_libraries(docproc
  ${OpenCV_LIBS}
)
