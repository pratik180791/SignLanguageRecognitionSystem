  #include<cv.h>
#include<cxcore.h>
#include<cvaux.h>
#include<highgui.h>
#include<iostream>
#include<imgproc\imgproc.hpp>
#include<tchar.h>
#include "cxmisc.h"
#include<stdio.h>

int dist_type = CV_DIST_L2;
int mask_size = CV_DIST_MASK_5;
int count_cntr1,count_cntr2;
//image declrations
	IplImage* input;
	IplImage* output;
	IplImage* sampleInput;
	
	IplImage* canny;
	IplImage* canny_sample;
	
	IplImage* ginput;
	IplImage* gsampleinput;
	
	IplImage* scale_img1;
	IplImage* scale_img2;	

	IplImage* smoothginput;
	IplImage* smoothgsampleinput;

	IplImage* tsginput;
	IplImage* tsgsampleinput;

	IplImage* temp;
	IplImage* temp1;
	
	IplImage* dimage;
	IplImage* dsampleimage;

	IplImage* deimage;
	IplImage* desampleimage;

	//integers for input data
	int height,width,step,channels;

	//loop integers
	int i,j,k;
	int cnt=0,tcnt=0,rcnt=0;

	CvScalar hsv_min = cvScalar(0,58,89);
	CvScalar hsv_max = cvScalar(25,173,220);
	CvScalar color = CV_RGB( rand()&255, rand()&255, rand()&255 );
	//CvScalar color = CV_RGB(255,0,0);

	CvSeq* firstContour = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);

	CvSeq* tempContour = 0;
	CvMemStorage* tempStorage = cvCreateMemStorage(0);

double norma;

IplImage *rotateImage(const IplImage *src, int angleDegrees)
{
	IplImage *imageRotated = cvCloneImage(src);
	if(angleDegrees!=0)
	{
		CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);

		// Compute rotation matrix
		CvPoint2D32f center = cvPoint2D32f( cvGetSize(imageRotated).width/2, cvGetSize(imageRotated).height/2 );
		cv2DRotationMatrix( center, angleDegrees, 1, rot_mat );

		// Do the transformation
		cvWarpAffine( src, imageRotated, rot_mat );
	}
	return imageRotated;
}

//dct function
IplImage* computeDistTransform(int flag)
{
	IplImage* dist = 0;
	IplImage* dist8u1 = 0;
	IplImage* dist8u2 = 0;
	IplImage* dist8u = 0;
	IplImage* dist32s = 0;
	IplImage* edge = 0;
	IplImage* gray = 0;
	if(flag == 1)
	{
		gray = cvCloneImage(ginput);
		edge = cvCloneImage(smoothginput);
	}
	else
	{
		gray = cvCloneImage(gsampleinput);
		edge = cvCloneImage(smoothgsampleinput);
	}
	dist8u1 = cvCloneImage( gray );
	dist8u2 = cvCloneImage( gray );
	dist8u = cvCreateImage( cvGetSize(gray), IPL_DEPTH_8U, 3 );
	dist32s = cvCreateImage( cvGetSize(gray), IPL_DEPTH_32S, 1 );
	cout<<"in dt:1"<<endl;
		
	dist = cvCreateImage( cvGetSize(sampleInput), IPL_DEPTH_32F, 1 );
	int edge_thresh = 50;
	int msize = mask_size;
	int _dist_type = dist_type;
	cout<<"in dt"<<endl;
	cvThreshold( gray, edge, (float)edge_thresh, (float)edge_thresh,CV_THRESH_BINARY );
	
	cvDistTransform( edge, dist, _dist_type, msize, NULL, NULL );
	cvConvertScale( dist, dist, 150.0, 0 );
	cvPow( dist, dist, 0.5 );
	cvConvertScale( dist, dist32s, 1.0, 0.5 );
	cvAndS( dist32s, cvScalarAll(255), dist32s, 0 );
	cvConvertScale( dist32s, dist8u1, 1, 0 );
	return dist8u1;
}
/*
IplImage* computing(IplImage* sampleInput)
{

}*/
int main(int argc, char** argv[])
{
	
	//sampleInput=(IplImage**)new IplImage[2];
	cvNamedWindow("input",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("cannysamp",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("tmp",CV_WINDOW_AUTOSIZE);
	CvCapture* capture = cvCaptureFromCAM(0);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,640);
	cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,480);
	temp1=cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,1);
	char* imgarr[]={"A.jpg","B.jpg"};
	/*for(int in=0;in<=1;in++)
	{
		temp=cvLoadImage(imgarr[i]);
		//cvShowImage("tmp",temp);
		//temp1=cvCreateImage(cvGetSize(temp),IPL_DEPTH_8U,1);
		cvCvtColor(temp,temp1,CV_BGR2GRAY);
		//cvShowImage("tmp",temp1);
		if(in==0)
			{
			count_cntr1=cvFindContours(temp1,storage, &firstContour, sizeof(CvContour), CV_RETR_LIST);
	        cout<<"Contour A"<<count_cntr1<<endl;
		cvShowImage("tmp",temp1);  
		}
		else
		{
			count_cntr2=cvFindContours(temp1,storage, &firstContour, sizeof(CvContour), CV_RETR_LIST);
				//cout<<"Contour A"<<count_cntr1<<endl;
		cout<<"Contour B"<<count_cntr2<<endl;
		}
	}
	*/
	for(;;)
	{
		input = cvQueryFrame(capture);
		while(cnt<2)
		{
			sampleInput = cvLoadImage(imgarr[cnt]);
			break;
		}
		//int width = input->width;
		//int height = input->height;

		//Creation of Empty images
		output =  cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,3);

		tsginput = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		tsgsampleinput = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);

		ginput = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		gsampleinput = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);

		smoothginput = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		smoothgsampleinput = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);

		//sample = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
		canny = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		canny_sample = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);
		
		dimage = cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		dsampleimage = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);
		
		deimage= cvCreateImage(cvGetSize(input),IPL_DEPTH_8U,1);
		desampleimage = cvCreateImage(cvGetSize(sampleInput),IPL_DEPTH_8U,1);

		//Transforms + other code
		cvCvtColor(input,ginput,CV_BGR2GRAY);
		cvCvtColor(sampleInput,gsampleinput,CV_BGR2GRAY);
	
		cvSmooth(ginput,smoothginput,CV_GAUSSIAN,5,5);
		cvSmooth(gsampleinput,smoothgsampleinput,CV_GAUSSIAN,5,5);

		cvThreshold(smoothginput,tsginput, 100, 255, CV_THRESH_BINARY );
		cvThreshold(smoothgsampleinput,tsgsampleinput,100,255,CV_THRESH_BINARY );

		//Edges
		cvCanny(tsginput,canny,10,10,3);
		cvCanny(tsgsampleinput,canny_sample,10,10,3);

		cvDilate(canny,dimage,NULL,5);
		cvDilate(canny_sample,dsampleimage,NULL,5);

		cvErode(dimage,deimage,NULL,2);
		cvErode(dsampleimage,desampleimage,NULL,2);
		
		cvFindContours(canny,storage, &firstContour, sizeof(CvContour), CV_RETR_LIST);
		cvFindContours(canny_sample,storage,&tempContour,sizeof(CvContour), CV_RETR_LIST);

		if(firstContour)
		{
			cvDrawContours(tsginput,firstContour,cvScalarAll(255),cvScalarAll(255), 100,5);
			cvDrawContours(tsgsampleinput,tempContour,cvScalarAll(255),cvScalarAll(255),100,5);
		}

		double compare = cvMatchShapes(canny,canny_sample,CV_CONTOURS_MATCH_I3);
		
		cout<<"b4 dt:";
		
		//calculate dist transform
		IplImage* dt1 = computeDistTransform(1);
		IplImage* dt2 = computeDistTransform(2);
		
		cout<<"after dt:";
		cvSaveImage("dt1.jpg", dt1);
		cvSaveImage("dt22.jpg", dt2);
		
		int count = 0;
		int maxcount = dt1->width * dt1->height;
		CvScalar sdt1, sdt2;

		for (int i=0; i < dt1->height; i++)
		{
			for(int j=0; j < dt1->width; j++)
			{
				sdt1 = cvGet2D(dt1, i, j);
				sdt2 = cvGet2D(dt2, i, j);
				if(abs(sdt1.val[0] - sdt2.val[0]) < 35) // threshold
					count ++;
			}
		}
		
		float ratio = (float)count/(float)maxcount;
		std :: cout << "\tcount : " << count;
		std :: cout << "\tmaxcount : " << maxcount;
		std :: cout << "\tratio : " << ratio;
		std :: cout << "\tcompare : " << compare;
		std :: cout << endl;
			
		if ( (ratio >= 0.5 && ratio <= 1.0) && (compare >= 0.0 && compare <=0.4))
		{
			tcnt=0;
			rcnt=0;
			std::cout << "\nMatch Found with DT";
			char text[255];
	//		if(==imgarr[0])
							sprintf(text, "Gesture %d",cnt);
			CvFont font;
			double hScale=2.0;
			double vScale=2.0;
			int lineWidth=2;
			
			cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,3,lineWidth);
			cvPutText (canny, text, cvPoint(20,120), &font, cvScalar(255,255,0));
			
			//rotateImage(canny_sample,0);

		}
		else
		{
			tcnt++;
			std::cout << "no match found";
			if(tcnt>5)
			{
				rotateImage(canny_sample,1);
				cvFindContours(canny_sample,storage,&tempContour,sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_CODE);
				rcnt++;
				if(rcnt>4)
				{cnt++;
				rcnt=0;}
			}
		}
		cout<<"tcnt"<<tcnt<<endl;
		cout<<"cnt"<<cnt<<endl;
		cout<<"rcnt"<<rcnt<<endl;
		cvShowImage("input",input);
		cvShowImage("Canny",canny);
		cvShowImage("cannysamp",canny_sample);
		cvShowImage("Sample",tsginput);

		cvWaitKey(50);
	}
	cvReleaseImage(&input);
	cvReleaseImage(&output);
	cvDestroyAllWindows();
	return 0;
}


