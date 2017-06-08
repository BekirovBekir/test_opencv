#include <string>
#include <iostream>
#include <cstdio>
#include  <thread>
#include <chrono>

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "Binary_tracker.h"
#include "TcpServer.h"

cv::Mat frameOut;
cv::Mat frame;
cv::Mat vis;
TcpServer* tcpServ;

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
    writeFrame(vis, *tcpServ);
    notify_event=false;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

static void showUsage(){
    using namespace std;

    cout << "Usage: tcpserver <port> \n";
    exit(1);

}

int main(int argc, char* argv[])
{

	//~~~~ settings ~~~~

    cv::VideoCapture cap(0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

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

    //cv::Mat frame;
    cv::Mat f_gray;
    //cv::Mat vis;
    std::vector < std::vector < cv::Point2f > > detected_marker_crnrs;
    std::vector<int> detected_marker_idxs;
    bool detect_stat = false;

    if (argc != 2) showUsage();

    tcpServ=new TcpServer(std::atoi(argv[1]));

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


        std::unique_lock<std::mutex> locker(mutex_event);
        notify_event=true;
        event.notify_one();

        cv::waitKey(5);
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
    //cv::RNG rng;
	cv::Scalar color;
	for (int i = 0; i < idxs.size(); ++i)
	{
        color = cv::Scalar(50,205,50,0.5);//(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

        cv::line(vis, crnrs[i][0], crnrs[i][1], color, 8, 8);
        cv::line(vis, crnrs[i][1], crnrs[i][2], color, 8, 8);
        cv::line(vis, crnrs[i][2], crnrs[i][3], color, 8, 8);
        cv::line(vis, crnrs[i][3], crnrs[i][0], color, 8, 8);
        cv::line(vis, crnrs[i][0], crnrs[i][2], color, 8, 8);
        cv::line(vis, crnrs[i][1], crnrs[i][3], color, 8, 8);

        cv::putText(vis, std::to_string(idxs[i]), crnrs[i][0] + cv::Point2f(10, 10), cv::FONT_ITALIC, 2, cv::Scalar(50,205,50,0.5), 1, CV_AA); //cv::FONT_ITALICFONT_HERSHEY_COMPLEX_SMALL
	}

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

