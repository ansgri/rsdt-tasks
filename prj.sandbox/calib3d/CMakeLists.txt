project(calib3d)
find_package(OpenCV REQUIRED)
add_executable(calib3d
  src/calib3d.cpp
)

target_link_libraries(calib3d
  ${OpenCV_LIBS}
)
