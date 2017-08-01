#ifndef RVTK_MODEL_PATH_DRAWER_H
#define RVTK_MODEL_PATH_DRAWER_H

#include <vector>
#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourWidget.h>
#include <vtkObjectFactory.h>
#include <boost/shared_ptr.hpp>
#include "ClosestPointFinder.h"

namespace RVTK
{

// Public callback observer for ModelPathDrawer
class rVTK_EXPORT ModelPathEventObserver
{
public:
    virtual void startInteraction() {}
    virtual void endInteraction() {}
    virtual void interactionEvent() {}  // Moving after selecting something
};  // end class


class rVTK_EXPORT ModelPathDrawer
{
public:
    typedef boost::shared_ptr<ModelPathDrawer> Ptr;
    static Ptr create( const ClosestPointFinder*);
    ~ModelPathDrawer();

    void setInteractor( vtkRenderWindowInteractor*);
    void setEventObserver( ModelPathEventObserver*);

    // Set/get whether the path is closed (end points are joined to form a loop).
    void setClosed( bool closed) { _closedLoop = closed;}
    bool isClosed() const { return _closedLoop;}

    // Set/get the boundary handles.
    void setPathHandles( const std::vector<cv::Vec3f>&);
    int getPathHandles( std::vector<cv::Vec3f>&) const; // Returns number of handles.
    int getNumHandles() const;  // Returns the number of handles

    // Get all of the boundary vertices (nodes plus intermediate points).
    int getAllPathVertices( std::vector<cv::Vec3f>&) const;

    void setVisibility( bool visible);
    bool getVisibility() const;

private:
    vtkSmartPointer<vtkContourWidget> _cWidget;
    bool _closedLoop;

    ModelPathDrawer();
    ModelPathDrawer( const ModelPathDrawer&);   // No copy
    void operator=( const ModelPathDrawer&);    // No copy

    void setModel( const ClosestPointFinder*);
};  // end class

}   // end namespace

#endif
