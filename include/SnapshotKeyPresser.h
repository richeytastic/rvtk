#pragma once
#ifndef RVTK_SNAPSHOT_KEY_PRESSER_H
#define RVTK_SNAPSHOT_KEY_PRESSER_H

#include <opencv2/opencv.hpp>
#include "Viewer.h"
#include "KeyPresser.h"


namespace RVTK
{

class rVTK_EXPORT SnapshotKeyPresser : public RVTK::KeyPresser
{
public:
    typedef boost::shared_ptr<SnapshotKeyPresser> Ptr;

    static RVTK::SnapshotKeyPresser::Ptr create( RVTK::Viewer::Ptr,
            const cv::Vec3d& camPos, const cv::Vec3d& focalDir, const cv::Vec3d& vUp, float fov);

    virtual bool handleKeyPress( const string& keySym);
    virtual ~SnapshotKeyPresser() {}

    virtual void printUsage( std::ostream&) const;

    void setImage( const cv::Mat&);

protected:
    SnapshotKeyPresser( RVTK::Viewer::Ptr v,
            const cv::Vec3d& camPos, const cv::Vec3d& focalDir, const cv::Vec3d& vUp, float fov);

private:
    const cv::Vec3d _camPos;    // Initial camera position
    const cv::Vec3d _focalDir;  // Direction of camera focus (added to _camPos to give focal point)
    const cv::Vec3d _viewUp;    // Initial camera view up
    const float _fov;   // Field of view
    mutable int _snapCount; // Snapshot counter for images saved from the rendering buffer
    cv::Mat _image; // Image to display
    bool _showingImage; // True if showing image

    void resetCamera(); // Reset to front
    void addCameraYaw( float yaw);
    void addCameraPitch( float pitch);
    void addCameraRoll( float roll);
    void saveSnapshot();
};  // end class

}   // end namespace

#endif
