#pragma once
#ifndef RVTK_MODEL_BOUNDER_H
#define RVTK_MODEL_BOUNDER_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourWidget.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h>
#include <vtkBezierContourLineInterpolator.h>
#include <vtkTerrainContourLineInterpolator.h>
#include <vtkPolyDataCollection.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <DijkstraShortestPathFinder.h> // RFeatures (because VTK's implementation appears to be broken)
#include <ObjModelTools.h>  // RFeatures
#include "VtkModel.h"   // RVTK
#include "PointPlacer.h"    // RVTK
#include "rVTK_Export.h"

namespace RVTK
{

class MBInteractionObserver;    // Used internally only

// Public callback observer
class InteractionObserver
{
public:
    virtual void callback() = 0;
};  // end clas


class rVTK_EXPORT ModelBounder
{
public:
    typedef boost::shared_ptr<ModelBounder> Ptr;

    static Ptr create( const PointPlacer::Ptr,
                       const vtkSmartPointer<vtkRenderWindowInteractor>);

    static Ptr create( const vtkSmartPointer<vtkRenderer>,
                       const vtkSmartPointer<vtkRenderWindowInteractor>);

    ~ModelBounder();

    void setModel( const RVTK::VtkModel::Ptr);
    inline const RVTK::VtkModel::Ptr getModel() const { return _model;}
    inline const PointPlacer::Ptr getPointPlacer() const { return _pplacer;}

    // Get/set the boundary handles.
    int getBoundaryHandles( std::vector<cv::Vec3f>&) const; // Returns number of handles.
    void setBoundaryHandles( const std::vector<cv::Vec3f>&);
    int getNumHandles() const;  // Returns the number of handles

    // Reset the full boundary given a list of unique vertices that are presumed to form a connected
    // closed loop on the model and are ordered in such a fashion. The list is parsed to only retain
    // those vertices necessary to retain the definition of the full loop according to Dijkstra's
    // shortest path algorithm, so that the number of boundary handles is minimised.
    // Returns the number of boundary points/handles used to define the full boundary.
    //int resetFullBoundaryToRequiredNodes( std::vector<int>& boundaryUniqueVertexIds);

    // Get all of the boundary vertices (nodes plus intermediate points).
    int getAllBoundaryVertices( std::vector<cv::Vec3f>&) const;

    // Get the points defining the boundary over this actor. Every vertex on the boundary
    // is given - not just the user handles (the nodes), but also the intermediate points.
    // If model (unique) vertex IDs are required, use getVertexIndicesFromPoints() below.
    //vtkSmartPointer<vtkPoints> getBoundaryPoints() const;
    //vtkSmartPointer<vtkIdList> getBoundaryUniqueVertices() const;   // Less efficient - calls getBoundaryPoints()

    void setVisibility( bool visible);
    bool getVisibility() const;

    // Set an optional callback for when user has ended interaction with the model bounder.
    void setEndInteractionObserver( InteractionObserver* io);

private:
    const vtkSmartPointer<vtkRenderer> _renderer;
    const vtkSmartPointer<vtkRenderWindowInteractor> _interactor;
    const PointPlacer::Ptr _pplacer;
    vtkSmartPointer<vtkContourWidget> _contourWidget;

    RVTK::VtkModel::Ptr _model;
    friend class MBInteractionObserver;
    vtkSmartPointer<MBInteractionObserver> _iobserver;  // Internal VTK observer
    InteractionObserver* _endInteractionObserver;

    ModelBounder( const PointPlacer::Ptr,
                  const vtkSmartPointer<vtkRenderWindowInteractor>);

    ModelBounder( const vtkSmartPointer<vtkRenderer>,
                  const vtkSmartPointer<vtkRenderWindowInteractor>);

    ModelBounder( const ModelBounder&); // Not implemented
    void operator=( const ModelBounder&);   // Not implemented
};  // end class


// Define an object to listen to events on ModelBounders
class MBInteractionObserver : public vtkObject
{
public:
    static MBInteractionObserver* New();
    vtkTypeMacro( MBInteractionObserver, vtkObject);

    void setModelBounder( const ModelBounder*);

    void endInteractionCallback();

protected:
    MBInteractionObserver();
    virtual ~MBInteractionObserver();

private:
    const ModelBounder *_mb;

    // Not implemented
    MBInteractionObserver( const MBInteractionObserver&);
    void operator=( const MBInteractionObserver&);
};  // end class

}   // end namespace

#endif
