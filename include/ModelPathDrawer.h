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

#ifndef RVTK_MODEL_PATH_DRAWER_H
#define RVTK_MODEL_PATH_DRAWER_H

#include "rVTK_Export.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkContourWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <boost/shared_ptr.hpp>

namespace RVTK
{

class ModelPathDrawer;

// Public callback observer for ModelPathDrawer
class rVTK_EXPORT ModelPathEventObserver
{ public:
    virtual void startInteraction( const ModelPathDrawer*){}
    virtual void endInteraction( const ModelPathDrawer*){}
    virtual void interactionEvent( const ModelPathDrawer*){}  // Moving after selecting something
};  // end class


class rVTK_EXPORT ModelPathDrawer
{
public:
    typedef boost::shared_ptr<ModelPathDrawer> Ptr;
    static Ptr create( vtkSmartPointer<vtkRenderWindowInteractor>);

    void setModel( const vtkActor*);
    void addEventObserver( ModelPathEventObserver*);

    void setLineWidth( double);
    void setLineColour( double r, double g, double b);  // Values in [0,1]
    void setPointSize( double);
    void setPointColour( double r, double g, double b);  // Values in [0,1]

    void setVisibility( bool visible);
    bool getVisibility() const;
    void setProcessEvents( bool enable); // Turns interaction on/off independently of visibility
    bool getProcessEvents() const;

    // Set/get whether the path is closed (end points are joined to form a loop).
    void setClosed( bool closed);
    bool isClosed() const;
    int getNumHandles() const;  // Returns the number of handles

    // Set/get the boundary handles.
    void setPathHandles( const std::vector<cv::Vec3f>&);
    int getPathHandles( std::vector<cv::Vec3f>&) const; // Returns number of handles.

    // Get all of the boundary vertices (nodes plus intermediate points).
    int getAllPathVertices( std::vector<cv::Vec3f>&) const;

private:
    vtkSmartPointer<vtkContourWidget> _cWidget;
    bool _closedLoop;

    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> getRep();
    const vtkSmartPointer<vtkOrientedGlyphContourRepresentation> getRep() const;
    ModelPathDrawer( vtkSmartPointer<vtkRenderWindowInteractor>);
    ~ModelPathDrawer();
    ModelPathDrawer( const ModelPathDrawer&);   // No copy
    void operator=( const ModelPathDrawer&);    // No copy
    class Deleter;
};  // end class

}   // end namespace

#endif
