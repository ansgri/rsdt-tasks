#include <cstdio>
#include <stdexcept>
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

struct Settings
{
    int bg_morph_wing;
    int fg_morph_wing;
    int fg_smooth_wing;
    int fg_min_val;
    double downscale_factor;

    Settings()
    : bg_morph_wing(10),
      fg_morph_wing(50),
      fg_smooth_wing(50),
      fg_min_val(70),
      downscale_factor(0.5)
    { }
};


static cv::Size size_for_wing(int w)
{
    return cv::Size(w * 2 + 1, w * 2 + 1);
}

static cv::Mat morph_filter(cv::Mat const& src, int wing, int operation)
{
    cv::Mat const strel = cv::getStructuringElement(cv::MORPH_RECT, size_for_wing(wing));
    cv::Mat dst;
    cv::morphologyEx(src, dst, operation, strel);
    return dst;   
}

static cv::Mat remove_background(cv::Mat const& grey, Settings const& settings, DebugImageWriter & w)
{
    cv::Mat background = morph_filter(grey, settings.bg_morph_wing, cv::MORPH_CLOSE);
    w.write("background", background);

    cv::Mat without_bg = background - grey;
    w.write("without_bg", without_bg);

    cv::Mat foreground = morph_filter(without_bg, settings.fg_morph_wing, cv::MORPH_DILATE);
    cv::GaussianBlur(foreground, foreground, size_for_wing(settings.fg_smooth_wing), 0);
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

static cv::Mat downscale(cv::Mat const& src, Settings const& settings, DebugImageWriter & w)
{
    double const factor = settings.downscale_factor;
    if (factor == 1.0)
        return src;

    int const morph_w = static_cast<int>(0.5 / factor);
    cv::Mat src_dilated = morph_filter(src, morph_w, cv::MORPH_DILATE);
    cv::Mat src_eroded = morph_filter(src, morph_w, cv::MORPH_ERODE);

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

static cv::Mat enhance_image(cv::Mat const& src, Settings const& settings, DebugImageWriter & w)
{
    cv::Mat grey;
    cv::cvtColor(src, grey, CV_RGB2GRAY);

    cv::Mat enhanced = remove_background(grey, settings, w);
    w.write("enhanced", enhanced);

    return downscale(enhanced, settings, w);
}


struct Args
{
    std::string src_image_path;
    std::string dst_image_path;
};


static void run(Args const& args)
{
    DebugImageWriter w("docproc", true);
    Settings settings; // default values are set in its ctor
    cv::Mat dst = enhance_image(cv::imread(args.src_image_path, CV_LOAD_IMAGE_COLOR), settings, w);
    cv::imwrite(args.dst_image_path, dst);
}

}}


int main(int argc, char const** argv)
{
    try
    {
        if (argc != 3)
            throw std::runtime_error("Bad command line; usage: ./docproc src-image dst-image");
        rsdt::docproc::Args args;
        args.src_image_path = argv[1];       
        args.dst_image_path = argv[2];

        rsdt::docproc::run(args);
        return 0;
    }
    catch (std::exception const& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
