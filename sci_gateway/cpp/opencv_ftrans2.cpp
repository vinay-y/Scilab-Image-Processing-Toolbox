/********************************************************
Author: Vinay

Function: ind2gray(image, colormap)
********************************************************/

#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
extern "C"
{
  #include "api_scilab.h"
  #include "Scierror.h"
  #include "BOOL.h"
  #include <localization.h>
  #include "sciprint.h"
  #include "../common.h"


  void rotate180(Mat &m) {
    double temp;
    for (int i=0; i<(m.rows+1)/2; i++) {
        int k = m.cols;
        if ((i+1)>=((m.rows+1)/2)) {
            k = (m.cols+1)/2;
        }
        for (int j=0; j<k; j++) {
            temp = m.at<double>(i, j);
            m.at<double>(i, j) = m.at<double>(m.rows-i-1, m.cols-j-1);
            m.at<double>(m.rows-i-1, m.cols-j-1) = temp;
        }
    }

  }  

  Mat fftshif(Mat m) {
    int a = m.rows/2;
    int b = m.cols/2;
    Mat r = Mat::zeros(m.size(), m.type());
    for (int i=0; i<m.rows; i++) {
        for (int j=0; j<m.cols; j++) {
            r.at<double>((i+a)%m.rows, (j+b)%m.cols) = m.at<double>(i, j);
        }
    }
    return r;
  }

  int opencv_ftrans2(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int iRows=0,iCols=0;
    int cRows=0,cCols=0;
    int *piAddr = NULL;
    int *piAddrNew = NULL;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1) ;

    // Mat image, imgcpy;
    // retrieveImage(image, 1);
    // string tempstring = type2str(image.type());
    // char *imtype;
    // imtype = (char *)malloc(tempstring.size() + 1);
    // memcpy(imtype, tempstring.c_str(), tempstring.size() + 1);
    // bool integer = true;
    // int scale = 1;
    // double error = 0;
    Mat b, bcpy;
    Mat P0, P1, P2, h, hh;

    if (nbInputArgument(pvApiCtx) == 2) {
        
        retrieveImage(bcpy, 1);
        bcpy.convertTo(b, CV_64F);
        rotate180(b);

        // for(int i=0;i<b.rows;i++)
        // {
        //     for(int j=0;j<b.cols;j++)
        //     {
        //         cout<<b.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;
        fftshif(b).copyTo(b);

        // for(int i=0;i<b.rows;i++)
        // {
        //     for(int j=0;j<b.cols;j++)
        //     {
        //         cout<<b.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;
        rotate180(b);

        // for(int i=0;i<b.rows;i++)
        // {
        //     for(int j=0;j<b.cols;j++)
        //     {
        //         cout<<b.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;


        Mat t, tcpy;
        retrieveImage(tcpy, 2);
        tcpy.convertTo(t, CV_64F);

        int inset1 = (t.rows-1)/2;
        int inset2 = (t.cols-1)/2;
        int n = (b.cols-1)/2;

        Mat a = Mat::zeros(1, n+1, CV_64F);

        a.at<double>(0, 0) = b.at<double>(0,0);
        for (int i=1; i<=n; i++) {
            a.at<double>(0, i) = 2*b.at<double>(0, i);
        }

        // for(int i=0;i<a.rows;i++)
        // {
        //     for(int j=0;j<a.cols;j++)
        //     {
        //         cout<<a.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;

        P0 = Mat::ones(1, 1, CV_64F);
        t.copyTo(P1);
        h = (P1*a.at<double>(0, 1));

        h.at<double>(inset1,inset2) = h.at<double>(inset1,inset2) + a.at<double>(0, 0);

        // for(int i=0;i<h.rows;i++)
        // {
        //     for(int j=0;j<h.cols;j++)
        //     {
        //         cout<<h.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }
        // cout<<endl;

        for (int i=2; i<=n; i++) {
            Mat src;
            int additionalRows = P1.rows-1, additionalCols = P1.cols-1;
            copyMakeBorder(t, src, (additionalRows+1)/2, additionalRows/2, (additionalCols+1)/2, additionalCols/2, BORDER_CONSTANT, Scalar(0));
            Point anchor(P1.cols - P1.cols/2 - 1, P1.rows - P1.rows/2 - 1);
            int borderMode = BORDER_CONSTANT;
            filter2D(src, P2, t.depth(), P1, anchor, 0, borderMode);

            P2 = P2 * 2;

            for (int x=0; x<P0.rows; x++) {
                for (int y=0; y<P0.cols; y++) {
                    P2.at<double>(x + 2*inset1, y+ 2*inset2) = P2.at<double>(x + 2*inset1, y+ 2*inset2) - P0.at<double>(x, y);
                }
            }

            h.copyTo(hh);
            h = (P2*a.at<double>(0, i));

            for (int x=0; x<P1.rows; x++) {
                for (int y=0; y<P1.cols; y++) {
                    h.at<double>(x + inset1, y + inset2) = h.at<double>(x + inset1, y + inset2) + hh.at<double>(x, y);
                }
            }

            P1.copyTo(P0);
            P2.copyTo(P1);

            for(int i=0;i<P1.rows;i++)
        {
            for(int j=0;j<P1.cols;j++)
            {
                cout<<P1.at<double>(i, j)<<"\t";
            }
            cout<<endl;
        }
        cout<<endl;

        }

        for(int i=0;i<h.rows;i++)
        {
            for(int j=0;j<h.cols;j++)
            {
                cout<<h.at<double>(i, j)<<"\t";
            }
            cout<<endl;
        }
        cout<<endl;

        rotate180(h);
    }

        for(int i=0;i<h.rows;i++)
        {
            for(int j=0;j<h.cols;j++)
            {
                cout<<h.at<double>(i, j)<<"\t";
            }
            cout<<endl;
        }
        cout<<endl;


        // Mat source = img;
        // if(CONVOLUTION_FULL == type) {
        //   source = Mat();
        //   const int additionalRows = kernel.rows-1, additionalCols = kernel.cols-1;
        //   copyMakeBorder(img, source, (additionalRows+1)/2, additionalRows/2, (additionalCols+1)/2, additionalCols/2, BORDER_CONSTANT, Scalar(0));
        // }
         
        // Point anchor(kernel.cols - kernel.cols/2 - 1, kernel.rows - kernel.rows/2 - 1);
        // int borderMode = BORDER_CONSTANT;
        // filter2D(source, dest, img.depth(), flip(kernel), anchor, 0, borderMode);

    Mat hcopy;
    h.copyTo(hcopy);

    string tempstring = type2str(hcopy.type());
    char *imtype;
    imtype = (char *)malloc(tempstring.size() + 1);
    memcpy(imtype, tempstring.c_str(), tempstring.size() + 1);
    returnImage(imtype,hcopy,1);
    free(imtype);


    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;
    //Returning the Output Variables as arguments to the Scilab environment
    ReturnArguments(pvApiCtx);            
    return 0;

  }
/* ==================================================================== */
}

