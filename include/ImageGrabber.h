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

#ifndef RVTK_IMAGE_GRABBER_H
#define RVTK_IMAGE_GRABBER_H

#include "Viewer.h"
typedef unsigned char byte;

namespace RVTK {

class rVTK_EXPORT ImageGrabber
{
public:
    // Grabs the current set of images from the passed in render window and scales to requested height.
    // If height is left as non-positive, produced images are the same size as the render window.
    ImageGrabber( vtkRenderWindow*, int reqPixelHeight=0);
    ImageGrabber( Viewer&, int reqPixelHeight=0);

    // Refresh images (only needed if render window has been updated since construction).
    void refresh( int reqPixelHeight = 0);

    cv::Size size() const { return _colmap.size();}
    cv::Mat_<cv::Vec3b> colour() const { return _colmap;}
    cv::Mat_<byte> light() const { return _dcmap;}
    cv::Mat_<float> depth() const { return _dzmap;}
    cv::Mat_<byte> depthb() const { return _ddmap;}

private:
    vtkRenderWindow* _renWin;
    cv::Mat_<cv::Vec3b> _colmap;  // Original colour map
    cv::Mat_<byte> _dcmap;   // CIE-L light map
    cv::Mat_<float> _dzmap;  // Depth map (raw z-buffer floats)
    cv::Mat_<byte> _ddmap;   // Converted depth map

    ImageGrabber( const ImageGrabber&) = delete;
    void operator=( const ImageGrabber&) = delete;
};  // end class

}   // end namespace

#endif
