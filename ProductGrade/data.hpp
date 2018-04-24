//
//  data.hpp
//  RefiningEdgeBoxDetection
//
//  Created by Joseph lefebvre on 2018-03-27.
//  Copyright © 2018 Joseph lefebvre. All rights reserved.
//

#ifndef data_h
#define data_h

struct videoConfigProp{
    
    cv::String name ;
    float bbound_largest_ratio  ;
    float bbound_smallest_ratio  ;
    int top_lane ; // chop of some sky
    int remove_bottom ; // chops off the text and any protruding parts of the vehicle 
    explicit videoConfigProp(cv::String n ,float largest, float smallest, int top=25, int bottom=100): name(n),bbound_largest_ratio(largest),bbound_smallest_ratio(smallest),top_lane(top),remove_bottom(bottom){}
};


std::vector<cv::String> filenames = {"MIN50__1000000",
    "MIN50__1000012",
    "MIN50__1000044",
    "MIN50__1000032",
    "MIN50__1000041",
    "MIN50__1000001",
    "MIN50__1000002",
    "MIN50__1000003",
    "MIN50__1000004",
    "MIN50__1000005",
    "MIN50__1000006",
    "MIN50__1000007",
    "MIN50__1000008",
    "MIN50__1000009",
    "MIN50__1000010",
    "MIN50__1000011",
    "MIN50__1000013",
    "MIN50__1000014",
    "MIN50__1000015",
    "MIN50__1000016",
    "MIN50__1000017",
    "MIN50__1000018",
    "MIN50__1000019",
    "MIN50__1000020",
    "MIN50__1000021",
    "MIN50__1000022",
    "MIN50__1000023",
    "MIN50__1000024",
    "MIN50__1000025",
    "MIN50__1000026",
    "MIN50__1000027",
    "MIN50__1000028",
    "MIN50__1000029",
    "MIN50__1000030",
    "MIN50__1000031",
    "MIN50__1000033",
    "MIN50__1000034",
    "MIN50__1000035",
    "MIN50__1000036",
    "MIN50__1000037",
    "MIN50__1000038",
    "MIN50__1000039",
    "MIN50__1000040",
    "MIN50__1000042",
    "MIN50__1000043",
    "MIN50__1000045",
    "MIN50__1000046",
    "MIN50__1000047",
    "MIN50__1000048",
    "MIN50__1000049",
    "MIN50__1000050",
    "MIN50__1000051",
    "MIN50__1000052",
    "MIN50__1000053",
    "MIN50__1000054",
    "MIN50__1000055",
    "MIN50__1000056",
    "MIN50__1000057",
    "MIN50__1000058",
    "MIN50__1000059",
    "MIN50__1000060",
    "MIN50__1000061",
    "MIN50__1000062",
    "MIN50__1000063",
    "MIN50__1000064",
    "MIN50__1000065",
    "MIN50__1000066",
    "MIN50__1000067",
    "MIN50__1000068",
    "MIN50__1000069",
    "MIN50__1000070",
    "MIN50__1000071",
    "MIN50__1000072",
    "MIN50__1000073",
    "MIN50__1000074",
    "MIN50__1000075",
    "MIN50__1000076",
    "MIN50__1000077",
    "MIN50__1000078",
    "MIN50__1000079",
    "MIN50__1000080",
    "MIN50__1000081",
    "MIN50__1000082",
    "MIN50__1000083",
    "MIN50__1000084",
    "MIN50__1000085",
    "MIN50__1000086",
    "MIN50__1000087",
    "MIN50__1000088",
    "MIN50__1000089",
    "MIN50__1000090",
    "MIN50__1000091",
    "MIN50__1000092",
    "MIN50__1000093",
    "MIN50__1000094",
    "MIN50__1000095",
    "MIN50__1000096",
    "MIN50__1000097",
    "MIN50__1000098",
    "MIN50__1000099"} ;

//std::vector<cv::String> filenames = {"MIN50__1000044",
//    "MIN50__1000044",
//    "MIN50__1000012",
//    "MIN50__1000012",
//    "MIN50__1000012",
//    "MIN50__1000012",
//    "MIN50__1000012",
//    "MIN50__1000012",
//    "MIN50__1000001"
//} ;

std::vector<videoConfigProp> videoNames = { videoConfigProp("video6",3,4,100,140),
    videoConfigProp("video4",3,2,100),
    videoConfigProp("video2",1.8,3,100,125),
    videoConfigProp("video3",1.8,2,100,125),
    videoConfigProp("video10",2,2,25,125),
    videoConfigProp("video1",3,4,100),
   
    videoConfigProp("video7",3,4,100),
    videoConfigProp("video8",3,4,100),
    videoConfigProp("video9",3,4,100),
    videoConfigProp("video5",3,4,100),
    videoConfigProp("video11",3,4,100),
    videoConfigProp("video12",3,4,100),
    videoConfigProp("video13",3,4,100),
    videoConfigProp("video14",3,4,100)};


#endif /* data_h */

