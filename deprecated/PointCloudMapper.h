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
 * Wraps around a point cloud object and a viewer to provide methods to draw itself.
 *
 * Richard Palmer
 * December 2011
 */

#pragma once
#ifndef RVTK_POINT_CLOUD_MAPPER_H
#define RVTK_POINT_CLOUD_MAPPER_H

#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>
#include <PointCloud.h>
using RFeatures::PointCloud;
#include "VTKTypes.h"


namespace RVTK
{

class rVTK_EXPORT PointCloudMapper
{
public:
    typedef boost::shared_ptr<PointCloudMapper> Ptr;
    static Ptr create( const cv::Mat_<cv::Vec3f> points, const cv::Mat_<cv::Vec3b> colourImg);
    static Ptr create( const PointCloud::Ptr points);
    static Ptr create();

    PointCloudMapper( const cv::Mat_<cv::Vec3f> points, const cv::Mat_<cv::Vec3b> colourImg);
    PointCloudMapper( const PointCloud::Ptr points);
    PointCloudMapper();
    ~PointCloudMapper(){}

    // Add the provided point cloud to mapper's existing point cloud.
    void add( const cv::Mat_<cv::Vec3f> points, const cv::Mat_<cv::Vec3b> colourImg);
    void add( const PointCloud::Ptr points);

    // For actors
    inline vtkSmartPointer<vtkPolyDataMapper> getMapper() const { return mapper;}

private:
    vtkSmartPointer<vtkPolyDataMapper> mapper;
};  // end class PointCloudMapper

}   // end namespace

#endif
