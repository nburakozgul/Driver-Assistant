#include "lane_detector.h"

Mat LaneDetector::mask_for_elim;

LaneDetector::LaneDetector(Mat image) {

	Mat temp;
	image.copyTo(temp);

	for (int pi = 0; pi < temp.rows; ++pi) {
		for (int pj = 0; pj < temp.cols; ++pj) {

			temp.at<Vec3b>(pi, pj)[0] = 255;
			temp.at<Vec3b>(pi, pj)[1] = 255;
			temp.at<Vec3b>(pi, pj)[2] = 255;
		}
	}

	for (int pi = 0; pi < temp.rows / 4; ++pi) {
		for (int pj = 0; pj < temp.cols; ++pj) {

			temp.at<Vec3b>(pi, pj)[0] = 0;
			temp.at<Vec3b>(pi, pj)[1] = 0;
			temp.at<Vec3b>(pi, pj)[2] = 0;
		}
	}

	LaneDetector::mask_for_elim = temp;
}

Mat LaneDetector::GetWhiteMask(Mat image) {

	Mat tresh_saturation,
		tresh_value,
		hsv;
	int sensitivity = 70;
	vector<Mat> channels;

	// Convert image to HSV, split it to channels
	cvtColor(LaneDetector::mask_for_elim & image, hsv, CV_BGR2HSV);
	split(hsv, channels);

	// http://i.stack.imgur.com/mkq1P.png
	tresh_saturation = channels[1] < sensitivity;
	tresh_value = channels[2] > 255 - sensitivity;

	return tresh_saturation & tresh_value;
}

vector<Vec4i> LaneDetector::GetLanes(Mat image) {

	Mat red_mask = GetWhiteMask(image);
	vector<Vec4i> lanes;

	erode(red_mask, red_mask, Mat(), Point(-1, -1), 1, 1, 1);
	dilate(red_mask, red_mask, Mat(), Point(-1, -1), 2, 1, 1);

	HoughLinesP(red_mask, lanes, 1, CV_PI / 360, 50, 50, 1);

	return lanes;
}

void LaneDetector::DrawLanes(Mat image, vector<Vec4i> lanes, Scalar color, int thickness) {

	for (unsigned int i = 0; i < lanes.size(); ++i)
	{
		line(image, cv::Point(lanes[i][0], lanes[i][1]), 
			cv::Point(lanes[i][2], lanes[i][3]), color, thickness);
	}
}