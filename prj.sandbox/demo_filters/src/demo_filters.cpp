#include <cstdio>
#include <stdexcept>
#include <algorithm>
#include <opencv2/opencv.hpp>


using cv::Mat;
using cv::Size;


static int const BLUR_APERTURE = 15;
static double const BLUR_BILATERAL_SIGMA = 100;
static bool SAVE_SIDE_BY_SIDE = true;


static cv::Mat hstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();

  cv::Mat r(std::max(m1.rows, m2.rows), m1.cols + m2.cols, m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(m1.cols, 0), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}

static cv::Mat vstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();

  cv::Mat r(m1.rows + m2.rows, std::max(m1.cols, m2.cols), m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(0, m1.rows), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}


static void save_result(std::string const& name, cv::Mat const& src, cv::Mat const& dst)
{
    cv::imwrite(name, SAVE_SIDE_BY_SIDE ? hstack(src, dst) : dst);
}


static void save_morph_images(Mat const& src, int strel_shape, std::string const& strel_name)
{
    Mat const strel = cv::getStructuringElement(strel_shape, Size(BLUR_APERTURE, BLUR_APERTURE));
    Mat dst;
    
    cv::dilate(src, dst, strel);
    save_result("morph_dilate_" + strel_name + ".png", src, dst);

    cv::erode(src, dst, strel);
    save_result("morph_erode_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_OPEN, strel);
    save_result("morph_open_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_CLOSE, strel);
    save_result("morph_close_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_GRADIENT, strel);
    save_result("morph_grad_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_TOPHAT, strel);
    save_result("morph_tophat_" + strel_name + ".png", src, dst);

    cv::morphologyEx(src, dst, cv::MORPH_BLACKHAT, strel);
    save_result("morph_blackhat_" + strel_name + ".png", src, dst);
}

static void save_filtered_images(Mat const& src)
{
    save_result("src.png", src, src);

    { // such blocks '{}' are to ensure deallocation of filtered images to preserve memory
      // and also to enable reuse of the name 'dst'
        Mat dst;
        cv::blur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE));
        save_result("box.png", src, dst);
    }

    {
        Mat dst;
        cv::medianBlur(src, dst, BLUR_APERTURE);
        save_result("median.png", src, dst);
    }

    {
        Mat dst;
        cv::GaussianBlur(src, dst, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
        save_result("gauss.png", src, dst);
    }

    save_morph_images(src, cv::MORPH_RECT, "box");
    save_morph_images(src, cv::MORPH_ELLIPSE, "circle");
    save_morph_images(src, cv::MORPH_CROSS, "cross");

    {
        Mat dst;
        cv::bilateralFilter(src, dst, BLUR_APERTURE, BLUR_BILATERAL_SIGMA, BLUR_BILATERAL_SIGMA);
        save_result("bilateral.png", src, dst);
    }

    {
        Mat smooth;
        cv::GaussianBlur(src, smooth, Size(BLUR_APERTURE, BLUR_APERTURE), 0);
        
        Mat dst;
        cv::Canny(smooth, dst, 20, 80);
        save_result("canny.png", src, dst);
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
