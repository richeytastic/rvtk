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

#include <ImagingInteractor.h>
using RVTK::ImagingInteractor;
#include <cassert>
#include <cmath>


//vtkStandardNewMacro( ImagingInteractor);


ImagingInteractor::ImagingInteractor()
{
	SetAutoAdjustCameraClippingRange( true);
}  // end ctor



void ImagingInteractor::OnMouseMove()
{
    InteractorC1::OnMouseMove();   // Forward
    updateImageRendering();
}  // end OnMouseMove



void ImagingInteractor::setViewer( Viewer *pv)
{
    InteractorC1::setViewer( pv);
    viewer = pv;
}   // end setViewer



void ImagingInteractor::setSphericalImage( const cv::Mat_<cv::Vec3b> &m)
{
    img = m;
    // Get the initial camera params for the centre of the image.
    cv::Vec3d pos, foc;
    viewer->getCamera( pos, foc, initUp);
    initDir = foc - pos;    // Direction that camera is pointing in
    initDir /= cv::norm(initDir);  // Ensure direction is normalised
    initUp /= cv::norm(initUp); // Normalised up vector
    assert( initDir.dot(initUp) == 0);  // Must be at right angles to one another!
    initRight = initDir.cross(initUp);

    /*
    // Get the angles of the initial direction vector
    initAngles[0] = atan2(initDir[2],initDir[1]);   // Rotation about X
    initAngles[1] = atan2(initDir[0],initDir[2]);   // Rotation about Y
    initAngles[2] = atan2(initDir[1],initDir[0]);   // Rotation about Z
    */
}   // end setSphericalImage



void ImagingInteractor::updateImageRendering()
{
    const double fov = viewer->getFieldOfView();
    assert( fov == 90); // Only works for 90 degree FoVs for now

    // Get the current camera position, focus and up vector
    cv::Vec3d pos, foc, upv;
    viewer->getCamera( pos, foc, upv);
    foc -= pos;
    foc /= cv::norm(foc);   // Normalised direction of camera
    upv /= cv::norm(upv);   // Normalised up vector
    rv = foc.cross(upv);    // Right vector

    cv::Vec3d angles;
    angles[0] = atan2(foc[2],foc[1]) - initAngles[0];   // Rotation about X difference
    angles[1] = atan2(foc[0],foc[2]) - initAngles[1];   // Rotation about Y difference
    angles[2] = atan2(foc[1],foc[0]) - initAngles[2];   // Rotation about Z difference


    upv -= pos;
    upv /= cv::norm(upv);   // Normalised up vector relative to camera
    cv::Vec3d rightVec = foc.cross(upv);    // Orthogonal to foc and upv in local positive X axis direction

    // foc is pointing to a position on the inside of the sphere. The difference
    // between this and the initial focal vector can give the coordinates for
    // the pixel from the spherical (equirectangular) image.
    // initDir is assumed to lie in the horizontal plane of the sphere incident
    // with the horizontal centre of the image.
    cv::Vec3d rvec(foc[0],foc[1],initDir[2]);
    rvec /= cv::norm(rvec); // Ensure unit length
    // rvec is now in the centre line horizontal plane
    double yawDelta = acos(initDir.dot(rvec));
    double wpxls = alpha * img.cols/(2*M_PI);   // Pixels wide difference

    // Radians between initial and current up vector
    double r2 = acos(


    // Render the correct section of the image
    ren = viewer->getRenderer();
}   // end updateImageRendering
