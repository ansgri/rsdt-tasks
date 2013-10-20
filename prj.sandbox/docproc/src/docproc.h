#pragma once

#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <opencv2/opencv.hpp>


namespace rsdt { namespace docproc {

struct Settings
{
    int bg_morph_wing;
    int fg_morph_wing;
    int fg_smooth_wing;
    int fg_min_val;
    double downscale_factor;
    double optangle_prescale_factor;
    int optangle_open_wing;
    double optangle_max_angle;
    double optangle_angle_step;

    Settings()
    : bg_morph_wing(10),
      fg_morph_wing(50),
      fg_smooth_wing(50),
      fg_min_val(70),
      downscale_factor(0.5),
      optangle_prescale_factor(0.5),
      optangle_open_wing(50),
      optangle_max_angle(10),
      optangle_angle_step(0.5)
    { }
};

class DebugImageWriter;

double find_optimal_angle(cv::Mat const& src, Settings const& settings, DebugImageWriter & w);

cv::Mat remove_background(cv::Mat const& grey, Settings const& settings, DebugImageWriter & w);

cv::Mat downscale(cv::Mat const& src, Settings const& settings, DebugImageWriter & w);

}}
