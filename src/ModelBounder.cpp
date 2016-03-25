#include "ModelBounder.h"
using RVTK::ModelBounder;
#include <iostream>
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>


/*** Custom line interpolation ***/
//class VTK_WIDGETS_EXPORT DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
class DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
{
public:
    static DijkstraShortestPathLineInterpolator* New()
    {
        return new DijkstraShortestPathLineInterpolator();
    }   // end New

    vtkTypeMacro( DijkstraShortestPathLineInterpolator, vtkContourLineInterpolator);

    virtual int InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int idx1, int idx2)
    {
        if ( !_spfinder)
        {
            std::cerr << "ERROR: DijkstraShortestPathLineInterpolator::InterpolateLine: must setPolyData() first!" << std::endl;
            return 0;
        }   // end if

        const int numNodes = rep->GetNumberOfNodes();
        if ( idx1 < 0 || idx2 < 0 || idx1 >= numNodes || idx2 >= numNodes)
            return 0;

        // Get the actual world positions of the contour nodes
        double p1[3];
        double p2[3];
        const int state1 = rep->GetNthNodeWorldPosition( idx1, p1);
        const int state2 = rep->GetNthNodeWorldPosition( idx2, p2);
        if ( !state1 || !state2)
        {
            std::cerr << "ERROR: DijkstraShortestPathLineInterpolator::InterpolateLine: failed to get Nth node world position!" << std::endl;
            return 0;
        }   // end if

        // Get the closest vertices on the model to the contour nodes
        //const int v1 = _plocator->getClosestVertex( p1);
        const int uv1 = _model->getClosestObjUniqueVertexIdx( p1);
        const int uv2 = _model->getClosestObjUniqueVertexIdx( p2);
        _spfinder->setEndPointUniqueVertexIndices( uv1, uv2);

        // Get the path vertices (inclusive of uv1 and uv2)
        std::vector<int>* pathVtxs = new std::vector<int>;
        const int pathLen = _spfinder->findShortestPath( *pathVtxs);
        if ( pathLen < 0)
        {
            std::cerr << "ERROR: DijkstraShortestPathLineInterpolator::InterpolateLine: no path found between unqiue vertices " << uv1 << " and " << uv2 << std::endl;
            return 0;
        }   // end if

        // Add the intermediate points
        // (Reverse the shortest path (since its actually uv2 to uv1 inclusive))
        const RFeatures::ObjModel::Ptr obj = _model->getObjectModel();
        double wpos[3];
        for ( int i = pathLen-1; i > 0; --i)
        {
            const cv::Vec3f& v = obj->getUniqueVertex( pathVtxs->at(i));
            wpos[0] = v[0]; wpos[1] = v[1]; wpos[2] = v[2];
            rep->AddIntermediatePointWorldPosition( idx1, wpos);
        }   // end for

        return 1;
    }   // end InterpolateLine


    void setModel( const RVTK::VtkModel::Ptr model)
    {
        reset();
        _model = model;
        _spfinder = new RFeatures::DijkstraShortestPathFinder( _model->getObjectModel());
    }   // end setPolyData

protected:
    DijkstraShortestPathLineInterpolator()
        : vtkContourLineInterpolator(), _spfinder(NULL) {}

    ~DijkstraShortestPathLineInterpolator()
    {
        reset();
    }   // end ctor

private:
    DijkstraShortestPathLineInterpolator( const DijkstraShortestPathLineInterpolator&);   // Not implemented
    void operator=( const DijkstraShortestPathLineInterpolator&);    // Not implemented

    RFeatures::DijkstraShortestPathFinder* _spfinder;
    RVTK::VtkModel::Ptr _model;

    void reset()
    {
        if ( _spfinder)
            delete _spfinder;
        _spfinder = NULL;
    }   // end reset
};  // end class



// public
ModelBounder::Ptr ModelBounder::create( const vtkSmartPointer<vtkRenderer> r,
                                        const vtkSmartPointer<vtkRenderWindowInteractor> i)
{
    return ModelBounder::Ptr( new ModelBounder( r, i));
}   // end create


ModelBounder::Ptr ModelBounder::create( const RVTK::PointPlacer::Ptr pp,
                                        const vtkSmartPointer<vtkRenderWindowInteractor> i)
{
    return ModelBounder::Ptr( new ModelBounder( pp, i));
}   // end create


vtkSmartPointer<vtkContourWidget> initContourWidget( const vtkSmartPointer<vtkRenderWindowInteractor> i)
{
    vtkSmartPointer<vtkContourWidget> cw = vtkSmartPointer<vtkContourWidget>::New();
    cw->SetInteractor( i);
    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> rep0 =
        vtkOrientedGlyphContourRepresentation::SafeDownCast( cw->GetRepresentation());
    rep0->GetLinesProperty()->SetColor(0.4,0.1,1);
    rep0->GetLinesProperty()->SetLineWidth(7.0);

    rep0->GetProperty()->SetPointSize(9);
    //rep0->GetActiveProperty()->SetPointSize(17);

    cw->GetContourRepresentation()->SetHandleSize( 0.015);  // Size that handle expands to when mouse hovers over it
    cw->SetEnabled(true);
    cw->Initialize();
    cw->SetWidgetState( 1); // Manipulate
    cw->SetEnabled(false);
    return cw;
}   // end initContourWidget


// private
ModelBounder::ModelBounder( const vtkSmartPointer<vtkRenderer> r,
                            const vtkSmartPointer<vtkRenderWindowInteractor> i)
    : _renderer(r), _interactor(i), _pplacer( RVTK::PointPlacer::create(r)), _endInteractionObserver(NULL)
{
    _contourWidget = initContourWidget(i);
}   // end ctor


// private
ModelBounder::ModelBounder( const RVTK::PointPlacer::Ptr pp,
                            const vtkSmartPointer<vtkRenderWindowInteractor> i)
    : _renderer(pp->getRenderer()), _interactor(i), _pplacer( pp), _endInteractionObserver(NULL)
{
    _contourWidget = initContourWidget(i);
}   // end ctor


// public
ModelBounder::~ModelBounder()
{
    _endInteractionObserver = NULL;
}   // end ctor




using RVTK::MBInteractionObserver;
vtkStandardNewMacro( MBInteractionObserver);


MBInteractionObserver::MBInteractionObserver() : vtkObject(), _mb(NULL)
{}    // end ctor

MBInteractionObserver::~MBInteractionObserver()
{}    // end dtor

void MBInteractionObserver::setModelBounder( const ModelBounder* mb)
{
    _mb = mb;
}   // end setModelBounder


void MBInteractionObserver::endInteractionCallback()
{
    if ( _mb->_endInteractionObserver)
        _mb->_endInteractionObserver->callback();
}   // end endInteractionCallback



// public
void ModelBounder::setModel( const RVTK::VtkModel::Ptr model)
{
    _pplacer->setModel( model->getSurfaceActor()); // Also removes old actors
    _contourWidget->GetContourRepresentation()->SetPointPlacer( _pplacer->getVtkPointPlacer());

    /*
    vtkSmartPointer<vtkBezierContourLineInterpolator> interpolator =
                                vtkSmartPointer<vtkBezierContourLineInterpolator>::New();
    interpolator->SetMaximumCurveError(1);
    */
    /*
    vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator> interpolator =
                                vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New();
    */

    vtkSmartPointer<DijkstraShortestPathLineInterpolator> interpolator = 
                                vtkSmartPointer<DijkstraShortestPathLineInterpolator>::New();
    interpolator->setModel( model);
    _contourWidget->GetContourRepresentation()->SetLineInterpolator( interpolator);
    // TODO adjust how the contour widget operates?

    // Set a callback for whenever the user ends interaction with the widget
    _iobserver = vtkSmartPointer<MBInteractionObserver>::New();
    _iobserver->setModelBounder(this);
    _contourWidget->AddObserver(vtkCommand::EndInteractionEvent, _iobserver, &MBInteractionObserver::endInteractionCallback);

    _model = model;
}   // end setModel



// public
void ModelBounder::setBoundaryHandles( const std::vector<cv::Vec3f>& nodes)
{
    vtkSmartPointer<vtkContourRepresentation> rep =_contourWidget->GetContourRepresentation();
    rep->ClearAllNodes();
    rep->SetClosedLoop(false);

    const int n = nodes.size();
    for ( int i = 0; i < n; ++i)
    {
        const cv::Vec3f& wv = nodes[i];
        rep->AddNodeAtWorldPosition( wv[0], wv[1], wv[2]);
    }   // end for

    _contourWidget->CloseLoop();
}   // end setBoundaryHandles


// public
int ModelBounder::getBoundaryHandles( std::vector<cv::Vec3f>& nodes) const
{
    vtkSmartPointer<vtkContourRepresentation> rep =_contourWidget->GetContourRepresentation();
    const int n = rep->GetNumberOfNodes();
    int j = nodes.size();
    nodes.resize( j + n);
    for ( int i = 0; i < n; ++i, ++j)
    {
        double v[3];
        rep->GetNthNodeWorldPosition( i, v);
        nodes[j] = cv::Vec3f(v[0], v[1], v[2]);
    }   // end for
    return n;
}   // end getBoundaryHandles


// public
int ModelBounder::getNumHandles() const
{
    return _contourWidget->GetContourRepresentation()->GetNumberOfNodes();
}   // end getNumHandles

/*
// public
int ModelBounder::resetFullBoundaryToRequiredNodes( std::vector<int>& uvids)
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
int ModelBounder::getAllBoundaryVertices( std::vector<cv::Vec3f>& vs) const
{
    const vtkSmartPointer<vtkContourRepresentation> rep = _contourWidget->GetContourRepresentation();
    const int numNodes = rep->GetNumberOfNodes();
    vtkSmartPointer<vtkPoints> bpts = vtkSmartPointer<vtkPoints>::New();

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
}   // end getAllBoundaryVertices


// public
void ModelBounder::setVisibility( bool visible)
{
    _contourWidget->SetEnabled(visible);
}   // end setVisibility


// public
bool ModelBounder::getVisibility() const
{
    return _contourWidget->GetEnabled();
}   // end getVisibility


// public
void ModelBounder::setEndInteractionObserver( RVTK::InteractionObserver* io)
{
    _endInteractionObserver = io;
}   // end setEndInteractionObserver
