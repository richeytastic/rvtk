/************************************************************************
 * Copyright (C) 2018 Richard Palmer
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

#ifndef RVTK_OFFSCREEN_MODEL_VIEWER_H
#define RVTK_OFFSCREEN_MODEL_VIEWER_H

#include <RendererPicker.h>
#include <Viewer.h>
#include <ObjModel.h>       // RFeatures
#include <CameraParams.h>   // RFeatures
using byte = unsigned char;

namespace RVTK {

class rVTK_EXPORT OffscreenModelViewer
{
public:
    // Camera initially set to position 0,0,500 looking at 0,0,0 with
    // up vector as 0,1,0 and normal vector 0,0,1.
    OffscreenModelViewer( const cv::Size& dims, float rng=500);
    ~OffscreenModelViewer();

    void clear();   // Clear the viewer (remove and delete the actor).

    void setModel( const RFeatures::ObjModel&); // Reset the viewer with the given model.

    void setSize( const cv::Size&); // Set size of the viewer for snapshots

    void setCamera( const RFeatures::CameraParams&);

    // Take and return a snapshot of the scene.
    cv::Mat_<cv::Vec3b> snapshot() const;
    cv::Mat_<byte> lightnessSnapshot() const;

    // The following picking operations all use the top left as the image plane origin.

    // Returns true if given point (with top left origin) intersects with the current model/actor.
    bool pick( const cv::Point2f&) const;

    // Return the projected world position of the point (use after pick).
    cv::Vec3f worldPosition( const cv::Point2f&) const;

    // The inverse operation which takes a position in space and maps it to the image plane.
    cv::Point2f imagePlane( const cv::Vec3f&) const;

private:
    vtkActor *_actor;
    Viewer::Ptr _viewer;
    mutable RendererPicker *_picker;
    RendererPicker *picker() const;

    OffscreenModelViewer( const OffscreenModelViewer&) = delete;
    void operator=( const OffscreenModelViewer&) = delete;
};  // end class

}   // end namespace

#endif
