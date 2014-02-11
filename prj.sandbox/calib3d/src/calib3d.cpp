#include <cstdio>
#include <vector>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>


using std::string;
using std::vector;
using cv::Mat;
using cv::Point2f;
using cv::Point3f;


int main()
{
  vector<string> fn;
  
  fn.push_back("right01");
  fn.push_back("right02");
  fn.push_back("right03");
  fn.push_back("right04");
  fn.push_back("right05");
  fn.push_back("right06");
  fn.push_back("right07");
  fn.push_back("right08");
  fn.push_back("right09");
  fn.push_back("right11");
  fn.push_back("right12");
  fn.push_back("right13");
  fn.push_back("right14");

  cv::Size const board_size(9, 6);
  double const square_size = 1.0;

  Mat camera_matrix;
  Mat dist_coeffs;
  vector<vector<Point2f> > pts_by_img;

  cv::Size src_size;
  for (size_t i = 0; i < fn.size(); ++i)
  {
    printf("Processing %s\n", fn[i].c_str());

    Mat src = cv::imread(fn[i] + ".jpg", CV_LOAD_IMAGE_COLOR);
    src_size = src.size();
    vector<Point2f> pattern_pts;
    bool const found = cv::findChessboardCorners(src, board_size, pattern_pts);
    if (!found)
    {
      printf("Corners not found for %s\n", fn[i].c_str());
      continue;
    }
      
    Mat src_gray;
    cv::cvtColor(src, src_gray, CV_BGR2GRAY);
    cv::cornerSubPix(src_gray, pattern_pts, cv::Size(11,11),
        cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

    Mat dst;
    cv::cvtColor(src_gray, dst, CV_GRAY2BGR);
    cv::drawChessboardCorners(dst, board_size, Mat(pattern_pts), found);
    cv::imwrite("_" + fn[i] + ".corners.jpg", dst);

    pts_by_img.push_back(pattern_pts);
  }

  if (pts_by_img.empty())
  {
    printf("No calibration patterns processed successfully\n");
    return 1;
  }

  camera_matrix = Mat::eye(3, 3, CV_64F);
  dist_coeffs = Mat::zeros(8, 1, CV_64F);
  

  vector<Point3f> corners3d;
  for (int y = 0; y < board_size.height; ++y)
      for (int x = 0; x < board_size.width; ++x)
          corners3d.push_back(Point3f(x * square_size, y * square_size, 0));
  vector<vector<Point3f> > corners3d_by_img(pts_by_img.size(), corners3d);



  vector<Mat> rvecs;
  vector<Mat> tvecs;
  double rms = cv::calibrateCamera(corners3d_by_img, pts_by_img, src_size, camera_matrix,
                                   dist_coeffs, rvecs, tvecs, CV_CALIB_FIX_K4);


  Mat map1;
  Mat map2;
  cv::initUndistortRectifyMap(camera_matrix, dist_coeffs, Mat(),
      cv::getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, src_size, 1, src_size, 0),
      src_size, CV_16SC2, map1, map2);


  for (size_t i = 0; i < fn.size(); ++i)
  {
    printf("Correcting %s\n", fn[i].c_str());

    Mat src = cv::imread(fn[i] + ".jpg", CV_LOAD_IMAGE_COLOR);
    Mat undist;
    cv::remap(src, undist, map1, map2, cv::INTER_LINEAR);
    cv::imwrite("_" + fn[i] + ".undist.jpg", undist);
  }

  return 0;
}