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
 * Interactor that understands how to map a spherical image to movements of the camera.
 * Richard Palmer
 * October 2012
 */

#ifndef RVTK_IMAGING_INTERACTOR_H
#define RVTK_IMAGING_INTERACTOR_H

#include <opencv2/opencv.hpp>
#include "InteractorC1.h"
using RVTK::InteractorC1;
#include "Viewer.h"
using RVTK::Viewer;



namespace RVTK
{

class rVTK_EXPORT ImagingInteractor : public InteractorC1
{
public:
    static ImagingInteractor *New();
    //vtkTypeMacro( ImagingInteractor, vtkInteractorStyleTrackballCamera);

    virtual void OnMouseMove();

    void setViewer( Viewer *pv);

    // Upon setting the colour spherical image, the current camera orientation is
    // initialised to the centre of this image. Movements of the camera focus will
    // be used to calculate new pixel indices *from this initial position*!
    void setSphericalImage( const cv::Mat_<cv::Vec3b>&);

private:
    Viewer *viewer;
    cv::Mat_<cv::Vec3b> img;  // The spherical image to map
    // Vectors that give the local coordinate system according to the direction of the
    // camera when the image is set. All yaw, pitch and roll is calculated with respect
    // to these vectors. Note that changes in camera position are NOT allowed!
    cv::Vec3d initDir;        // Initial direction of camera orientation (into scene)
    cv::Vec3d initUp;         // Initial up vector (orthogonal to initDir)
    cv::Vec3d initRight;      // Initial vector being the cross product of initDir and initUp
    //cv::Vec3d initAngles;   // Rotation angles of the initial focal vector (x,y,z)

    ImagingInteractor();
    void updateImageRendering();
}; // end class

}   // end namespace

#endif
