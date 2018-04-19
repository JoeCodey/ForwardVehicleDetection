//
//  main.cpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
//#include "data.hpp"
#include "objectness.hpp"
#include "configuration.hpp"
#include <ctime>


using namespace cv ;


bool comp(std::pair<int,Rect>& i,std::pair<int,Rect>& j) {
    return i.first > j.first ;
}


int imageWidth, imageHeight ;


int main(int argc, const char * argv[]) {
    
    
//    for(int i = 0 ; i < videoNames.size() ; ++i){
//    cv::VideoCapture cap(path_video+videoNames[0])
//    }
    
    Mat orig_frame ;
    Mat lanes_orig_frame ;
    Mat gray_lanes ;
    int lane_offset ;
    
    Mat grad_x, grad_y , gradients_sobel ; // Mat images to develop sobel
    Mat cannyEdges ;

    for(int imageId = 0 ; imageId < filenames.size() ; ++imageId){
      
        orig_frame = imread(pathtoData+filenames[imageId]+".png") ;
        if(orig_frame.empty()){
            std::cout << "She is empty, throwing exception" << std::endl ;
            //throw Exception() ;
        }
        
        std::cout << "\n**** Reading Image at " << filenames[imageId] << " *******\n" << std::endl ;
        
        imshow("Orig_Frame", orig_frame) ;
        char StartKey = waitKey() ;
        if(StartKey == (int) 'n'){
            destroyAllWindows();
            continue ;
        }
        
        while(true){
            
            std::cout << "Press 's' to start object scoring" << std::endl ;
            if(StartKey == (int) 's')
                break ;
            
            StartKey = waitKey() ;
        }
        int start_s=clock();
        // the code you wish to time goes here
      
        imageHeight = orig_frame.rows ;
        imageWidth = orig_frame.cols ;
        lane_offset = imageWidth/4 ;
        
        lane_coordinate lane = lane_coordinate(imageWidth,4,0.85,imageHeight-removeText,25) ;
        
        Mat trapMask = Mat::zeros(orig_frame.rows, orig_frame.cols, CV_8UC1) ;
         int npts[] = {4} ;
        fillPoly( trapMask, lane.ppts , npts, 1, Scalar( 255, 255, 255 ), 8 );
        
//        imshow("polyMask", trapMask) ;
//        waitKey() ;
        
        /* Now we need a default retangular ROI to contain the trapezoidal Mat */
        int width_roi = lane.laneBottomWidth ;
        int height_roi = lane.BottomLeft.y - lane.TopLeft.y ;
        
        Point offsetROI(lane.bottomOffset,lane.top_lane) ; //
        Rect ROI_default(offsetROI.x,offsetROI.y,width_roi,height_roi);
        
        orig_frame.copyTo(lanes_orig_frame, trapMask) ;
        lanes_orig_frame = lanes_orig_frame(ROI_default) ;
    
//        imshow("lanes_orig_frame", lanes_orig_frame);
//        waitKey() ;
        
        
        
        
        
        /*Convert frame to gray*/
        
        cvtColor(lanes_orig_frame, gray_lanes, CV_BGR2GRAY) ;
        
        /* Apply standard Guassian Blur filter */
        
        GaussianBlur(gray_lanes, gray_lanes, Size(5,5), 0, BORDER_DEFAULT) ;
        
        
        /* Store canny properties in struct (stored in configuration.hpp) */
        cannyProperties canny_prop(100,150) ;
        
        /* calculate canny edge detection of window */
        Canny(gray_lanes,cannyEdges,canny_prop.minThreshold,canny_prop.maxThreshold) ;
        
        /* Calculate sobel gradients of frame */
        Sobel( gray_lanes, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
        Sobel( gray_lanes, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
        
        convertScaleAbs( grad_x, grad_x );
        convertScaleAbs( grad_y, grad_y );
        addWeighted( grad_x, 0.5, grad_y, 0.5, 0, gradients_sobel ) ;
        
        movingWindowProperties window(3,2) ; // Largest box = 1/3 of width of bottom of lane
                                             // Smallest box = 1/2 of width of top of lane
        window.starting_bbox_size = cvRound(lane.laneBottomWidth/window.bbound_largest_ratio) ;
        window.smallest_bbox_size = cvRound(lane.laneTopWidth/window.bbound_smallest_ratio) ;
        
        
        int window_n_rows = window.starting_bbox_size ;
        int window_n_cols = window.starting_bbox_size ;
        
   
        
        std::vector<std::pair<int,Rect>> candidates ;
        std::pair<int,Rect> scoreCandidate ;
        std::pair<int,Rect>  bestofthree [3] ;
        int counter_bbox = 0 ;
        int max_among3 = 0 ;
        
        /* Linear equation for decreasing bounding_box size as we travel up the lane */
            // y = mx + b ,
                // let x be the row containing the bottom most edge of the boudning box
                // let y be the width of the bottom most edge of the bounding box
            // m = (x2 - x1) - (y2 - y1)
                // x2 is top of frame , x1 is bottom of frame
                // y2 is width of smallest bouding box, y1 is width of largest bounding bo
                // (x1 , y1) : x1 = src_lanes.rows , y1 = starting_bbox_size
                // (x2 , y2) : x2 = 0 , y2 = smallest_bbox_size
                // b = smallest_box_size  ;
        float m = (window.smallest_bbox_size - window.starting_bbox_size) / (0.0 - lane.laneHeight) ;
        
        
        int bufferRegion = window.bufferRegion ;
        int scoreIndivWindow ;
        
        int totalScoreAllCandidates = 0 ;
        
        window.stepSlide = 2 ;
        
        
        for (int col = lane.laneBottomWidth/3 ; col  <= lane.laneBottomWidth - window_n_cols; col += window.stepSlide){
            //std::cout << "outer loop" << std::endl ;
            
            for (int row = lane.laneHeight ; row >= window.stepSlide ; row -= window.stepSlide){
                // std::cout << "inner loop" << std::endl ;
                Rect bboxWindow(col,row,window_n_cols,window_n_rows) ;
                
                Rect cannyWindow(col-window.bufferRegion,row-window.bufferRegion,window_n_cols+window.bufferRegion*2,
                                 window_n_rows+window.bufferRegion*2) ;
                
                if(row > lane.laneHeight - (window_n_rows + bufferRegion))
                    cannyWindow.height = lane.laneHeight - (row-bufferRegion) ;
                else if (row < window_n_rows + bufferRegion)
                    cannyWindow.y = 0 ;
                
                if(col > lane.laneBottomWidth - (window_n_cols + bufferRegion*2)){
                    cannyWindow.width = lane.laneBottomWidth - (col-bufferRegion) ;
                }
                
                Mat canny_of_window = cannyEdges(cannyWindow) ;
                scoreIndivWindow = objectness(canny_of_window,gradients_sobel, bboxWindow, Point(cannyWindow.x,cannyWindow.y), gray_lanes) ;
                scoreCandidate = std::pair<int,Rect>(scoreIndivWindow,bboxWindow) ;
                
                bestofthree[counter_bbox] = scoreCandidate ;
          
                
                if(scoreIndivWindow > bestofthree[max_among3].first)
                    max_among3 = counter_bbox ;
                counter_bbox++;
                
                if (counter_bbox == 3){
                    // reset counter which counts every third candidate checked.
                    counter_bbox = 0 ;
                    if(bestofthree[max_among3].first != 0){
                        candidates.push_back(bestofthree[max_among3]);
                        totalScoreAllCandidates += bestofthree[max_among3].first ;
                    }
                    max_among3 = 0 ;
                }
                /* Calcualtion of a box complement, now we must decrease the size of the ROI
                 as we move up the column (i.e. the lane ) of the frame */
                window_n_cols  =  m*(row - window.stepSlide) + window.smallest_bbox_size ;
                window_n_rows = m*(row - window.stepSlide) + window.smallest_bbox_size ;
                
                //waitKey() ;
    
            }
            /* OUTER For loop */
            window_n_cols = window.starting_bbox_size ;
            window_n_rows = window.starting_bbox_size ;
            
        }
        /* End of candidate generation */
        
        
        if(candidates.empty()){
            std::cout << "No candidate detected, noisy imaage" << std::endl ;
            continue ;
        }

        std::cout << candidates.size() << std::endl ;
        Mat copy_orig ;
        
        
        std::sort(candidates.begin(),candidates.end()
                  ,comp );
        
        std::vector<Rect> topRectangles ; // For clustering
        for(int i = 0 ; i < 25 ; ++i){
            topRectangles.push_back(candidates[i].second);
        }
        groupRectangles(topRectangles, 2);
        Mat orig_frame_copy ;
        orig_frame.copyTo(orig_frame_copy);
        int greenLevel = 255 ;
        for(int j = 0 ; j < topRectangles.size() ; ++j){
            
            
            if (j == 0){
                rectangle(orig_frame_copy, topRectangles[j].tl()+offsetROI,topRectangles[j].br()+offsetROI, Scalar(0,greenLevel,0),1) ;
            }else{
                float ratio =  1 ;
                greenLevel = greenLevel * (ratio) ;
                rectangle(orig_frame_copy, topRectangles[j].tl()+offsetROI,topRectangles[j].br()+offsetROI, Scalar(0,greenLevel,0),1) ;
            }
        }
        namedWindow("clustering_rectanlges");
        moveWindow("clustering_rectanlges", 0,nativeResolution.second-orig_frame_copy.rows) ;
        imshow("clustering_rectanlges", orig_frame_copy);
        greenLevel = 255 ;
      
            
            
        for(int j = 0 ; j < 25 ; ++j){
            
            if (j == 0){
                rectangle(orig_frame, candidates[j].second.tl()+offsetROI,candidates[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
            }else{

                float ratio =  candidates[j].first / candidates[j-1].first ;

                greenLevel = greenLevel * (ratio) ;
                rectangle(orig_frame, candidates[j].second.tl()+offsetROI,candidates[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
            }
        }
        imshow("Candidates", orig_frame) ;
        int stop_s=clock();
         waitKey() ;
        objectness(cannyEdges, gradients_sobel, candidates[0].second, Point(0,0), lanes_orig_frame) ;
        
        std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
        std::cout << "\n/***** End of Processing ****/ " << std::endl ;
        waitKey() ;
        destroyAllWindows() ;
        
       
        
        
    
      
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    }
    
    
    
    
    
    
    
    
    
    
    
    
}
