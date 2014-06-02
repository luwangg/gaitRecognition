/* -*- c-basic-offset: 4 -*-
 */

/**
 * @author Reynald Pereira <KS10928@kingston.ac.uk>
 * @version 0.0.1 - Tuesday May 20 2014
 *
 * @todo create the class 
 * @bug check if simple Corner pointer is enough
 */

/**
 * @file tacker.cpp 
 *
 * algorithme finding corners points in a picture/image/frame using Shi and Tomasi Algorithme or Harris' one
 */

#include <iostream>
#include "tracker.h"

Tracker::Tracker()
{
	// exemple provided in openCV book

	int cornerCount = GR_CORNER_NUMBER;

	_imageArray = new cv::Mat[GR_IMAGE_NUMBER];
	_corners = new Corner[GR_CORNER_NUMBER];

	//_imageArray[GR_INPUT_IMAGE] = cv::imread("./data/image0.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	//_imageArray[GR_INPUTB_IMAGE] = cv::imread("./data/image1.jpg",CV_LOAD_IMAGE_GRAYSCALE);
	_imageArray[GR_MASK_TRACK_IMAGE] = cv::Mat();

	_cornerFinder = new CornerFinder(_imageArray,_corners,cornerCount);
	_cornerPrecizer = new CornerPrecizer(_imageArray,_corners);
	_lukasKanade =  new LukasKanade(_imageArray);
	_pyrLK = new PyrLukasKanade(_imageArray,_corners,cornerCount);
	_hornSchunck = new HornSchunck(_imageArray);

	_init = true;

}

Tracker::~Tracker() 
{
}

void Tracker::runAlgos()
{
 	int color;
 	cv::Mat tmp,alpha,dst;
 	if ( _init ) 
 	{
 		_cornerFinder->perform();
		_cornerPrecizer->perform();
		_init = false;
	}
	_pyrLK->perform();
	_outCorners = _pyrLK->getOutCorners();



	
	cvtColor(_imageArray[GR_INPUT_IMAGE], _imageArray[GR_INPUT_IMAGE], CV_GRAY2BGR);

  	for( size_t i = 0; i < _corners->size(); i++ )
    { 
    	color = (((*_corners)[i].x - (*_outCorners)[i].x)*255/_imageArray[GR_OUTPUT_IMAGE].size().width + ((*_corners)[i].y - (*_outCorners)[i].y)*255/_imageArray[GR_OUTPUT_IMAGE].size().height)/2 ;
    	//std::cout<<"Color found : "<<color<<std::endl;
    	if ( color < 0 ) 
    	{
    		color = -color;
    	}
    	if ( color > 45 && color < 85 )
    	{
 			cv::circle( _imageArray[GR_OUTPUT_IMAGE], (*_corners)[i], 1, cv::Scalar(85,0,0), -1, 8, 0 );
    	}
    	else if ( color > 85 && color < 170 )
    	{
 			cv::circle( _imageArray[GR_OUTPUT_IMAGE], (*_corners)[i], 1, cv::Scalar(0,color,color), -1, 8, 0 );
    	}
    	else  if ( color > 170 && color < 255 )
    	{
 			cv::circle( _imageArray[GR_OUTPUT_IMAGE], (*_corners)[i], 1, cv::Scalar(0,0,color), -1, 8, 0 );
    	}
    	//cv::circle( _imageArray[GR_INPUT_IMAGE], (*_corners)[i], 2, cv::Scalar(128,128,128), -1, 8, 0 );
    	cv::line( _imageArray[GR_INPUT_IMAGE], (*_corners)[i],(*_outCorners)[i],cv::Scalar(128,128,128),1,1,0);

    }
	cvtColor(_imageArray[GR_OUTPUT_IMAGE],tmp,CV_BGR2GRAY);
	cv::threshold(tmp,alpha,100,255,cv::THRESH_BINARY);
	cv::Mat rgb[3];
	cv::split(_imageArray[GR_OUTPUT_IMAGE],rgb);
	cv::Mat rgba[4]={rgb[0],rgb[1],rgb[2],alpha};
	cv::merge(rgba,4,dst);

	/*for (int i = 0 ;i < _imageArray[GR_OUTPUT_IMAGE].size().width;i++ ) 
	{
		for (int j =0; j< _imageArray[GR_OUTPUT_IMAGE].size().height;j++ )
		{
			if ( _imageArray[GR_OUTPUT_IMAGE].at<cv::Vec3b>(i,j) != *(new cv::Vec3b(0,0,0)))
			{
				_imageArray[GR_INPUT_IMAGE].at<cv::Vec3b>(i,j) = *(new cv::Vec3b(_imageArray[GR_OUTPUT_IMAGE].at<cv::Vec3b>(i,j)));
			}
		}
	}*/
   	//_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE] = *(new cv::Mat(_imageArray[GR_OUTPUT_IMAGE].size(),8,1));
	//cv::inRange(_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE], cv::Scalar(125.0, 0.0, 0.0), cv::Scalar(255.0 + 1.0, 130.0, 130.0), _imageArray[GR_MASK_OPTICAL_FLOW_IMAGE]);
	//cv::bitwise_not(_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE],_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE]);
	//_imageArray[GR_OUTPUT_IMAGE].copyTo(_imageArray[GR_INPUT_IMAGE],_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE]);
	//cv::applyColorMap(_imageArray[GR_OUTPUT_IMAGE], _imageArray[GR_INPUT_IMAGE], cv::COLORMAP_AUTUMN);

	/*cv::cvtColor(_imageArray[GR_OUTPUT_IMAGE],_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE] ,CV_BGR2GRAY);
	cv::threshold(_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE],_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE],10,1,cv::THRESH_BINARY_INV);
	_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE].reshape(3);

	_imageArray[GR_INPUT_IMAGE].mul(_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE]);
	//_imageArray[GR_OUTPUT_IMAGE].copyTo(_imageArray[GR_INPUT_IMAGE],_imageArray[GR_MASK_OPTICAL_FLOW_IMAGE]);
	*/

	cv::imwrite("dst.png",_imageArray[GR_INPUT_IMAGE]);
	/*cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", _imageArray[GR_INPUT_IMAGE] ); 
    cv::namedWindow( "HeatMap", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "HeatMap", _imageArray[GR_OUTPUT_IMAGE] ); */


	
}

void Tracker::setInputImage1(cv::Mat img)
{
	_imageArray[GR_INPUT_IMAGE] = cv::Mat(img);
}

void Tracker::setInputImage2(cv::Mat img)
{
	_imageArray[GR_INPUTB_IMAGE] = cv::Mat(img);
}

void Tracker::setHeatMapImage(cv::Mat img)
{
	_imageArray[GR_OUTPUT_IMAGE] = cv::Mat(img.size(),CV_8UC4,cv::Scalar(0,0,0));
}

void Tracker::reInit()
{
	_init = true;
}

cv::Mat* Tracker::getOutputFrame()
{
	return new cv::Mat(_imageArray[GR_INPUT_IMAGE]);
}

Corner* Tracker::getCorners()
{
	return _corners;
}

Corner* Tracker::getOutCorners()
{
	return _outCorners;
}

void Tracker::reallocCorners()
{	
    _corners = _outCorners;
}