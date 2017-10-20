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
 * Display generic actors in a VTK viewer.
 *
 * Richard Palmer
 * June/December 2011
 */

#ifndef RVTK_VIEWER_H
#define RVTK_VIEWER_H

#include "VTKTypes.h"
#include <CameraParams.h>   // RFeatures
#include <boost/shared_ptr.hpp>
#include <opencv2/opencv.hpp>
#include "rVTK_Export.h"


namespace RVTK
{

class rVTK_EXPORT Viewer
{
public:
    typedef boost::shared_ptr<Viewer> Ptr;
    static Ptr create( bool offscreenRendering=false);

    // Check for multi-texturing support.
    bool getSupportsMultiTexturing() const;
    int getNumFixedTextureUnits() const;

    // Add the provided actor. If this is the first actor,
    // subsequent actors will be placed relative to it.
    void addActor( const vtkSmartPointer<vtkActor> actor);
    // Remove the provided actor from the viewer.
    void removeActor( const vtkSmartPointer<vtkActor> actor);
    void clear();	// Remove all actors

    void setCamera( const RFeatures::CameraParams&);
    RFeatures::CameraParams getCamera() const;

    // Get/set the near and far clipping range values
    double getClipNear() const;
    double getClipFar() const;
    void setClippingRange( double near, double far);    // Default is 0.1, 1000

    // Affects direction camera is looking in (i.e. modifies focus and up vector).
    void setCameraOrientation( double pitch, double yaw, double roll);
    void addCameraYaw( double yaw);
    void addCameraPitch( double pitch);
    void addCameraRoll( double roll);

    // Set perspective to be enabled if true or parallel projection if false.
    void setPerspective( bool enabled);

    // Set view scale for orthogonal view (only matters in parallel projection mode).
    void setParallelScale( double scale);

    // Set the interactor to be used on this viewer. Set the passed in interactor
    // with a custom style before or after calling this function. Alternatively,
    // use function setInteractorStyle below to attach a style to a new Interactor
    // which is set for the render window.
    void setInteractor( vtkSmartPointer<vtkRenderWindowInteractor>);
    void setInteractorStyle( vtkSmartPointer<vtkInteractorStyle>);

	// Change background colour to something between 0 and 255.
	void changeBackground( double c);

	// Set stereo rendering on or off and get the current stereo rendering value.
	void setStereoRendering( bool opt);
	bool getStereoRendering() const;

    void setSize( int width, int height);
    int getWidth() const;
    int getHeight() const;
    cv::Size getSize() const;

	// Update the rendered image after making changes.
	void updateRender();

	// Return the rendering window to interested clients.
	inline vtkSmartPointer<vtkRenderWindow> getRenderWindow() { return _renWin;}

    // Return the renderer to interested clients
    inline vtkSmartPointer<vtkRenderer> getRenderer() { return _ren;}

    // Create and return an image of the current window
    cv::Mat_<cv::Vec3b> extractImage() const;

    // Extract Z buffer - ensure camera clipping range is set properly prior to using!
    cv::Mat_<float> extractZBuffer() const;

private:
    vtkSmartPointer<vtkRenderer> _ren;
    vtkSmartPointer<vtkRenderWindow> _renWin;

    Viewer( bool);
    ~Viewer();
    Viewer( const Viewer&);             // No copy
    void operator=( const Viewer &);    // No copy
    class Deleter;
};  // end class

}   // end namespace

#endif
