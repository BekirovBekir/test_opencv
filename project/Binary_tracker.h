#ifndef BIN_TRACK_H
#define BIN_TRACK_H

#include "opencv2/video/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>

// main function to detect binary marker (single) in frame
bool Binary_detector(const cv::Mat &im,
	                   const cv::Mat &code,
					   std::vector<cv::Point2f> &detect_crnrs,
	                   const int min_contour,
	                   const float eps);

// main function to detect binary markers (multi) in frame
bool Binary_detector_multi(const cv::Mat &im,
	                       const std::vector<cv::Mat> &code,
						   std::vector < std::vector < cv::Point2f > > &detect_crnrs,
						   std::vector<int> &detect_idxs,
	                       const int min_contour,
	                       const float eps);

// find Quad contours on image
std::vector < std::vector < cv::Point2f > > FindContours(const cv::Mat &im, const int min_contour, const float eps);

// detect Binary marker in quad patches
bool DetectBinaryMarker(const cv::Mat &image, 
	                    const std::vector < cv::Point2f > &m_crnrs,
	                    const cv::Mat &marker_code,
	                    const std::vector < std::vector < cv::Point2f > > &test_crnrs,
					    std::vector < cv::Point2f > &out_corners,
						const int marker_sz);

// detect Binary markerS in quad patches
bool DetectBinaryMarkers(const cv::Mat &image,
	                     const std::vector < cv::Point2f > &m_crnrs,
						 const std::vector <cv::Mat > &marker_code,
	                     const std::vector < std::vector < cv::Point2f > > &test_crnrs,
						 std::vector < std::vector < cv::Point2f > > &out_corners,
						 std::vector < int > &marker_idx,
	                     const int marker_sz);

// recongnize binary marker in quad
int PatchDecoding(const cv::Mat &image,
	              const std::vector < cv::Point2f >  &m_crnrs,
				  const std::vector < cv::Mat> &marker_code,
				  const std::vector < cv::Point2f > &test_crnrs,
				  std::vector < cv::Point2f > &out_corners,
				  const int marker_sz);

// allign quad corners in clock-wise order
void AlignCornersOrderCW(std::vector <cv::Point2f> &crnrs);

// check padding arround marker code
bool CheckPadding(const cv::Mat &im, const int cell_size);

//read binary code from quad patch
cv::Mat ReadCode(const cv::Mat &in_image, const int cell_size);

// match detected binary code with patterns
bool BinaryMatching(const cv::Mat &test_code, const cv::Mat &pattern_code);

// rotate detected code on 90 degress
cv::Mat Rotate90CW55(const cv::Mat &in_image);

//read camera intrinsic parameters from YML file
bool LoadCameraParameters(const std::string &filename, cv::Mat &camera_matrix, cv::Mat &distortion_coefficients);

#endif
