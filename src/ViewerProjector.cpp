/************************************************************************
 * Copyright (C) 2017 Richard Palmer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

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
