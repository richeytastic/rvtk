/**
 * Wraps around a vector of lines and a VTK viewer to provide methods to draw itself.
*
* Richard Palmer
* February 2012
*/

#pragma once
#ifndef RVTK_LINES_MAPPER_H
#define RVTK_LINES_MAPPER_H

#include "VTKTypes.h"
#include <boost/shared_ptr.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
typedef std::vector<cv::Vec6f> Lines3d;  // 3D lines (x1,y1,z1,x2,y2,z2)


namespace RVTK
{

class rVTK_EXPORT LinesMapper
{
public:
    typedef boost::shared_ptr<LinesMapper> Ptr;
    static Ptr create( const Lines3d &lines);
    static Ptr create();
    explicit LinesMapper( const Lines3d &lines);
    LinesMapper();
    ~LinesMapper(){}

    // Add the provided list of lines to this actor
    void add( const Lines3d &lines);

    inline Lines3d getLines() const { return lines3d;}

    inline vtkSmartPointer<vtkPolyDataMapper> getMapper() const { return mapper;}

private:
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    Lines3d lines3d;
};  // end class LinesMapper

}   // end namespace

#endif
