#include <cstdio>
#include <stdexcept>
#include <opencv2/opencv.hpp>


namespace rsdt { namespace docproc {

struct Settings
{
    int bg_morph_wing;

    Settings()
    : bg_morph_wing(10)
    { }
};


static cv::Mat enhance_image(cv::Mat const& src, Settings const& settings)
{
    throw std::runtime_error("enhance_image not implemented");
}


struct Args
{
    std::string src_image_path;
    std::string dst_image_path;
};


static void run(Args const& args)
{
    Settings settings; // default values are set in its ctor
    cv::Mat dst = enhance_image(cv::imread(args.src_image_path, CV_LOAD_IMAGE_COLOR), settings);
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
