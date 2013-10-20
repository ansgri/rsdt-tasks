#pragma once
#include <cstdio>
#include <opencv2/opencv.hpp>
#include <boost/noncopyable.hpp>


namespace rsdt { namespace docproc {


class DebugImageWriter : private boost::noncopyable
{
public:
    DebugImageWriter(std::string const& prefix, bool enabled)
    : prefix_(prefix),
      enabled_(enabled),
      image_no_(0)
    { }

    bool enabled() const { return enabled_; }

    void write(std::string const& label, cv::Mat const& img)
    {
        if (!enabled_)
            return;

        char buf[16] = {0};
        sprintf(buf, "%02d", image_no_);
        ++image_no_;
        std::string const filename = prefix_ + buf + "_" + label + ".png";
        cv::imwrite(filename, img);
    }

private:
    std::string prefix_;
    bool enabled_;
    int  image_no_;
};


cv::Size size_for_wing(int wx, int wy);

cv::Mat morph_filter(cv::Mat const& src, int wx, int wy, int operation);

cv::Mat rotate_around_center(cv::Mat const& src, double angle);

}}
