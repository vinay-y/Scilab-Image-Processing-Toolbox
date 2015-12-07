/********************************************************
Author: Vinay
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

  int opencv_ind2gray(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int iRows=0,iCols=0;
    int pirows=0,picols=0;
    int *piAddr = NULL;
    int *piAddrNew = NULL;
    int *piAddr2  = NULL;
    //unsigned short int *image = NULL;
    double *map = NULL;
    int i,j,k;
    double x, y, width, height;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1) ;

    Mat image;
    retrieveImage(image, 1);
    iRows = image.rows;
    iCols = image.cols;

    Mat cmap;
    retrieveImage(cmap, 2);


    unsigned char *r;
    r=(unsigned char *)malloc(sizeof(unsigned char)*iRows*iCols);
    int m = 0;


    for (int i=0; i<iRows; i++) {
        for (int j=0; j<iCols; j++) {
            int temp = image.at<uchar>(i, j);
            r[i + iRows*j] = (unsigned char)((0.2989 * cmap.at<double>(temp, 0) + 0.5870 * cmap.at<double>(temp, 1) + 0.1140 * cmap.at<double>(temp, 2))*255 + 0.5) ;

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
    sciErr = createMatrixOfUnsignedInteger8InList(pvApiCtx, nbInputArgument(pvApiCtx)+1 , piAddrNew, 1,iRows, iCols, r);
    free(r);
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
