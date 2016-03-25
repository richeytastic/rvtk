#include "ViewerProjector.h"
using RVTK::ViewerProjector;
#include <algorithm>
#include <iostream>


ViewerProjector::ViewerProjector( const RVTK::Viewer::Ptr viewer) : _viewer(viewer)
{}   // end ctor


cv::Mat_<cv::Vec3b> ViewerProjector::extractImage() const
{
    return _viewer->extractImage();
}   // end extractImage



cv::Mat ViewerProjector::makeRangeMap( float depthProp, int colourMap) const
{
    cv::Mat_<float> rngMap = _viewer->extractZBuffer();

    double mn, mx;
    cv::minMaxLoc( rngMap, &mn, &mx);
    rngMap -= float(mn);   // Make nearest point 0
    
    cv::Mat img;
    rngMap.convertTo( img, CV_8U, -255./((mx-mn)*depthProp), 255);

    if ( colourMap < 0)
        return img;

    cv::Mat outMap;
    cv::applyColorMap( img, outMap, colourMap);
    return outMap;
}   // end makeRangeMap
