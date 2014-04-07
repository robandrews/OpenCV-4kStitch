//
//  main.cpp
//  fourKProcessing
//

#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<stdio.h>
#include<iostream>
#include<cmath>
#include<string>


using namespace cv;

int stitchAndTimeStampVideo(string videoPrefix, string videoSuffix,int);

int main(){
    
	const int numVideos = 1;	// input number of videos to process (4 quadrants = 1 video)
    
    // NOTE: 4 quadrants = 1 video
	string videoPrefix[numVideos] = {"52"};							// input filename prefix for each video (e.g., if filename is "62A_0001.mp4", enter "62")
	string videoSuffix[numVideos] = {"0033"};		// input filename suffix for each video (e.g., if filename is "62A_0001.mp4", enter "0001")
	
	int hrTimeOfDay = 10;
	int minTimeOfDay = 37;
	int secTimeOfDay =58;
	
	int minOffset = 0;
	int secOffset = 1;
	int msecOffset = 701;
    
	int timeOfDay = (secTimeOfDay*1000+hrTimeOfDay*3600*1000+minTimeOfDay*60*1000) - (msecOffset + 1000*secOffset + 60*1000*minOffset);
	
	for (int n=0;n<numVideos;n++){
		timeOfDay = stitchAndTimeStampVideo(videoPrefix[n],videoSuffix[n], timeOfDay);
	}
	
    return 0;
}

int stitchAndTimeStampVideo(string videoPrefix, string videoSuffix,int timeOfDay)
{
	string video1 = videoPrefix + "A_" + videoSuffix + ".mp4";
	string video2 = videoPrefix + "B_" + videoSuffix + ".mp4";
	string video3 = videoPrefix + "C_" + videoSuffix + ".mp4";
	string video4 = videoPrefix + "D_" + videoSuffix + ".mp4";
    
	string videoOut = videoPrefix + videoSuffix + ".avi";
    
	//Create matrix to store image
	Mat image;
	VideoCapture cap(video1);
	
	cap >> image;
	double vidFPS = cap.get(CV_CAP_PROP_FPS);
	Size imagesize = image.size();
	int numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	int imgwidth = imagesize.width;
	int imgheight = imagesize.height;
	
	//initialize stiched image container
	Mat stichedQuadrants(2*imgheight,2*imgwidth,image.type());
    
	//initialize output video
	VideoWriter out(videoOut,
                    CV_FOURCC('D','I','V','X'),vidFPS,
                    stichedQuadrants.size(),true);
	
	//create window to show image
	//namedWindow("window",CV_WINDOW_AUTOSIZE);
    
	//get quadrant caps
	VideoCapture capA(video1);
	VideoCapture capB(video2);
	VideoCapture capC(video3);
	VideoCapture capD(video4);
    
	//Define font properties
	int fontFace = FONT_HERSHEY_SIMPLEX;
	double fontScale = 2;
	int thickness = 2;
	int baseline=0;
	Point textOrg(50,50);
	char text[20];
    
	for (int n=0;n<numFrames;n++){
        
		//copy video cap to image
		capA>>image;
		image.copyTo(stichedQuadrants.colRange(0,image.cols).rowRange(0,image.rows));
		
		capB>>image;
		image.copyTo(stichedQuadrants.colRange(image.cols,stichedQuadrants.cols).rowRange(0,image.rows));
        
		capC>>image;
		image.copyTo(stichedQuadrants.colRange(0,image.cols).rowRange(image.rows,stichedQuadrants.rows));
        
		capD>>image;
		image.copyTo(stichedQuadrants.colRange(image.cols,stichedQuadrants.cols).rowRange(image.rows,stichedQuadrants.rows));
        
		// timestamp
		double video_timestamp = capA.get(CV_CAP_PROP_POS_MSEC) + timeOfDay;
		
		// Get hh:mm:ss:msec
		int hour = (int)video_timestamp/1000/3600;
		int minute = (int)video_timestamp/1000/60-hour*60;
		int second = (int)video_timestamp/1000-hour*3600-minute*60;
		int msec = (int)video_timestamp-second*1000-hour*3600*1000-minute*60*1000;
		sprintf(text,"%02d:%02d:%02d:%d",hour,minute,second,msec);
		
		Size textSize = getTextSize(text, fontFace,
                                    fontScale, thickness, &baseline);
        
		rectangle(stichedQuadrants, textOrg + Point(0, baseline),
                  textOrg + Point(textSize.width, -(textSize.height+5)),
                  Scalar(0,0,0),-1);
        
		putText(stichedQuadrants,text,textOrg,fontFace,fontScale,
                Scalar::all(255),thickness,8);
        
		//print image to screen
		//imshow("window",stichedQuadrants);
        
		//delay 33ms
		//char c = waitKey(33);
		//if( c == 27) break;
		out << stichedQuadrants;
	}
	return ((int)capA.get(CV_CAP_PROP_POS_MSEC) + timeOfDay);
    
}


