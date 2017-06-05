#include <string>
#include <iostream>
#include <cstdio>
#include  <thread>
#include "chrono"
#include <gtk/gtk.h>

#include "Binary_tracker.h"

#include "opencv2/opencv.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "TcpServer.h"
TcpServer tcpServ(19090);

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

cv::Mat frameOut;
cv::Mat vis;
static constexpr int JPEG_QUALITY = 50;

std::condition_variable event;
std::mutex mutex_event;
bool notify_event=false;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// draw detected marker contours
void DrawDetectMarkers(cv::Mat &vis, const std::vector < std::vector < cv::Point2f > > &crnrs, const std::vector<int> &idxs);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//--------------------------------------------
// Write the frame to stdout (binary mode) as jpg with a size header at present
static void writeFrame(const cv::Mat & frame, TcpServer & tcpServ)
{
    using namespace cv;
    using namespace std;

    vector<uchar> buf; // JPEG buffer

    // Encode frame -> buf as jpeg
    // imwrite("test.jpg", frameOut, {IMWRITE_JPEG_QUALITY, JPEG_QUALITY});
    imencode(".jpg", frame, buf, {IMWRITE_JPEG_QUALITY, JPEG_QUALITY});

    int size = buf.size(); // Jpeg image size

    //    cerr << "size = " << size << endl;

    // Send jpg data to all connected TCP clients
    tcpServ.sendData(buf.data(), size);
}
//--------------------------------------------

// Process the frame, put calls to any algorithms here (preferably in separate cpp files)
static void processFrame(const cv::Mat & in, cv::Mat & out)
{
    using namespace cv;
    using namespace std;

    out = in.clone(); // Copy

    // We Work on out here
    int tx = in.cols / 3;  // Size of the central 1/3
    int ty = in.rows / 3;
    Mat r(out, Rect(tx, ty, tx, ty)); // Central 1/3
    bitwise_not(r, r); // Photo negative
}
//--------------------------------------------

void thread1 ()
{
    while (1)
    {
    std::unique_lock<std::mutex>locker(mutex_event);
        while (!notify_event) event.wait(locker);
        notify_event=false;
    processFrame(vis, frameOut);
    writeFrame(frameOut, tcpServ);
    notify_event=false;
    }
}

int main()
{

	//~~~~ settings ~~~~

    cv::VideoCapture cap(0);

    // set binary markers
    std::vector<cv::Mat> marker_codes(2);
    marker_codes[0] = (cv::Mat_<uchar>(5, 5) << 1,1,0,0,1,
                                                0,0,1,1,0,
                                               0,1,1,1,1,
                                                0,1,1,1,1,
                                                0,1,0,0,1);

    marker_codes[1] = (cv::Mat_<uchar>(5, 5) << 0,1,1,1,0,
                                                1,1,1,0,1,
                                                0,1,1,1,0,
                                                0,0,0,0,1,
                                                1,0,0,1,0);
    //Detection parameters

    const int min_cntr_area = 2500;
    const float eps_convex = 0.05;

    // Load camera parameters
    std::string cam_par_file = "Camera Parameters/Gemix_camera_parameters.yml";
    cv::Mat camera_matrix;
    cv::Mat distortions;
    LoadCameraParameters(cam_par_file, camera_matrix, distortions);

    //~~~~~~~~~~~~~~~~~~

    // init variables
    cv::Mat frame;
    cv::Mat f_gray;
    //cv::Mat vis;
    std::vector < std::vector < cv::Point2f > > detected_marker_crnrs;
    std::vector<int> detected_marker_idxs;

    bool detect_stat = false;


    //TcpServer tcpServ(19090); // Start the tcp server
    cv::namedWindow("TcpServer",1);
    cv::Mat frameIn, frameOut;

    std::thread thr1(thread1);
    thr1.detach();

    // start tracking loop
    while (char(cv::waitKey(1)) != 'q')
    {
        cap >> frame;

        if (frame.empty())
        {
            std::cout << "VideoCapture: no signal!" << std::endl;
            break;
        }

        frame.channels() > 1 ? cv::cvtColor(frame, f_gray, CV_BGR2GRAY) : frame.copyTo(f_gray);

        // try to detect markers in frame
        detect_stat = Binary_detector_multi(f_gray, marker_codes, detected_marker_crnrs, detected_marker_idxs, min_cntr_area, eps_convex);

        // visualize detection result
        vis = frame.clone();
        if (detect_stat)
        {
            DrawDetectMarkers(vis, detected_marker_crnrs, detected_marker_idxs);
        }

        cv::imshow("Binary multi tracker", vis);



        // Process the frame
       // processFrame(vis, frameOut);
       //Show 1 frame
       //imshow("TcpServer", frameOut);

       // Encode and write to stdout the frame as jpeg
       //writeFrame(vis, tcpServ);


        std::unique_lock<std::mutex> locker(mutex_event);
        notify_event=true;
        event.notify_one();

        cv::waitKey(1);

	}
		
	return 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// draw detected marker contours
void DrawDetectMarkers(cv::Mat &vis, const std::vector < std::vector < cv::Point2f > > &crnrs, const std::vector<int> &idxs)
{
	if (vis.empty() || crnrs.size() != idxs.size() || idxs.empty())
	{
		std::cerr << "DrawDetectMarkers: wrong inputs !"<< std::endl;
		return;
	}

	if (vis.channels() == 1)
	{
		cv::cvtColor(vis, vis, CV_GRAY2BGR);
	}

	// draw marker contours
	cv::RNG rng;
	cv::Scalar color;
	for (int i = 0; i < idxs.size(); ++i)
	{
        color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

		cv::line(vis, crnrs[i][0], crnrs[i][1], color, 3, 8);
		cv::line(vis, crnrs[i][1], crnrs[i][2], color, 3, 8);
		cv::line(vis, crnrs[i][2], crnrs[i][3], color, 3, 8);
		cv::line(vis, crnrs[i][3], crnrs[i][0], color, 3, 8);
		cv::line(vis, crnrs[i][0], crnrs[i][2], color, 3, 8);
		cv::line(vis, crnrs[i][1], crnrs[i][3], color, 3, 8);

		cv::putText(vis, std::to_string(idxs[i]), crnrs[i][0] + cv::Point2f(10, 10), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cv::Scalar(0, 0, 255), 1, CV_AA);
	}

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

