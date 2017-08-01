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
