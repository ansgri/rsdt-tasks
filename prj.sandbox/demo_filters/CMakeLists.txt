project(demo_filters)
find_package(OpenCV REQUIRED)
add_executable(demo_filters
  src/demo_filters.cpp
)

target_link_libraries(demo_filters
  ${OpenCV_LIBS}
)
