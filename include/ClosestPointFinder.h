#pragma once
#ifndef RVTK_CLOSEST_POINT_FINDER_H
#define RVTK_CLOSEST_POINT_FINDER_H

#include <opencv2/opencv.hpp>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkOctreePointLocator.h>
#include <vtkCellLocator.h>
#include "rVTK_Export.h"

namespace RVTK
{

class rVTK_EXPORT ClosestPointFinder
{
public:
    explicit ClosestPointFinder(
            const vtkSmartPointer<vtkPolyData>& data);

    int getClosestVertex( const cv::Vec3f& pos) const;  // Returns index of closest vertex
    int getClosestVertex( const double pos[3]) const;

    // Returns index of closest cell. Also sets the closest
    // point and the squared distance to the closest point if not NULL.
    int getClosestCell( const cv::Vec3f& pos, cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;
    int getClosestCell( const double pos[3], cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;

private:
    const vtkSmartPointer<vtkOctreePointLocator> _plocator;
    const vtkSmartPointer<vtkCellLocator> _clocator;
};   // end class

}   // end namespace

#endif
