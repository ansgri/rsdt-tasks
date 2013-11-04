#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "docproc.h"
#include "utils.h"


namespace rsdt { namespace docproc {


double find_optimal_angle(cv::Mat const& src, Settings const& settings, DebugImageWriter & w)
{
    cv::Mat src_scaled;
    cv::resize(src, 
               src_scaled, 
               cv::Size(), 
               settings.optangle_prescale_factor, 
               settings.optangle_prescale_factor, 
               cv::INTER_AREA);
    cv::Mat morph_grad = morph_filter(src_scaled, 1, 1, cv::MORPH_DILATE)
                       - morph_filter(src_scaled, 1, 1, cv::MORPH_ERODE);
    w.write("optangle_morph_grad", morph_grad);

    double best_angle = 0.0;
    double best_score = 0;
    for (double angle = -settings.optangle_max_angle; 
         angle <= settings.optangle_max_angle; 
         angle += settings.optangle_angle_step)
    {
        cv::Mat morph_grad_rot = rotate_around_center(morph_grad, angle);
        w.write("optangle_morph_grad_rot", morph_grad);
        
        cv::Mat vbars = morph_filter(morph_grad_rot, 0, settings.optangle_open_wing, cv::MORPH_OPEN);
        w.write("optangle_vbars", vbars);
        cv::Mat hbars = morph_filter(morph_grad_rot, settings.optangle_open_wing, 0, cv::MORPH_OPEN);
        // w.write("optangle_hbars", hbars);
        double const score = std::max(cv::mean(vbars)[0], cv::mean(hbars)[0]);
        if (score > best_score)
        {
            best_score = score;
            best_angle = angle;
        }
    }

    return best_angle;
}


cv::Mat remove_background(cv::Mat const& grey, Settings const& settings, DebugImageWriter & w)
{
    cv::Mat background = morph_filter(grey, settings.bg_morph_wing, settings.bg_morph_wing, 
                                      cv::MORPH_CLOSE);
    w.write("background", background);

    cv::Mat without_bg = background - grey;
    w.write("without_bg", without_bg);

    cv::Mat foreground = morph_filter(without_bg, settings.fg_morph_wing, settings.fg_morph_wing, 
                                      cv::MORPH_DILATE);
    cv::GaussianBlur(foreground, foreground, size_for_wing(settings.fg_smooth_wing, settings.fg_smooth_wing), 0);
    foreground = cv::max(foreground, settings.fg_min_val);
    w.write("foreground", foreground);

    cv::Mat foreground_f;
    foreground.convertTo(foreground_f, CV_32F, 1.0 / 255);
    cv::Mat without_bg_f;
    without_bg.convertTo(without_bg_f, CV_32F);
    
    cv::Mat dst_f = without_bg_f / foreground_f;
    cv::Mat dst;
    dst_f.convertTo(dst, CV_8U);

    return cv::Scalar(255) - dst;
}

cv::Mat downscale(cv::Mat const& src, Settings const& settings, DebugImageWriter & w)
{
    double const factor = settings.downscale_factor;
    if (factor == 1.0)
        return src;

    int const morph_w = static_cast<int>(0.5 / factor);
    cv::Mat src_dilated = morph_filter(src, morph_w, morph_w, cv::MORPH_DILATE);
    cv::Mat src_eroded = morph_filter(src, morph_w, morph_w, cv::MORPH_ERODE);

    cv::Mat ds;
    cv::resize(src, ds, cv::Size(), factor, factor, cv::INTER_AREA);
    w.write("ds", ds);

    cv::Mat ds_dilated;
    cv::resize(src_dilated, ds_dilated, cv::Size(), factor, factor, cv::INTER_AREA);
    w.write("ds_dilated", ds_dilated);

    cv::Mat ds_eroded;
    cv::resize(src_eroded, ds_eroded, cv::Size(), factor, factor, cv::INTER_AREA);
    w.write("ds_eroded", ds_eroded);

    cv::Mat dist_eroded = ds - ds_eroded;
    cv::Mat dist_dilated = ds_dilated - ds;
    ds_eroded.copyTo(ds, dist_eroded > dist_dilated);
    ds_dilated.copyTo(ds, dist_dilated < dist_eroded);

    return ds;
}


}}
