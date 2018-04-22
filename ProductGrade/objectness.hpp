//
//  objectness.hpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//



#ifndef objectness_hpp
#define objectness_hpp

#include <opencv2/opencv.hpp>

extern std::pair<int, int> nativeResolution ;

int objectness(cv::Mat&, cv::Mat&, cv::Rect, cv::Point, cv::Mat& src_original, cv::Mat& pixelDifference) ;


#endif /* objectness_hpp */
