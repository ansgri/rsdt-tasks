#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "utils.h"
#include "docproc.h"


namespace rsdt { namespace docproc {


static cv::Mat enhance_image(cv::Mat const& src, Settings const& settings, DebugImageWriter & w)
{
    cv::Mat grey;
    cv::cvtColor(src, grey, CV_RGB2GRAY);

    cv::Mat enhanced = remove_background(grey, settings, w);
    w.write("enhanced", enhanced);

    double const angle = find_optimal_angle(enhanced, settings, w);
    printf("Best angle: %.2f\n", angle);
    cv::Mat rotated = rotate_around_center(enhanced, angle);

    cv::Mat downscaled = downscale(rotated, settings, w);
    return downscaled;
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
