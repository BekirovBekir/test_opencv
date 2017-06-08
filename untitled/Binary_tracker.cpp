
#include "Binary_tracker.h"

//......................................................................................

// main function to detect binary marker (single) in frame
bool Binary_detector(const cv::Mat &im,
	                 const cv::Mat &code,
					 std::vector<cv::Point2f> &detect_crnrs,
	                 const int min_contour,
	                 const float eps)
{
	// check input image
	if (im.empty() || im.channels() != 1 || code.empty())
	{
		std::cerr << "Binary_detector: wrong inputs!" << std::endl;
		detect_crnrs.clear();
		return false;
	}

	//set marker corners
	int marker_sz = 140;
	std::vector < cv::Point2f > m_crnrs(4);
	m_crnrs[0] = cv::Point2f(0.f, 0.f);
	m_crnrs[1] = cv::Point2f(marker_sz, 0.f);
	m_crnrs[2] = cv::Point2f(marker_sz, marker_sz);
	m_crnrs[3] = cv::Point2f(0.f, marker_sz);

	//detect contours on image
	std::vector < std::vector < cv::Point2f > > test_crnrs = FindContours(im, min_contour, eps);

	// check number of detected corners
	if (test_crnrs.empty())
	{
		std::cout << "Binary_detector: no detected contours! " << std::endl;
		detect_crnrs.clear();
		return false;
	}

	// recognize marker from contours
	if (!DetectBinaryMarker(im, m_crnrs, code, test_crnrs, detect_crnrs, marker_sz))
	{
		std::cout << "Binary_detector: marker i not recognized" << std::endl;
		detect_crnrs.clear();
		return false;
	}
	
	// apply subpixel precision for detected marker corners
	cornerSubPix(im, detect_crnrs, cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03));

	return true;
}

// main function to detect binary markers (multi) in frame
bool Binary_detector_multi(const cv::Mat &im,
	                       const std::vector<cv::Mat> &code,
						   std::vector < std::vector < cv::Point2f > > &detect_crnrs,
						   std::vector<int> &detect_idxs,
	                       const int min_contour,
                           const float eps)
{
	// check input image
	if (im.empty() || im.channels() != 1 || code.empty())
	{
		std::cerr << "Binary_detector_multi: wrong inputs!" << std::endl;
		detect_crnrs.clear();
		detect_idxs.clear();
		return false;
	}

	//set marker corners
	int marker_sz = 70;
	std::vector < cv::Point2f > m_crnrs(4);
	m_crnrs[0] = cv::Point2f(0.f, 0.f);
	m_crnrs[1] = cv::Point2f(marker_sz, 0.f);
	m_crnrs[2] = cv::Point2f(marker_sz, marker_sz);
	m_crnrs[3] = cv::Point2f(0.f, marker_sz);

	//detect contours on image
	std::vector < std::vector < cv::Point2f > > test_crnrs = FindContours(im, min_contour, eps);

	// check number of detected corners
	if (test_crnrs.empty())
	{
		std::cout << "Binary_detector_multi: no detected contours! " << std::endl;
		detect_crnrs.clear();
		detect_idxs.clear();
		return false;
	}

	// detect Binary markers from  quad patches
	if (!DetectBinaryMarkers(im, m_crnrs, code, test_crnrs, detect_crnrs, detect_idxs, marker_sz))
	{
		std::cout << "Binary_detector_multi: markers is not recognized" << std::endl;
		detect_crnrs.clear();
		detect_idxs.clear();
		return false;
	}

	// apply subpixel precision for detected marker corners
	for (int i = 0; i < detect_crnrs.size(); ++i)
	{
		cornerSubPix(im, detect_crnrs[i], cv::Size(5, 5), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03));
	}

	return true;
}

std::vector < std::vector < cv::Point2f > > FindContours(const cv::Mat &im, const int min_contour, const float eps)
{
	assert(im.channels() == 1);

	cv::Mat im_th;
	adaptiveThreshold(im, im_th, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 7, 7);

	std::vector < std::vector < cv::Point > > cntrs;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(im_th, cntrs, hierarchy, cv::RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	int i = 0;
	std::vector < cv::Point2f >  cntr_apprx;
	std::vector < std::vector < cv::Point2f > > cntrs_filt;

	for (; i >= 0; i = hierarchy[i][0])
	{
		if ((int)cv::contourArea(cntrs[i]) > min_contour)
		{
			approxPolyDP(cntrs[i], cntr_apprx, cntrs[i].size() * eps, true);

			if (cntr_apprx.size() == 4 && cv::isContourConvex(cntr_apprx))
			{
				AlignCornersOrderCW(cntr_apprx);
				cntrs_filt.push_back(cntr_apprx);
			}
		}
	}

	return cntrs_filt;
}

//......................................................................................
bool DetectBinaryMarker(const cv::Mat &image, 
	                    const std::vector < cv::Point2f > &m_crnrs,
	                    const cv::Mat &marker_code,
	                    const std::vector < std::vector < cv::Point2f > > &test_crnrs,
					    std::vector < cv::Point2f > &out_corners,
						const int marker_sz)
{
	// clear outputs
	out_corners.clear();

	//set cell size
	int cell_sz = marker_sz / 7;

	// init var-s
	cv::Mat warped_marker;
	cv::Mat test_code;
	cv::Mat Tmat;
	std::vector < cv::Mat> _code;
	_code.push_back(marker_code);

	int idx = -1;
	for (int i = 0; i < test_crnrs.size(); ++i)
	{
		idx = PatchDecoding(image, m_crnrs, _code, test_crnrs[i], out_corners, marker_sz);

		if (idx > -1 && !out_corners.empty())
		{
			return true;
		}
	}

	out_corners.clear();
	return false;
}

// detect Binary marker in quad patches
bool DetectBinaryMarkers(const cv::Mat &image,
	                     const std::vector < cv::Point2f > &m_crnrs,
	                     const std::vector <cv::Mat> &marker_code,
	                     const std::vector < std::vector < cv::Point2f > > &test_crnrs,
						 std::vector < std::vector < cv::Point2f > > &out_corners,
	                     std::vector < int > &marker_idx,
	                     const int marker_sz)
{
	// clear outputs
	out_corners.clear();
	marker_idx.clear();

	//set cell size
	int cell_sz = marker_sz / 7;

	// init var-s
	cv::Mat warped_marker;
	cv::Mat test_code;
	cv::Mat Tmat;
	int idx = -1;
	std::vector < cv::Point2f > _crnrs;
	for (int i = 0; i < test_crnrs.size(); ++i)
	{
		idx = PatchDecoding(image, m_crnrs, marker_code, test_crnrs[i], _crnrs, marker_sz);

		if (idx > -1 && !_crnrs.empty())
		{
			out_corners.push_back(_crnrs);
			marker_idx.push_back(idx);
		}
	}

	if (out_corners.empty() || marker_idx.empty())
	{
		out_corners.clear();
		marker_idx.clear();
		return false;
	}
	
	return true;
}

// recongnize binary marker in quad
int PatchDecoding(const cv::Mat &image,
	              const std::vector < cv::Point2f >  &m_crnrs,
				  const std::vector < cv::Mat> &marker_code,
				  const std::vector < cv::Point2f > &test_crnrs,
				  std::vector < cv::Point2f > &out_corners,
				  const int marker_sz)
{
	// clear outputs
	out_corners.clear();

	//set cell size
	int cell_sz = marker_sz / 7;

	// init var-s
	cv::Mat warped_marker;
	cv::Mat test_code;
	cv::Mat Tmat;

	// Get the Perspective Transform Matrix
	Tmat = cv::getPerspectiveTransform(test_crnrs, m_crnrs);
	// Apply the Perspective Transform 
	cv::warpPerspective(image, warped_marker, Tmat, cv::Size(marker_sz, marker_sz));

	//thresholding of warped marker
	cv::threshold(warped_marker, warped_marker, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	if (!CheckPadding(warped_marker, cell_sz))//check black padding
	{
		return -1;
	}
	
	test_code = ReadCode(warped_marker, cell_sz);

	// check 4 vars of marker rotation with all marker patterns
	for (int m_idx = 0; m_idx < marker_code.size(); ++m_idx)
	{
		//check central code element
		if (test_code.at<uchar>(2, 2) != marker_code[m_idx].at<uchar>(2, 2))
		{
			continue;
		}

		//check sums of code elements
		if (cv::sum(test_code) != cv::sum(marker_code[m_idx]))
		{
			continue;
		}

		//#1
		if (BinaryMatching(test_code, marker_code[m_idx]))
		{
			out_corners = test_crnrs;
			return m_idx;
		}

		//#2
		test_code = Rotate90CW55(test_code);
		if (BinaryMatching(test_code, marker_code[m_idx]))
		{
			out_corners = { test_crnrs[3], test_crnrs[0], test_crnrs[1], test_crnrs[2] };
			return m_idx;
		}

		//#3
		test_code = Rotate90CW55(test_code);
		if (BinaryMatching(test_code, marker_code[m_idx]))
		{
			out_corners = { test_crnrs[2], test_crnrs[3], test_crnrs[0], test_crnrs[1] };
			return m_idx;
		}

		//#4
		test_code = Rotate90CW55(test_code);
		if (BinaryMatching(test_code, marker_code[m_idx]))
		{
			out_corners = { test_crnrs[1], test_crnrs[2], test_crnrs[3], test_crnrs[0] };
			return m_idx;
		}
	}
	
	return -1;
}
//......................................................................................

bool CheckPadding(const cv::Mat &im, const int cell_size)
{
	int sum = 0;
	int th = cell_size*cell_size / 2;
	cv::Mat cell_roi;

	for (int y = 0; y < 7; ++y )
	{
		for (int x = 0; x < 7; ++x )
		{
			if (x == 0 || x == 6 || y == 0 || y == 6)
			{
				cell_roi = im(cv::Rect(x*cell_size, y*cell_size, cell_size, cell_size));
				if (cv::countNonZero(cell_roi) > th)
				{
					return false;
				}
			}
		}
	}

	return true;
}

//......................................................................................
void AlignCornersOrderCW(std::vector <cv::Point2f> &crnrs)
{
	// check as clock wise order of points
	double dx1, dx2, dy1, dy2;
	double o;

	dx1 = crnrs[1].x - crnrs[0].x;
	dy1 = crnrs[1].y - crnrs[0].y;
	dx2 = crnrs[2].x - crnrs[0].x;
	dy2 = crnrs[2].y - crnrs[0].y;
	o = (dx1*dy2) - (dy1*dx2);

	if (o < 0.0) //contr clockwise
	{
        //cv::swap(crnrs[1], crnrs[3]); ///
	}
}
//......................................................................................

cv::Mat ReadCode(const cv::Mat &in_image, const int cell_size)
{
	int th = cell_size*cell_size / 2;
	cv::Mat cell_roi; 
	cv::Mat code = cv::Mat::zeros(5, 5, CV_8U);

	for (int y = 1; y < 6; ++y)
	{
		for (int x = 1; x < 6; ++x)
		{
			cell_roi = in_image(cv::Rect(x*cell_size, y*cell_size, cell_size, cell_size));
			if (cv::countNonZero(cell_roi) > th)
			{
				code.at<uchar>(y - 1, x - 1) = 1;
			}
		}
	}

	return code;
}

//......................................................................................

bool BinaryMatching(const cv::Mat &test_code, const cv::Mat &pattern_code)
{
	cv::Mat diffmat;
	cv::bitwise_xor(pattern_code, test_code, diffmat);
	//cv::absdiff(pattern_code, test_code, diffmat);

	if (cv::countNonZero(diffmat) > 0)
	{
		return false;
	}

	return true;
}
//......................................................................................

cv::Mat Rotate90CW55(const cv::Mat &in_image)
{
	assert(in_image.rows == 5 && in_image.cols == 5);

	cv::Mat out_image(5,5,CV_8U);

	out_image.at<uchar>(0, 0) = in_image.at<uchar>(4, 0);
	out_image.at<uchar>(0, 1) = in_image.at<uchar>(3, 0);
	out_image.at<uchar>(0, 2) = in_image.at<uchar>(2, 0);
	out_image.at<uchar>(0, 3) = in_image.at<uchar>(1, 0);
	out_image.at<uchar>(0, 4) = in_image.at<uchar>(0, 0);

	out_image.at<uchar>(1, 0) = in_image.at<uchar>(4, 1);
	out_image.at<uchar>(1, 1) = in_image.at<uchar>(3, 1);
	out_image.at<uchar>(1, 2) = in_image.at<uchar>(2, 1);
	out_image.at<uchar>(1, 3) = in_image.at<uchar>(1, 1);
	out_image.at<uchar>(1, 4) = in_image.at<uchar>(0, 1);

	out_image.at<uchar>(2, 0) = in_image.at<uchar>(4, 2);
	out_image.at<uchar>(2, 1) = in_image.at<uchar>(3, 2);
	out_image.at<uchar>(2, 2) = in_image.at<uchar>(2, 2);
	out_image.at<uchar>(2, 3) = in_image.at<uchar>(1, 2);
	out_image.at<uchar>(2, 4) = in_image.at<uchar>(0, 2);

	out_image.at<uchar>(3, 0) = in_image.at<uchar>(4, 3);
	out_image.at<uchar>(3, 1) = in_image.at<uchar>(3, 3);
	out_image.at<uchar>(3, 2) = in_image.at<uchar>(2, 3);
	out_image.at<uchar>(3, 3) = in_image.at<uchar>(1, 3);
	out_image.at<uchar>(3, 4) = in_image.at<uchar>(0, 3);

	out_image.at<uchar>(4, 0) = in_image.at<uchar>(4, 4);
	out_image.at<uchar>(4, 1) = in_image.at<uchar>(3, 4);
	out_image.at<uchar>(4, 2) = in_image.at<uchar>(2, 4);
	out_image.at<uchar>(4, 3) = in_image.at<uchar>(1, 4);
	out_image.at<uchar>(4, 4) = in_image.at<uchar>(0, 4);

	//cv::transpose(in_image, out_image);
	//cv::flip(out_image, out_image, 1);
	return out_image;
}

//......................................................................................

//read camera intrinsic parameters from YML file
bool LoadCameraParameters(const std::string &filename, cv::Mat &camera_matrix, cv::Mat &distortion_coefficients)
{
	//read camera parameters
	cv::FileStorage fs;

	fs.open(filename, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		std::cerr << "LoadCameraParameters: Failed to open file with camera parameters!" << filename << std::endl;
		return false;
	}

	fs["cameraMatrix"] >> camera_matrix;
	fs["distCoeffs"] >> distortion_coefficients;

	fs.release();

	return true;
}

//......................................................................................
