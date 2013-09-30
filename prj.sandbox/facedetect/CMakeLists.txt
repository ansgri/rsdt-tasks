project(task2-facedetect)
find_package(OpenCV REQUIRED)
add_executable(facedetect
  src/facedetect.cpp
)

target_link_libraries(facedetect
  ${OpenCV_LIBS}
)
