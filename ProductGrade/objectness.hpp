//
//  objectness.hpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//



#ifndef objectness_hpp
#define objectness_hpp

#include "opencv2/opencv.hpp"

extern std::pair<int, int> nativeResolution ;

int objectness(cv::Mat&, cv::Mat&, cv::Rect, cv::Point, cv::Mat& src_original, cv::Mat& pixelDifference) ;

int containsHorizontalLine(std::vector<cv::Vec4i> candidateLines,cv::Rect bboxWindow,cv::Point offset,cv::Mat& lanes_orig_frame);

void generateHorizontalLines(std::vector<cv::Vec4i>& lines,std::vector<cv::Vec4i>& filteredLines, cv::Mat& verticalGradients,int lineThreshold=10,int minLength=10,int maxLength=25) ;


#endif /* objectness_hpp */
