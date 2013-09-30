#include <cstdio>
#include <stdexcept>
#include <opencv2/opencv.hpp>

// used by extractFace
static int const STANDARD_FACE_WIDTH = 200;


// run OpenCV face detector on img, store detections to result
static void detectFaces(std::vector<cv::Rect> & result, cv::Mat const& img)
{
    cv::CascadeClassifier cascade;
    if (!cascade.load("../data/haarcascade_frontalface_alt.xml"))
        throw std::runtime_error("Unable to load cascade");

    cv::Mat grey;
    cv::cvtColor(img, grey, CV_RGB2GRAY);
    cascade.detectMultiScale(grey, result);
}

// return a new image obtained by drawing supplied rectangles (faces) on img
// img must not be modified
static cv::Mat drawFaces(cv::Mat const& img, std::vector<cv::Rect> const& faces)
{
    cv::Mat canvas = img.clone();
    for (size_t i = 0; i < faces.size(); ++i)
        cv::rectangle(canvas, faces[i], cv::Scalar(0, 255, 0), 2);
    return canvas;
}

// return an image with witdth exactly equal to STANDARD_FACE_WIDTH
// obtained by cropping and scaling the given rect (face) of the img 
static cv::Mat extractFace(cv::Mat const& img, cv::Rect const& face)
{
    if (face.area() == 0)
        return cv::Mat();
    cv::Mat result;
    double const scaleFactor = static_cast<double>(STANDARD_FACE_WIDTH) / face.width;
    cv::resize(img(face), result, cv::Size(), scaleFactor, scaleFactor);
    return result;
}

static cv::Rect selectFace(std::vector<cv::Rect> const& faces)
{
    if (faces.empty())
        return cv::Rect();
    int maxwidth = 0;
    size_t argmax = 0;
    for (size_t i = 0; i < faces.size(); ++i)
    {
        if (faces[i].width > maxwidth)
        {
            maxwidth = faces[i].width;
            argmax = i;
        }
    }
    return faces[argmax];
}


int main(int argc, char const** argv)
{
    /*
    The program should open the video source (camera if no arguments,
    video file if 1 argument), run OpenCV face detector, and display two windows:
        - input image with marked face rectangles (use drawFaces)
        - cropped and resized face image (use extractFace), blank if no faces are found, something else if more than one found

    For face detection, see cv::CascadeClassifier::detectMultiScale: http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html#cascadeclassifier-detectmultiscale
    */

    try
    {
        cv::VideoCapture cap;
        if (argc == 1)
            cap.open(0);
        else
            cap.open(argv[1]);
        if (!cap.isOpened())
            throw std::runtime_error("Unable to open VideoCapture");

        while (true)
        {
            cv::Mat frame;
            if (!cap.read(frame))
                break;

            std::vector<cv::Rect> faces;
            detectFaces(faces, frame);
            cv::imshow("facedetect", drawFaces(frame, faces));
            cv::Rect face = selectFace(faces);
            cv::imshow("face", extractFace(frame, face));
            
            if ((cv::waitKey(20) & 0xFF) == 27)
                break;
        }
        return 0;
    }
    catch (std::exception const& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}
