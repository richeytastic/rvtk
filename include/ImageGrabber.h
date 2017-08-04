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

#include <opencv2/opencv.hpp>
#include <vtkRenderWindow.h>
#include "rVTK_Export.h"
typedef unsigned char byte;

namespace RVTK
{

class rVTK_EXPORT ImageGrabber
{
public:
    explicit ImageGrabber( const vtkRenderWindow*);

    // Grabs the current set of images from the passed in render window and scales to requested height.
    // If height is left as non-positive, produced images are the same size as the render window.
    void update( int reqPixelHeight = 0);

    cv::Size getImageSize() const { return _colmap.size();}
    const cv::Mat_<cv::Vec3b>& getColourMap() const { return _colmap;}
    const cv::Mat_<byte>& getLightMap() const { return _dcmap;}
    const cv::Mat_<float>& getDepthMap() const { return _dzmap;}
    const cv::Mat_<byte>& getByteDepthMap() const { return _ddmap;}

private:
    const vtkRenderWindow* _renWin;
    cv::Mat_<cv::Vec3b> _colmap;  // Original colour map
    cv::Mat_<byte> _dcmap;   // CIE-L light map
    cv::Mat_<float> _dzmap;  // Depth map (raw z-buffer floats)
    cv::Mat_<byte> _ddmap;   // Converted depth map

    ImageGrabber( const ImageGrabber&);
    void operator=( const ImageGrabber&);
};  // end class

}   // end namespace

#endif
