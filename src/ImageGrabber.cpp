#include "ImageGrabber.h"
#include <VtkTools.h>       // RVTK
#include <FeatureUtils.h>   // RFeatures
using RVTK::ImageGrabber;


// public
ImageGrabber::ImageGrabber( const vtkRenderWindow* rw) : _renWin(rw)
{
}   // end ctor


// public
void ImageGrabber::update( int reqHeight)
{
    if ( reqHeight <= 0)
        reqHeight = const_cast<vtkRenderWindow*>(_renWin)->GetSize()[1];
    // Get the raw input images at their view size
    const cv::Mat_<cv::Vec3b> cimg = RVTK::extractImage( _renWin);
    const cv::Mat_<float> dimg = RVTK::extractZBuffer( _renWin);

    const cv::Size REQSZ( cvRound( reqHeight * double(cimg.cols)/cimg.rows), reqHeight);
    cv::resize( dimg, _dzmap, REQSZ); // Resize depth image to required dims
    cv::resize( cimg, _colmap, REQSZ); // Resize colour image to required dims
    // Get the contrast stretched CIE-L component
    _dcmap = RFeatures::contrastStretch( RFeatures::getLightness( _colmap, 255, CV_8U));

    // Make a depth byte map from the reduced size z-buffer map
    double mn, mx;
    cv::minMaxLoc( _dzmap, &mn, &mx);
    cv::Mat_<float> rngMap = _dzmap.clone();
    rngMap -= float(mn);   // Make nearest point 0
    const float depthProp = 1.0;    // Proportion of depth buffer to use (also try 1/2)
    rngMap.convertTo( _ddmap, CV_8U, -255./((mx-mn)*depthProp), 255);
}   // end update
