#include "utils.h"


namespace rsdt { namespace docproc {

cv::Size size_for_wing(int wx, int wy)
{
    return cv::Size(wx * 2 + 1, wy * 2 + 1);
}

cv::Mat morph_filter(cv::Mat const& src, int wx, int wy, int operation)
{
    cv::Mat const strel = cv::getStructuringElement(cv::MORPH_RECT, size_for_wing(wx, wy));
    cv::Mat dst;
    cv::morphologyEx(src, dst, operation, strel);
    return dst;   
}

cv::Mat rotate_around_center(cv::Mat const& src, double angle)
{
    cv::Mat rotated;
    cv::warpAffine(src,
                   rotated, 
                   cv::getRotationMatrix2D(cv::Point2f(src.cols / 2, src.rows / 2), angle, 1.0),
                   src.size());
    return rotated;
}

}}
