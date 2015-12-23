/********************************************************
Author: Vinay

Function: ind2gray(image, colormap)
********************************************************/

#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
using namespace cv;
using namespace std;

#define pi 3.14159265358979323846

void rot180(Mat &m) {
    double temp;
    for (int i=0; i<(m.rows+1)/2; i++) {
        int k = m.cols;
        if ((i+1)>=((m.rows+1)/2) && m.rows%2!=0) {
            k = (m.cols+1)/2;
        }
        for (int j=0; j<k; j++) {
            temp = m.at<double>(i, j);
            m.at<double>(i, j) = m.at<double>(m.rows-i-1, m.cols-j-1);
            m.at<double>(m.rows-i-1, m.cols-j-1) = temp;
        }
    }

}  

Mat ffshift(Mat m) {
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

void meshgrid(Mat &a, Mat &b) {
    if (a.rows!=1) {
        transpose(a, a);
    }
    if (b.cols!=1) {
        transpose(b, b);
    }
    Mat c = Mat::ones(1, a.cols, CV_64F);
    Mat d = Mat::ones(b.rows, 1, CV_64F);
    cout<<"pain1"<<endl;
    a = d * a;
    cout<<"pain2"<<endl;
    b = b * c;
}

void makecol(Mat &src, Mat &des) {
    for (int j=0; j<src.cols; j++) {
        for (int i=0; i<src.rows; i++) {
            des.at<double>(j*src.rows + i) = src.at<double>(i, j);
        }
    }
}

extern "C"
{
  #include "api_scilab.h"
  #include "Scierror.h"
  #include "BOOL.h"
  #include <localization.h>
  #include "sciprint.h"
  #include "../common.h"


  int opencv_fsamp2(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int iRows=0,iCols=0;
    int rows=0,cols=0;
    int *piAddr = NULL;
    int *piAddr2 = NULL;
    int *piAddrNew = NULL;
    double *re, *im;

    //checking input argument
    CheckInputArgument(pvApiCtx, 1, 4);
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

    if (nbInputArgument(pvApiCtx) == 1) {
        Mat hd, hdcpy;
        retrieveImage(hdcpy, 1);
        hdcpy.convertTo(hd, CV_64F);
        rot180(hd);

        ffshift(hd).copyTo(hd);

        rot180(hd);


        Mat padded;                            //expand input image to optimal size
        int m = getOptimalDFTSize( hd.rows );
        int n = getOptimalDFTSize( hd.cols ); // on the border add zero values
        copyMakeBorder(hd, padded, 0, m - hd.rows, 0, n - hd.cols, BORDER_CONSTANT, Scalar::all(0));

        Mat planes[] = {Mat_<double>(padded), Mat::zeros(padded.size(), CV_64F)};
        Mat complexI;
        merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

        dft(complexI, complexI, DFT_INVERSE | DFT_SCALE);            // this way the result may fit in the source matrix

        split(complexI, planes);   

        ffshift(planes[0]).copyTo(planes[0]);
        ffshift(planes[1]).copyTo(planes[1]);
        rot180(planes[0]);
        rot180(planes[1]);

        re = (double *)malloc(planes[0].rows * planes[0].cols * sizeof(double));
        im = (double *)malloc(planes[0].rows * planes[0].cols * sizeof(double));

        for(int i=0;i<planes[0].rows;i++)
        {
            for(int j=0;j<planes[0].cols;j++)
            {
                re[i+planes[0].rows*j]=planes[0].at<double>(i, j);
                //cout<<planes[0].at<double>(i, j)<<" ";
                im[i+planes[0].rows*j]=planes[1].at<double>(i, j);
            }
            //cout<<endl;
        }
        rows = planes[0].rows;
        cols = planes[0].cols;


    }
    else if (nbInputArgument(pvApiCtx)==2 || nbInputArgument(pvApiCtx)==3) {
        sciprint("1 or 4 arguments expected.");
        return 0;
    }
    else {
        Mat f1, f1cpy;
        retrieveImage(f1cpy, 1);
        f1cpy.convertTo(f1, CV_64F);

        Mat f2, f2cpy;
        retrieveImage(f2cpy, 2);
        f2cpy.convertTo(f2, CV_64F);

        Mat hd, hdcpy;
        retrieveImage(hdcpy, 3);
        hdcpy.convertTo(hd, CV_64F);

        meshgrid(f1, f2);

        cout<<"HEllO"<<endl;

        (f1*=pi);
        (f2*=pi);

        double * siz;

        sciErr = getVarAddressFromPosition(pvApiCtx,4,&piAddr2);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }
        sciErr = getMatrixOfDouble(pvApiCtx, piAddr2, &rows, &cols, &siz);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        rows = siz[0];
        cols = siz[1];

        cout<<"HEll1"<<endl;

        Mat n1 = Mat::zeros(cols, 1, CV_64F);
        Mat n2 = Mat::zeros(rows, 1, CV_64F);
        int s1, s2;
        s1 = cols/2;
        s2 = rows/2;

        for (int i = 0; i<cols; i++) {
            n1.at<double>(i, 0) = i - s1;
        }
        for (int i = 0; i<rows; i++) {
            n2.at<double>(i, 0) = i - s2;
        }

        meshgrid(n1, n2);

        cout<<"HEll2"<<endl;

        Mat hre_col = Mat::zeros(rows*cols, 1, CV_64F);
        Mat him_col = Mat::zeros(rows*cols, 1, CV_64F);
        Mat hd_col = Mat::zeros(hd.rows * hd.cols, 1, CV_64F);
        Mat f1_col = Mat::zeros(f1.rows * f1.cols, 1, CV_64F);
        Mat f2_col = Mat::zeros(f2.rows * f2.cols, 1, CV_64F);
        Mat n1_col = Mat::zeros(n1.rows * n1.cols, 1, CV_64F);
        Mat n2_col = Mat::zeros(n2.rows * n2.cols, 1, CV_64F);
        makecol(hd, hd_col);
        makecol(f1, f1_col);
        makecol(f2, f2_col);
        makecol(n1, n1_col);
        makecol(n2, n2_col);

        // for (int i=0; i<f1_col.rows; i++) {
        //     for (int j=0; j<f1_col.cols; j++) {
        //         cout<<f1_col.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }

        // for (int i=0; i<f2_col.rows; i++) {
        //     for (int j=0; j<f2_col.cols; j++) {
        //         cout<<f2_col.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }

        // for (int i=0; i<n1_col.rows; i++) {
        //     for (int j=0; j<n1_col.cols; j++) {
        //         cout<<n1_col.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }

        // for (int i=0; i<n2_col.rows; i++) {
        //     for (int j=0; j<n2_col.cols; j++) {
        //         cout<<n2_col.at<double>(i, j)<<" ";
        //     }
        //     cout<<endl;
        // }


        transpose(n1_col, n1_col);
        transpose(n2_col, n2_col);
        f1_col = (f1_col * n1_col);
        f2_col = f2_col * n2_col;
        f1_col = f1_col + f2_col;

        cout<<"HEll3"<<endl;

        Mat DFT[5];
        DFT[0] = Mat::zeros(f1_col.rows, f1_col.cols, CV_64F);
        DFT[1] = Mat::zeros(f1_col.rows, f1_col.cols, CV_64F);
        DFT[2] = Mat::zeros(f1_col.rows, f1_col.cols, CV_64F);
        DFT[3] = Mat::zeros(f1_col.rows, f1_col.cols, CV_64F);
        DFT[4] = Mat::zeros(2 * f1_col.rows, 2 * f1_col.cols, CV_64F);

        for (int i=0; i<f1_col.rows; i++) {
            for (int j=0; j<f1_col.cols; j++) {
                DFT[0].at<double>(i, j) = cos(f1_col.at<double>(i, j));
                DFT[1].at<double>(i, j) = -1 * sin(f1_col.at<double>(i, j));
            }
        }

        cout<<"HEll4"<<endl;

        for(int i=0;i<DFT[0].rows;i++) {
            for(int j=0;j<DFT[0].cols;j++) {
                DFT[4].at<double>(i,j)  =  DFT[0].at<double>(i,j);  
                DFT[4].at<double>(i,j+DFT[0].cols) = DFT[1].at<double>(i,j);
                DFT[4].at<double>(i+DFT[0].rows,j) = - DFT[1].at<double>(i,j);
                DFT[4].at<double>(i+DFT[0].rows,j+DFT[0].cols) = DFT[0].at<double>(i,j);  
            }
        }

        for (int i=0; i<DFT[4].rows; i++) {
            for (int j=0; j<DFT[4].cols; j++) {
                cout<<DFT[4].at<double>(i, j)<<" ";
            }
            cout<<endl;
        }

        DFT[4] = DFT[4].inv(DECOMP_LU);
        for(int i=0;i<DFT[0].rows;i++) {
            for(int j=0;j<DFT[0].cols;j++) {
                DFT[2].at<double>(i,j) = DFT[4].at<double>(i,j);
                DFT[3].at<double>(i,j) = DFT[4].at<double>(i,j+DFT[0].cols);
            }
        }

        for (int i=0; i<hd_col.rows; i++) {
            for (int j=0; j<hd_col.cols; j++) {
                cout<<hd_col.at<double>(i, j)<<" ";
            }
            cout<<endl;
        }

        for (int i=0; i<DFT[2].rows; i++) {
            for (int j=0; j<DFT[2].cols; j++) {
                cout<<DFT[2].at<double>(i, j)<<" ";
            }
            cout<<endl;
        }

        for (int i=0; i<DFT[3].rows; i++) {
            for (int j=0; j<DFT[3].cols; j++) {
                cout<<DFT[3].at<double>(i, j)<<" ";
            }
            cout<<endl;
        }

        hre_col = DFT[2] * hd_col;
        him_col = DFT[3] * hd_col;

        Mat h[2];
        h[0] = Mat::zeros(rows, cols, CV_64F);
        h[1] = Mat::zeros(rows, cols, CV_64F);

        for (int j=0; j<cols; j++) {
            for (int i=0; i<rows; i++) {
                h[0].at<double>(i, j) = hre_col.at<double>(j*rows + i);
                h[1].at<double>(i, j) = him_col.at<double>(j*rows + i); 
            }
        }

        rot180(h[0]);
        rot180(h[1]);

        cout<<"HEll5"<<endl;

        re = (double *)malloc(h[0].rows * h[0].cols * sizeof(double));
        im = (double *)malloc(h[0].rows * h[0].cols * sizeof(double));

        double eps = 1.5e-20;
        for(int i=0;i<h[0].rows;i++)
        {
            for(int j=0;j<h[0].cols;j++)
            {
                if (abs(h[0].at<double>(i,j)) > eps) {
                    re[i+h[0].rows*j]=h[0].at<double>(i, j);
                }
                else {
                    re[i+h[0].rows*j]=0;
                }
                //cout<<h[0].at<double>(i, j)<<" ";
                if (abs(h[1].at<double>(i,j)) > eps) {
                    im[i+h[0].rows*j]=h[1].at<double>(i, j);
                }
                else {
                    im[i+h[0].rows*j]=0;
                }
            }
            //cout<<endl;
        }

    }

        sciErr = createList(pvApiCtx, nbInputArgument(pvApiCtx) + 1, 1, &piAddrNew);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }


        //Adding the R value matrix to the list
        //Syntax : createMatrixOfInteger32InList(void* _pvCtx, int _iVar, int* _piParent, int _iItemPos, int _iRows, int _iCols, const int* _piData)
        sciErr = createComplexMatrixOfDoubleInList(pvApiCtx, nbInputArgument(pvApiCtx)+1 , piAddrNew, 1, rows, cols, re, im);
        free(re);
        free(im);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }



    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;
    //Returning the Output Variables as arguments to the Scilab environment
    ReturnArguments(pvApiCtx);            
    return 0;

  }
/* ==================================================================== */
}

