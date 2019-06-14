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
