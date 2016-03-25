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
