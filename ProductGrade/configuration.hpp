//
//  configuration.hpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//

#ifndef configuration_h
#define configuration_h
#include "data.hpp"

cv::String pathtoData = "/Users/josephlefebvre/Honours_Project/ViewnyxData/VYX_MIN50_SAMPLE_FRAMES/" ;

cv::String path_video = "/Users/josephlefebvre/Honours_Project/ViewnyxData/video/Forward Leading/" ;


// Constants for Sobel Edge Image
int scale = 1 ;
int delta = 0 ;
int ddepth = CV_16S ;



int removeText = 100 ;


struct lane_coordinate{
    cv::Point lanePoints[1][4];
    cv::Point BottomLeft, TopLeft, TopRight, BottomRight ;
    
    const cv::Point* ppts[1] = {lanePoints[0] };
    //int npts[] = {4} ;
    int bottomOffset;
    int base_lane ;
    int top_lane ;
    float ratio_top_base;
    int width_ratio ;
    int topOffset_relToBottom ;
    int topOffset ;
    
    int laneBottomWidth ;
    int laneTopWidth ;
    int laneHeight ;
    
    explicit lane_coordinate(int imageWidth,int wr=4,float r=0.75,int b=0,int t=25):  ratio_top_base(r),base_lane(b),top_lane(t),width_ratio(wr) {
        bottomOffset = imageWidth/width_ratio;
        topOffset_relToBottom = bottomOffset*(ratio_top_base) ;
        topOffset = bottomOffset + topOffset_relToBottom ;
        BottomLeft = cv::Point(bottomOffset,base_lane) ;
        TopLeft = cv::Point(topOffset,top_lane) ;
        TopRight = cv::Point(imageWidth - topOffset,top_lane) ;
        BottomRight = cv::Point(imageWidth-bottomOffset,base_lane) ;
        
        lanePoints[0][0] = BottomLeft ;
        lanePoints[0][1] = TopLeft ;
        lanePoints[0][2] = TopRight;
        lanePoints[0][3] = BottomRight ;
        
        laneBottomWidth = BottomRight.x - BottomLeft.x ;
        laneTopWidth = TopRight.x - TopLeft.x ;
        laneHeight = BottomLeft.y - TopLeft.y   ; 
        
        
    }
    
};

struct cannyProperties {
    
    int minThreshold ;
    int maxThreshold ;
    
    explicit cannyProperties(int min=50, int max=150, int ratio=1): minThreshold(min), maxThreshold(max) {
        maxThreshold = minThreshold*ratio ;
    }
    
    
};

struct movingWindowProperties {
    int stepSlide ;
    float bbound_largest_ratio  ;
    float bbound_smallest_ratio  ;
    int bufferRegion ;
    
    int starting_bbox_size ; // Largest size box tested (immediately in front of car)
    int smallest_bbox_size ;
    
    explicit movingWindowProperties(float large,float small,int sl=5, int br=10): bbound_largest_ratio(large), bbound_smallest_ratio(small), stepSlide(sl), bufferRegion(br){
        
    }
};










#endif /* configuration_h */

