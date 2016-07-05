#include "ModelPathDrawer.h"
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkBezierContourLineInterpolator.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h> // Doesn't work!...
#include "DijkstraShortestPathLineInterpolator.h"   // RVTK
using RVTK::ModelPathDrawer;

// public
ModelPathDrawer::Ptr ModelPathDrawer::create( const vtkSmartPointer<vtkRenderer> r)
{
    return ModelPathDrawer::Ptr( new ModelPathDrawer( r));
}   // end create


// private
ModelPathDrawer::ModelPathDrawer( const vtkSmartPointer<vtkRenderer> r)
    : _renderer(r), _endInteractionObserver(NULL), _closedLoop(false)
{
    _contourWidget = vtkSmartPointer<vtkContourWidget>::New();
    //_contourWidget->SetInteractor( i);
    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> rep0 =
        vtkOrientedGlyphContourRepresentation::SafeDownCast( _contourWidget->GetRepresentation());
    rep0->GetLinesProperty()->SetColor(0.5,0.4, 1.0);
    rep0->GetLinesProperty()->SetLineWidth(4.0);

    rep0->GetProperty()->SetPointSize(6);
    //rep0->GetActiveProperty()->SetPointSize(17);

    _contourWidget->GetContourRepresentation()->SetHandleSize( 0.013);  // Size that handle expands to when mouse hovers over it
}   // end ctor


// public
ModelPathDrawer::~ModelPathDrawer()
{
    _endInteractionObserver = NULL;
}   // end ctor


// public
void ModelPathDrawer::setInteractor( const vtkSmartPointer<vtkRenderWindowInteractor> i)
{
    _contourWidget->SetInteractor(i);
    const bool oldenabled = _contourWidget->GetEnabled() != 0;
    _contourWidget->SetEnabled(true);
    _contourWidget->Initialize();
    _contourWidget->SetWidgetState( 1); // Manipulate
    _contourWidget->SetEnabled(oldenabled); // Reset previous state
}   // end setInteractor


using RVTK::MBInteractionObserver;
vtkStandardNewMacro( MBInteractionObserver);


MBInteractionObserver::MBInteractionObserver() : vtkObject(), _mb(NULL)
{}    // end ctor

MBInteractionObserver::~MBInteractionObserver()
{}    // end dtor

void MBInteractionObserver::setModelPathDrawer( const ModelPathDrawer* mb)
{
    _mb = mb;
}   // end setModelPathDrawer


void MBInteractionObserver::endInteractionCallback()
{
    if ( _mb->_endInteractionObserver)
        _mb->_endInteractionObserver->callback();
}   // end endInteractionCallback


// public
void ModelPathDrawer::setModel( const RVTK::VtkModel::Ptr model)
{
    _model = model;
    vtkSmartPointer<vtkPolygonalSurfacePointPlacer> pplacer = vtkPolygonalSurfacePointPlacer::New();
    pplacer->AddProp( model->getSurfaceActor());
    _contourWidget->GetContourRepresentation()->SetPointPlacer( pplacer);

    //vtkSmartPointer<vtkBezierContourLineInterpolator> interpolator =
    //                            vtkSmartPointer<vtkBezierContourLineInterpolator>::New();

    // Doesn't appear to work!
    //vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator> interpolator =
    //                            vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New();

    vtkSmartPointer<RVTK::DijkstraShortestPathLineInterpolator> interpolator = 
                                vtkSmartPointer<RVTK::DijkstraShortestPathLineInterpolator>::New();
    interpolator->setModel( model);

    _contourWidget->GetContourRepresentation()->SetLineInterpolator( interpolator);
    // TODO adjust how the contour widget operates?

    // Set a callback for whenever the user ends interaction with the widget
    _iobserver = vtkSmartPointer<MBInteractionObserver>::New();
    _iobserver->setModelPathDrawer(this);
}   // end setModel


// public
void ModelPathDrawer::setPathHandles( const std::vector<cv::Vec3f>& nodes)
{
    vtkSmartPointer<vtkContourRepresentation> rep =_contourWidget->GetContourRepresentation();
    rep->ClearAllNodes();
    rep->SetClosedLoop(false);

    const int n = (int)nodes.size();
    for ( int i = 0; i < n; ++i)
    {
        const cv::Vec3f& wv = nodes[i];
        rep->AddNodeAtWorldPosition( wv[0], wv[1], wv[2]);
    }   // end for

    if ( isClosed())
        _contourWidget->CloseLoop();
}   // end setPathHandles


// public
int ModelPathDrawer::getPathHandles( std::vector<cv::Vec3f>& nodes) const
{
    vtkSmartPointer<vtkContourRepresentation> rep =_contourWidget->GetContourRepresentation();
    const int n = rep->GetNumberOfNodes();
    int j = (int)nodes.size();
    nodes.resize( j + n);
    for ( int i = 0; i < n; ++i, ++j)
    {
        double v[3];
        rep->GetNthNodeWorldPosition( i, v);
        nodes[j] = cv::Vec3f(v[0], v[1], v[2]);
    }   // end for
    return n;
}   // end getPathHandles


// public
int ModelPathDrawer::getNumHandles() const
{
    return _contourWidget->GetContourRepresentation()->GetNumberOfNodes();
}   // end getNumHandles

/*
// public
int ModelPathDrawer::resetFullBoundaryToRequiredNodes( std::vector<int>& uvids)
{
    assert( _model);
    const RFeatures::ObjModel::Ptr om = _model->getObjectModel();

    int pathLen = 16;
    const int nvids = (int)uvids.size();
    if ( nvids <= 2*pathLen)
    {
        resetBoundary( uvids);
        return nvids;
    }   // end if

    std::vector<int> fuvids;

    RFeatures::DijkstraShortestPathFinder dspf( om);
    int si = 0;
    fuvids.push_back(uvids[0]);

    int pathInc = 0;
    int fi = pathLen;  // Path finish vertex to be tested
    while ( fi != 0 && si < (nvids-1))
    {
        // If fi is within 2*pathLen of the last vertex, we set it to be the first vertex
        // which means this could be the last iteration of the outer loop.
        if ( (fi + pathLen) >= nvids)
            fi = 0;

        const int stidx = uvids[si];
        const int fnidx = uvids[fi];
        if ( fnidx == stidx)
            break;

        dspf.setEndPointUniqueVertexIndices( fnidx, stidx);
        std::vector<int> sp; // Shortes path returned is from uvA to uvB which is why the params are swapped above.
        dspf.findShortestPath( sp);

        // Test the point of deviation with the shortest path.
        const int splen = (int)sp.size();
        int i;
        int sj;
        bool setPathPoint = false;
        for ( sj = si, i = 0; i < splen; ++i, ++sj)
        {
            // Check if deviation from the calculated shortest path seen
            if ( uvids[sj] != sp[i])
            {
                // If point of deviation is less than half the shortest path length, we need to try again but
                // with a path end point closer to the start point.
                if ( pathLen > 2 && (sj - si < pathLen/2))
                {
                    pathLen /= 2;
                    if ( fi != 0)
                        fi = si + pathLen;
                }   // end if
                else
                {   // Otherwise, we allow the point to be set
                    setPathPoint = true;
                    sj--;   // Vertex at sj-1 is the last on the same path
                }   // end else

                break;
            }   // end if
        }   // end for

        // If we didn't break early, the tested path length is too short, so we extend and try again
        if ( i == splen)
            pathInc++;

        if ( setPathPoint)
        {
            if ( sj == si)
                sj++;
            
            if ( sj - si < 2 && si != 0)
                fuvids.pop_back();

            fuvids.push_back(uvids[sj]);
            si = sj;
            pathInc = 0;
            pathLen *= 2;
        }   // end else

        if ( fi != 0)
            fi = si + pathLen + pathInc;
    }   // end while

    resetBoundary( fuvids);
    return (int)fuvids.size();
}   // end resetFullBoundaryToRequiredNodes
*/


// public
int ModelPathDrawer::getAllPathVertices( std::vector<cv::Vec3f>& vs) const
{
    const vtkSmartPointer<vtkContourRepresentation> rep = _contourWidget->GetContourRepresentation();
    const int numNodes = rep->GetNumberOfNodes();

    int count = 0;
    double x[3];
    for ( int i = 0; i < numNodes; ++i)
    {
        rep->GetNthNodeWorldPosition( i, x);
        vs.push_back( cv::Vec3f(x[0], x[1], x[2]));
        count++;

        const int n = rep->GetNumberOfIntermediatePoints( i); // From i to i+1
        for ( int j = 0; j < n; ++j)
        {
            rep->GetIntermediatePointWorldPosition( i, j, x);
            vs.push_back( cv::Vec3f(x[0], x[1], x[2]));
        }   // end for - intermediate points
        count += n;
    }   // end for - nodes

    return count;
}   // end getAllPathVertices


// public
void ModelPathDrawer::setVisibility( bool visible)
{
    _contourWidget->SetEnabled(visible);
}   // end setVisibility


// public
bool ModelPathDrawer::getVisibility() const
{
    return _contourWidget->GetEnabled() != 0;
}   // end getVisibility


// public
void ModelPathDrawer::setEndInteractionObserver( RVTK::InteractionObserver* io)
{
    _endInteractionObserver = io;
}   // end setEndInteractionObserver
#include "ModelPathDrawer.h"
