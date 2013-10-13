#include <cstdio>
#include <stdexcept>
#include <opencv2/opencv.hpp>


using cv::imwrite;
using cv::Mat;
using cv::Size;


static int const BLUR_APERTURE = 15;
static double const BLUR_BILATERAL_SIGMA = 100;


static void save_morph_images(Mat const& src, int strel_shape, std::string const& strel_name)
{
    Mat const strel = cv::getStructuringElement(strel_shape, Size(BLUR_APERTURE, BLUR_APERTURE));
    Mat dst;
    
    cv::dilate(src, dst, strel);
    imwrite("morph_dilate_" + strel_name + ".png", dst);

    cv::erode(src, dst, strel);
    imwrite("morph_erode_" + strel_name + ".png", dst);

    cv::morphologyEx(src, dst, cv::MORPH_OPEN, strel);
    imwrite("morph_open_" + strel_name + ".png", dst);

    cv::morphologyEx(src, dst, cv::MORPH_CLOSE, strel);
    imwrite("morph_close_" + strel_name + ".png", dst);

    cv::morphologyEx(src, dst, cv::MORPH_GRADIENT, strel);
    imwrite("morph_grad_" + strel_name + ".png", dst);

    cv::morphologyEx(src, dst, cv::MORPH_TOPHAT, strel);
    imwrite("morph_tophat_" + strel_name + ".png", dst);

    cv::morphologyEx(src, dst, cv::MORPH_BLACKHAT, strel);
    imwrite("morph_blackhat_" + strel_name + ".png", dst);
}

static void save_filtered_images(Mat const& src)
{
    imwrite("src.png", src);

    { // such blocks '{}' are to ensure deallocation of filtered images to preserve memory
      // and also to enable reuse of the name 'dst'
        Mat dst;
        cv::blur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE));
        imwrite("box.png", dst);
    }

    {
        Mat dst;
        cv::medianBlur(src, dst, BLUR_APERTURE);
        imwrite("median.png", dst);
    }

    {
        Mat dst;
        cv::GaussianBlur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
        imwrite("gauss.png", dst);
    }

    save_morph_images(src, cv::MORPH_RECT, "box");
    save_morph_images(src, cv::MORPH_ELLIPSE, "circle");
    save_morph_images(src, cv::MORPH_CROSS, "cross");

    {
        Mat dst;
        cv::bilateralFilter(src, dst, BLUR_APERTURE, BLUR_BILATERAL_SIGMA, BLUR_BILATERAL_SIGMA);
        imwrite("bilateral.png", dst);
    }
}


int main(int argc, char const** argv)
{
    try
    {
        if (argc != 2)
            throw std::runtime_error("Bad usage: must have input image as sole arg");
        std::string const input_image_path = argv[1];
        Mat const input_image = cv::imread(input_image_path, CV_LOAD_IMAGE_GRAYSCALE);

        save_filtered_images(input_image);

        return 0;
    }
    catch (std::exception const& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
