//
//  main.cpp
//  ProductGrade
//
//  Created by Joseph lefebvre on 2018-04-16.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//

#include <iostream>
//#include <opencv2/opencv.hpp>
#include "opencv2/opencv.hpp"
//#include "data.hpp"
#include "objectness.hpp"
#include "configuration.hpp"
#include <ctime>


using namespace cv ;

void my_mouse_callback_Sobel(int event , int x , int y, int flags, void* params){
    Mat* clickedImage = (Mat*) params ;
    
    switch( event ){
        case EVENT_MOUSEMOVE:
            std::cout << int( clickedImage->at<uchar>(Point(x,y)) ) << std::endl ;
            
    }
    
}


bool comp(std::pair<int,Rect>& i,std::pair<int,Rect>& j) {
    return i.first > j.first ;
}
// String Path_demo = "/Users/josephlefebvre/Honours_Project/Demo/videoTest/" ;


// int imageWidth, imageHeight ;


int main(int argc, const char * argv[]) {
    
    Mat black_test = Mat::zeros(250,250,CV_8UC1); 
    imshow("include test", black_test);
    
    Mat orig_frame , gray_orig_frame;
    Mat orig_frame2, gray_orig_frame2;
    Mat lanes_orig_frame , lanes_pixel_diff;
    Mat gray_lanes ;
    Mat pixelDiffFrame;
    int lane_offset ;
    
    Mat grad_x, grad_y , gradients_sobel ; // Mat images to develop sobel
    Mat cannyEdges ;
    Mat trapMask ;
    Mat canny_of_window;
    
    for(int vidId = 0 ; vidId < videoNames.size(); ++vidId){
        std::cout << "VIDEO PATH: " << path_video+videoNames[vidId].name+"/"+videoNames[vidId].name+".mp4" << std::endl ;
        std::cout << "Press key to attempt to read video... " << std::endl ;
        
        cv::VideoCapture cap(path_video+videoNames[vidId].name+"/"+videoNames[vidId].name+".mp4") ;
       
    //}
    

        Rect prevFrameDetection ; 
        
        int fpsVideo = cap.get(CAP_PROP_FPS) ;
        int frameWidth = cap.get(CAP_PROP_FRAME_WIDTH) ;
        int frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
        int numFrames = cap.get(CAP_PROP_FRAME_COUNT) ;
        
        cv::VideoWriter videoMaker("/Users/josephlefebvre/Honours_Project/horizonalEVERY3frames/output_"+videoNames[vidId].name +".mp4", cv::VideoWriter::fourcc('m','p','4','v'),fpsVideo,cv::Size(frameWidth,frameHeight),1);
        
    Point offsetROI ;
    
    std::vector<std::pair<int,Rect>> candidatesFrame ;
    //for(int i = 0 ; i < filenames.size() ; ++i){
        int frameId = 0 ;
      
        int imageHeight = frameHeight ;
        int imageWidth = frameWidth ;
        lane_offset = 4 ;
        
        /* Create lane properties before entering video feed loop */
        lane_coordinate lane = lane_coordinate(imageWidth,lane_offset,0.75,imageHeight-videoNames[vidId].remove_bottom,videoNames[vidId].top_lane) ;
        
        trapMask = Mat::zeros(frameHeight, frameWidth, CV_8UC1) ;
        int npts[] = {4} ;
        fillPoly( trapMask, lane.ppts , npts, 1, Scalar( 255, 255, 255 ), 8 );
        namedWindow("TRAP_MASK");
        moveWindow("TRAP_MASK", 660, 0);
        imshow("TRAP_MASK",trapMask);
        //waitKey();
        
        /* Now we need a default retangular ROI to contain the trapezoidal Mat */
        int width_roi = lane.laneBottomWidth ;
        int height_roi = lane.laneHeight ;
        offsetROI = Point(lane.bottomOffset,lane.top_lane) ; //
        Rect ROI_default(offsetROI.x,offsetROI.y,width_roi,height_roi);
        
        /* initalize size of window */
        movingWindowProperties window(videoNames[vidId].bbound_largest_ratio,videoNames[vidId].bbound_smallest_ratio ) ; // Largest box = 1/3 of width of bottom of lane
        // Smallest box = 1/2 of width of top of lane
        window.starting_bbox_size = cvRound(lane.laneBottomWidth/window.bbound_largest_ratio) ;
        window.smallest_bbox_size = cvRound(lane.laneTopWidth/window.bbound_smallest_ratio) ;
        
        /* Store canny properties in struct (stored in configuration.hpp) */
        cannyProperties canny_prop(50,150) ;
        
        /* initial size of a bounding box */
        
        int window_n_rows = window.starting_bbox_size ;
        int window_n_cols = window.starting_bbox_size ;
        std::cout << "Press 's' to start video processing\nPress 'n' to skip to the next video" << std::endl ;
        
        namedWindow("orig_frame");
        moveWindow("orig_frame", 0, nativeResolution.second-imageHeight);
        namedWindow("orig_frame2");
        moveWindow("orig_frame", 0,0);
        moveWindow("orig_frame2", 0,480);
        namedWindow("lanesROI");
        
        namedWindow("PixelDiffFrame");
        moveWindow("PixelDiffFrame", nativeResolution.first-imageWidth, 0);
        namedWindow("PixelDiffFrame_thres") ;
        moveWindow("PixelDiffFrame_thres", nativeResolution.first-imageWidth, nativeResolution.second-imageHeight);
        namedWindow("cannyEdges_w/outPixelDiff");
        moveWindow("cannyEdges_w/outPixelDiff", imageWidth + 10 , 0);
        namedWindow("Sobel");
        moveWindow("Sobel", 0, imageHeight);

        //imshow("LAUNCH_POINT",Mat::zeros(imageHeight, imageWidth, CV_8UC1));
        
//        char StartKey = waitKey() ;
//
//        if(StartKey == (int) 'n'){
//            continue;
//        }
        
        int start_s, stop_s ;
    while(frameId < numFrames ){
     
//        orig_frame = imread(pathtoData+filenames[i]+".png") ;
       
            cap >> orig_frame;
       
            cap >> orig_frame2;
        
            if(orig_frame.empty() || orig_frame2.empty())
                break ;
//        Mat orig_frame_copy = orig_frame ;
//
        imshow("orig_frame", orig_frame);
       




        if(frameId % 3 != 0){
            std::cout << "Skip frame " << frameId << std::endl ;
            int greenLevel = 255 ;
//            Mat mask = Mat::zeros(orig_frame.rows, orig_frame.cols, CV_8UC1) ;
//            Rect roi = Rect(candidatesFrame[0].second.tl()+offsetROI, candidatesFrame[0].second.br() + offsetROI) ;
//            mask(roi) = 255 ;
//            imshow("mask", mask);
//            waitKey(1);
//            orig_frame_copy.~Mat() ;
//            orig_frame.copyTo(orig_frame_copy,mask) ;
//            imshow("orig_wMask ", orig_frame_copy);
//            waitKey(1);
            for(int j = 0 ; j < 5 ; ++j){

                if (j == 0){
                    rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
                }else{


                    //std::cout << "J: " << j << " cand[j]: " << candidates[j].first << " cand[j-1] " << candidates[j].first << std::endl ;
                    
                    float ratio ;
                    if(candidatesFrame[j-1].first != 0 )
                        ratio = candidatesFrame[j].first / candidatesFrame[j-1].first ;
                    //std::cout << "ratio " << ratio << std::endl ;

                    greenLevel = greenLevel * (ratio) ;
                    rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
                }
               
            }
            videoMaker << orig_frame ;
            std::cout << "FrameId: " << frameId << " added to output video " << vidId  << std::endl ;
            frameId++ ;
            imshow("Candidates_FromPreviousDetection",orig_frame) ;
            
            //waitKey(1);
            continue ;

        //    imshow("Candidates", orig_frame) ;
            //waitKey(1) ;


            }else{
        
        cvtColor(orig_frame, gray_orig_frame, COLOR_BGR2GRAY);
        cvtColor(orig_frame2, gray_orig_frame2, COLOR_BGR2GRAY);
        
        pixelDiffFrame = cv::abs(gray_orig_frame2 - gray_orig_frame ) ;
        
        //imshow("PixelDiffFrame", pixelDiffFrame) ;
        //waitKey() ;
        threshold(pixelDiffFrame, pixelDiffFrame, 150,255, THRESH_BINARY);
       // imshow("PixelDiffFrame_thres", pixelDiffFrame) ;
        //waitKey();

        // Rescan image for candidates

            candidatesFrame.clear() ;

       // std::cout << "\n**** Reading Image at " << filenames[i] << " *******\n" << std::endl ;

            
       start_s=clock();
        // the code you wish to time goes here

//        Mat orig_frame_difference = abs(gray_orig_frame - pixelDiffFrame);
//        namedWindow("Orig_Frame_minus_pixel_difference");
//        moveWindow("Orig_Frame_minus_pixel_difference", 0, nativeResolution.second-orig_frame_difference.rows);
//        //imshow("Orig_Frame_minus_pixel_difference",orig_frame_difference) ;

       
        orig_frame.copyTo(lanes_orig_frame, trapMask) ;
                namedWindow("lanesROI");
                moveWindow("lanesROI", 660, 200);
                imshow("lanesROI", lanes_orig_frame);
                //waitKey();
        pixelDiffFrame.copyTo(lanes_pixel_diff,trapMask) ;


        lanes_orig_frame = lanes_orig_frame(ROI_default) ;
            lanes_pixel_diff = lanes_pixel_diff(ROI_default) ;

//      .

        /*Convert frame to gray*/

        cvtColor(lanes_orig_frame, gray_lanes, COLOR_BGR2GRAY) ;

        /* Apply standard Guassian Blur filter */
        
        GaussianBlur(gray_lanes, gray_lanes, Size(5,5), 0, BORDER_DEFAULT) ;

        /* calculate canny edge detection of window */
        Canny(gray_lanes,cannyEdges,canny_prop.minThreshold,canny_prop.maxThreshold) ;
//       imshow("cannyEdges", cannyEdges);
        Canny(lanes_pixel_diff,lanes_pixel_diff,canny_prop.minThreshold,canny_prop.maxThreshold) ;
                namedWindow("lane_pixel_diff_w/canny");
                moveWindow("lane_pixel_diff_w/canny", imageWidth/2, imageHeight);
                imshow("lane_pixel_diff_w/canny", lanes_pixel_diff);
                
        /* Subtract object moving rapidly between frames (i.e. stationary objects) from the cannyEdges */
                
        cannyEdges = abs(cannyEdges-lanes_pixel_diff) ;
        
//        imshow("cannyEdges_w/outPixelDiff", cannyEdges) ;
    
        String path_grady = "/Users/josephlefebvre/Honours_Project/videoTest/video1/Sobel_grad_y" ;
         /* Calculate sobel gradients of frame */
        Sobel( gray_lanes, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
        Sobel( gray_lanes, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
                namedWindow("SOBEL");
                moveWindow("SOBEL", 0, imageHeight);
        setMouseCallback("SOBEL", my_mouse_callback_Sobel,(void*)& grad_y);
        convertScaleAbs( grad_x, grad_x );
        convertScaleAbs( grad_y, grad_y );
                imshow("SOBEL", grad_y);
               // imshow("lanes_orig", lanes_orig_frame);
        //imwrite(path_grady+"grad_y.png", grad_y);
        addWeighted( grad_x, 0.5, grad_y, 0.5, 0, gradients_sobel ) ;
            
        //imshow("Sobel", gradients_sobel) ;
                
        

        std::pair<int,Rect> scoreCandidate ;
        std::pair<int,Rect>  bestofthree [3] ;
        

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
                /* Increase candidate score if they posses a strong horizonatal line */
                
                
                std::vector<Vec4i> lines ;
                std::vector<Vec4i> candidateLines ;
                
                generateHorizontalLines(lines, candidateLines, grad_y,5);
                
            //waitKey(1) ; // Visual data while it is processed
                int row_save = 0 ;
                char controlKey ;
        for (int col = 0 ; col  <= lane.laneBottomWidth - window_n_cols; col += window.stepSlide){
            //std::cout << "outer loop" << std::endl ;
            int counter_bbox = 0 ;
            int max_among3 = 0 ;
            for (int row = lane.laneHeight ; row >= window.stepSlide ; row -= window.stepSlide){
                if(controlKey == 3 || controlKey == 2){
                    row = row_save ;
                }
            
                // std::cout << "inner loop" << std::endl ;
                Rect bboxWindow(col,row,window_n_cols,window_n_rows) ;
                
                if(trapMask.at<uchar>(bboxWindow.tl()+offsetROI) == 0 || trapMask.at<uchar>(Point(bboxWindow.tl().x+window_n_cols,bboxWindow.tl().y)+offsetROI) == 0)
                    break ;
                
                Rect cannyWindow(col-window.bufferRegion,row-window.bufferRegion,window_n_cols+window.bufferRegion*2,
                                 window_n_rows+window.bufferRegion*2) ;

                if(row > lane.laneHeight - (window_n_rows + bufferRegion))
                    cannyWindow.height = lane.laneHeight - (row-bufferRegion) ;
                else if (row < window_n_rows + bufferRegion)
                    cannyWindow.y = 0 ;

                if(col > lane.laneBottomWidth - (window_n_cols + bufferRegion*2)){
                    cannyWindow.width = lane.laneBottomWidth - (col-bufferRegion) ;
                }else if (col < window_n_rows + bufferRegion)
                    cannyWindow.x = 0 ;

                 canny_of_window = cannyEdges(cannyWindow) ;
                
                scoreIndivWindow = objectness(canny_of_window,gradients_sobel, bboxWindow, Point(cannyWindow.x,cannyWindow.y), lanes_orig_frame,lanes_pixel_diff) ;
                
                if(scoreIndivWindow == 0 ){
                    window_n_cols  =  m*(row - window.stepSlide) + window.smallest_bbox_size ;
                    window_n_rows = m*(row - window.stepSlide) + window.smallest_bbox_size ;
                    continue ;
                }
            
                // if object score is greater than the average score so far, it has a change to be a top candidate
                // therefore check if the candidate contains any strong horizontal lines
               // if(scoreIndivWindow > totalScoreAllCandidates/(candidatesFrame.size()+1)) //
                   scoreIndivWindow += containsHorizontalLine(candidateLines, bboxWindow, Point(bboxWindow.x,bboxWindow.y), grad_y) ;
                
                
                
                scoreCandidate = std::pair<int,Rect>(scoreIndivWindow,bboxWindow) ;

                bestofthree[counter_bbox] = scoreCandidate ;

                if(scoreIndivWindow > bestofthree[max_among3].first)
                    max_among3 = counter_bbox ;
                counter_bbox++;
                
                if (counter_bbox == 3){
                    // reset counter which counts every third candidate checked.
                    counter_bbox = 0 ;
                    if( (bestofthree[max_among3].first != 0) &&
                      (bestofthree[max_among3].first > totalScoreAllCandidates/(candidatesFrame.size()+1) )){
                        bestofthree[max_among3].first += containsHorizontalLine(candidateLines, bboxWindow, Point(bboxWindow.x,bboxWindow.y), gradients_sobel) ;
                        candidatesFrame.push_back(bestofthree[max_among3]);
                        totalScoreAllCandidates += bestofthree[max_among3].first ;
                    }
                    max_among3 = 0 ;
                }
                /* Calculation of a box size, now we must decrease the size of the ROI
                 as we move up the column (i.e. the lane ) of the frame */
                window_n_cols  =  m*(row - window.stepSlide) + window.smallest_bbox_size ;
                window_n_rows = m*(row - window.stepSlide) + window.smallest_bbox_size ;
                
                //controlKey = waitKey(); // MANUAL_CONTROL OF BOX
                
                if(controlKey == 3){
                    row_save = row ;
                    col += window.stepSlide ;
                }
                else if (controlKey == 2){
                    col -= window.stepSlide ;
                    row_save = row ;
                }
                else if (controlKey == 1){
                    if( row + window.stepSlide*2 < lane.laneBottomWidth){
                        row += window.stepSlide*2 ;
                    }
                }else if (controlKey == (int) 'n'){
                    if(controlKey == 110){
                        row = lane.laneBottomWidth - window_n_rows ;
                        col = lane.laneHeight - window_n_cols ;
                        break;
                    }
                }

            }
            /* OUTER For loop */
            if (controlKey == 2){
                col -= window.stepSlide*2 ;
            }
            window_n_cols = window.starting_bbox_size ;
            window_n_rows = window.starting_bbox_size ;

        }
//        /* End of candidate generation */

        if(candidatesFrame.empty()){
            std::cout << "No candidate detected, noisy imaage" << std::endl ;
            continue ;
        }

//        for(int i = 0 ; i < candidates.size(); ++i){
//
//            std::cout  << "Candidate#: " << i << " br of candidate and score (" <<  candidates[i].first << ","
//            << candidates[i].second.br() << ")" << "\n"  ;
//        }
        std::cout << "\t**candidates size = " << candidatesFrame.size() << std::endl ;
       
        std::sort(candidatesFrame.begin(),candidatesFrame.end()
                  ,comp );

   
//        for(int j = 0 ; j < 5 ; ++j){
//
//            if (j == 0){
//                rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
//                putText(orig_frame, std::to_string(candidatesFrame[j].first), candidatesFrame[j].second.br()+offsetROI, FONT_HERSHEY_PLAIN, 1, Scalar(255,0,0)) ;
//            }else{
//
//
//                //std::cout << "J: " << j << " cand[j]: " << candidates[j].first << " cand[j-1] " << candidates[j].first << std::endl ;
//                float ratio =  candidatesFrame[j].first / candidatesFrame[j-1].first ;
//                //std::cout << "ratio " << ratio << std::endl ;
//
//                greenLevel = greenLevel * (ratio) ;
//                rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
//            }
//        }
//                imshow("Candidates_With_Simultaneous_line_Detection", orig_frame);
                
                

             
              /* Increase candidate score if they posses a strong horizonatal line */
                


//                                putText(roiBGR, "Point1: ", Point(l[0], l[1]),  FONT_ITALIC, 0.25, Scalar(255,0,0));
//                                putText(roiBGR, "Point2: ", Point(l[2], l[3]), FONT_ITALIC, 0.25, Scalar(0,255,0));
//                    line( roiBGR, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, AA);
//                    //Red lines are all lines detected without maximum filtering
////                    line( roiBGR, Point(l[0]+offsetROI.x, l[1]+offsetROI.y),
////                         Point(l[2] +offsetROI.x, l[3]+offsetROI.y), Scalar(0,0,255), 3, AA);
//                    imshow("Sobel", roiBGR);
//                    waitKey();

                
        

//                for(int j = 0 ; j < candidatesFrame.size() ; ++j){
//                    for(size_t i =0 ; i < candidateLines.size() ; ++i){
//                        Vec4i l = candidateLines[i] ;
//                        //Draw green lines representing lines filtered out by maximum size
//                        Point lineA(l[0], l[1]);
//                        Point lineB(l[2], l[3]);
//                        line( roiBGR, lineA,lineB, Scalar(0,255,0), 1, AA);
//                        imshow("orig_frame_lines",roiBGR);
//                        waitKey(5);
//                        if(candidatesFrame[j].second.contains(lineA) &&candidatesFrame[j].second.contains(lineB)){
//                            std::cout << "increasing score of " << j << std::endl ;
//                            candidatesFrame[j].first += 100 ;
//
//                        }
//
//                    }
//                }
        
            }
        
            int greenLevel = 255 ;
                //rstd::sort(candidatesFrame.begin(),candidatesFrame.end(),comp);
                for(int j = 0 ; j < 5 ; ++j){

                    if (j == 0){

                        rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
                        putText(orig_frame, std::to_string(candidatesFrame[j].first), candidatesFrame[j].second.br()+offsetROI, FONT_HERSHEY_PLAIN, 3, Scalar(255,0,0)) ;
                    }else{


                        //std::cout << "J: " << j << " cand[j]: " << candidates[j].first << " cand[j-1] " << candidates[j].first << std::endl ;
                        float ratio ;
                        if(candidatesFrame[j-1].first != 0)
                            ratio =  candidatesFrame[j].first / candidatesFrame[j-1].first ;
                        
                        //std::cout << "ratio " << ratio << std::endl ;

                        greenLevel = greenLevel * (ratio) ;
                        rectangle(orig_frame, candidatesFrame[j].second.tl()+offsetROI,candidatesFrame[j].second.br()+offsetROI, Scalar(0,greenLevel,0),1) ;
                    }
                }
                 stop_s=clock();
         objectness(cannyEdges, gradients_sobel, candidatesFrame[0].second, Point(0,0), lanes_orig_frame, lanes_pixel_diff) ;
                imshow("Candidates_afterHORIZONAL", orig_frame);
        
                prevFrameDetection = candidatesFrame[0].second ;
        

        std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;
        std::cout << "\n/***** End of Processing ****/ " << std::endl ;
                
               // imshow("orig_frame2", lanes_orig_frame);
         //imshow("Candidates", orig_frame) ;
         ;

        

        
       
            videoMaker << orig_frame ;
            std::cout << "FrameId: " << frameId << " added to output video " << vidId  << std::endl ;
//            if(frameId % 5 == 0 )
//                waitKey() ;
            frameId++;
            if( waitKey(1) == 110 ){
                frameId = numFrames;
                vidId++;
            }
         destroyAllWindows() ;
    }
        
        destroyAllWindows() ; 
        }
        
    }
    
    
    
    

    
    
    
    
    

