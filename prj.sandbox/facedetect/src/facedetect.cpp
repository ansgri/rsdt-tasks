// TODO: includes

// used by extractFace
static int const STANDARD_FACE_WIDTH = 200;

// The following functions are to be implemented

// run OpenCV face detector on img, store detections to result
static void detectFaces(std::vector<cv::Rect> & result, cv::Mat const& img);

// return a new image obtained by drawing supplied rectangles (faces) on img
// img must not be modified
static cv::Mat drawFaces(cv::Mat const& img, std::vector<cv::Rect> const& faces);

// return an image with witdth exactly equal to STANDARD_FACE_WIDTH
// obtained by cropping and scaling the given rect (face) of the img 
static cv::Mat extractFace(cv::Mat const& img, cv::Rect const& face);


int main(int argc, char const** argv)
{
    /*
    The program should open the video source (camera if no arguments,
    video file if 1 argument), run OpenCV face detector, and display two windows:
        - input image with marked face rectangles (use drawFaces)
        - cropped and resized face image (use extractFace), blank if no faces are found, something else if more than one found

    For face detection, see cv::CascadeClassifier::detectMultiScale: http://docs.opencv.org/modules/objdetect/doc/cascade_classification.html#cascadeclassifier-detectmultiscale
    */
    return 0;
}
