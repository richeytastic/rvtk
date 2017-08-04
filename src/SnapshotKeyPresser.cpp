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

#include "SnapshotKeyPresser.h"
using RVTK::SnapshotKeyPresser;
#include <cmath>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>


// private
void SnapshotKeyPresser::resetCamera()
{
    const cv::Vec3d focPoint = _camPos + _focalDir * 1e-10;
    getViewer()->resetCamera( _camPos, focPoint, _viewUp, _fov);
}   // end resetCamera


// private
void SnapshotKeyPresser::addCameraYaw( float yaw)
{
    RVTK::Viewer::Ptr pv = getViewer();
    pv->addCameraYaw( yaw);
}   // end addCameraYaw


// private
void SnapshotKeyPresser::addCameraPitch( float pitch)
{
    RVTK::Viewer::Ptr pv = getViewer();
    pv->addCameraPitch( pitch);
}   // end addCameraPitch


// private
void SnapshotKeyPresser::addCameraRoll( float roll)
{
    RVTK::Viewer::Ptr pv = getViewer();
    pv->addCameraRoll( roll);
}   // end addCameraRoll


// private
void SnapshotKeyPresser::saveSnapshot()
{
    getViewer()->updateRender();
    const cv::Mat_<cv::Vec3b> img = getViewer()->extractImage();
    std::ostringstream oss;
    oss << "snapshot_" << (_snapCount++) << ".jpg";
    cv::imwrite( oss.str(), img);
    cout << "Snapshot saved to " << oss.str() << endl;
}   // end saveSnapshot



RVTK::SnapshotKeyPresser::Ptr SnapshotKeyPresser::create( RVTK::Viewer::Ptr p,
        const cv::Vec3d& camPos, const cv::Vec3d& focalDir, const cv::Vec3d& upVec, float fov)
{
    return RVTK::SnapshotKeyPresser::Ptr( new SnapshotKeyPresser( p, camPos, focalDir, upVec, fov));
}   // end create



bool SnapshotKeyPresser::handleKeyPress( const string& keySym)
{
    // Is this key press handled by the super class?
    bool handled = false;
    if ( keySym == "Prior") // PageUp
    {
        handled = true;
        resetCamera();
        getViewer()->updateRender();
    }   // end if
    else if ( keySym == "Home")
    {
        handled = true;
        resetCamera();
        for ( int i = 0; i < 90; ++i)
            addCameraYaw(1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "Next") // PageDown
    {
        handled = true;
        resetCamera();
        for ( int i = 0; i < 180; ++i)
            addCameraYaw(1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "End")
    {
        handled = true;
        resetCamera();
        for ( int i = 0; i < 90; ++i)
            addCameraYaw(-1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "S")    // Generate an image from the current rendering
    {
        handled = true;
        saveSnapshot();
    }   // end else if
    else if ( keySym == "D")
    {
        handled = true;
        const string winName = "Image (any key to exit)";
        if ( !_image.empty())
        {
            cv::namedWindow( winName);
            cv::imshow( winName, _image);
            cv::waitKey();
            cv::destroyWindow( winName);
        }   // end if
    }   // end else if
    else if ( keySym == "Left")
    {
        handled = true;
        addCameraYaw(1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "Right")
    {
        handled = true;
        addCameraYaw(-1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "Up")
    {
        handled = true;
        addCameraPitch(1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "Down")
    {
        handled = true;
        addCameraPitch(-1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "less")
    {
        handled = true;
        addCameraRoll(-1);
        getViewer()->updateRender();
    }   // end else if
    else if ( keySym == "greater")
    {
        handled = true;
        addCameraRoll(1);
        getViewer()->updateRender();
    }   // end else if

    if ( !handled)
        handled = RVTK::KeyPresser::handleKeyPress( keySym);

    return handled;
}   // end handleKeyPress


// public
void SnapshotKeyPresser::printUsage( std::ostream& os) const
{
    KeyPresser::printUsage( os);
    os << "Front view: 'PageUp'" << endl;
    os << "Rear view: 'PageDown'" << endl;
    os << "Left view: 'Home'" << endl;
    os << "Right view: 'End'" << endl;
    os << "Yaw & pitch camera with cursor keys" << endl;
    os << "Roll camera with '<' and '>'" << endl;
    os << "Display set image: 'D'" << endl;
    os << "Save JPEG Snapshot: 'S'" << endl;
}   // end printUsage



SnapshotKeyPresser::SnapshotKeyPresser( RVTK::Viewer::Ptr v,
        const cv::Vec3d& camPos, const cv::Vec3d& focDir, const cv::Vec3d& vUp, float fov)
    : RVTK::KeyPresser(v), _camPos(camPos), _focalDir(focDir / cv::norm(focDir)), _viewUp(vUp), _fov(fov), _snapCount(0)
{
    _showingImage = false;
    resetCamera();
}   // end ctor



void SnapshotKeyPresser::setImage( const cv::Mat& img)
{
    _image = img;
}   // end setImage
