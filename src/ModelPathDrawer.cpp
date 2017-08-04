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

#include "ModelPathDrawer.h"
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkProperty.h>
#include <vtkBezierContourLineInterpolator.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h> // Doesn't work!...
#include "DijkstraShortestPathLineInterpolator.h"   // RVTK
using RVTK::ModelPathDrawer;


// Handle interaction events
class ModelPathCommandSender : public vtkCommand
{
public:
    vtkTypeMacro( ModelPathCommandSender, vtkCommand);

    static ModelPathCommandSender* New()
    {
        return new ModelPathCommandSender;
    }   // end New()

    virtual void Execute( vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData))
    {
        if ( !_peo)
            return;

        switch ( eventId)
        {
            case vtkCommand::StartInteractionEvent:
                _peo->startInteraction();
                break;
            case vtkCommand::EndInteractionEvent:
                _peo->endInteraction();
                break;
            case vtkCommand::InteractionEvent:
                _peo->interactionEvent();
                break;
            default:
                break;
        }   // end switch
    }   // end Execute

    void setModelPathEventObserver( RVTK::ModelPathEventObserver* peo)
    {
        _peo = peo;
    }   // end setModelPathEventObserver

private:
    ModelPathCommandSender() : _peo(NULL) {}
    RVTK::ModelPathEventObserver* _peo;
};  // end class



// public
ModelPathDrawer::Ptr ModelPathDrawer::create( const RVTK::ClosestPointFinder* cpf)
{
    ModelPathDrawer::Ptr mpd = ModelPathDrawer::Ptr( new ModelPathDrawer());
    mpd->setModel( cpf);
    return mpd;
}   // end create


// private
ModelPathDrawer::ModelPathDrawer() : _closedLoop(false)
{
    _cWidget = vtkSmartPointer<vtkContourWidget>::New();
    //_cWidget->SetInteractor( i);
    vtkSmartPointer<vtkOrientedGlyphContourRepresentation> rep =
        vtkOrientedGlyphContourRepresentation::SafeDownCast( _cWidget->GetRepresentation());
    rep->GetLinesProperty()->SetColor(0.5, 0.4, 1.0);
    rep->GetLinesProperty()->SetLineWidth(4.0);

    rep->GetProperty()->SetPointSize(6);
    //rep->GetActiveProperty()->SetPointSize(17);
    _cWidget->GetContourRepresentation()->SetHandleSize( 0.013);  // Size that handle expands to when mouse hovers over it
}   // end ctor


// public
ModelPathDrawer::~ModelPathDrawer()
{
    _cWidget->SetEnabled(false);
}   // end ctor


// public
void ModelPathDrawer::setInteractor( vtkRenderWindowInteractor* i)
{
    _cWidget->SetInteractor(i);
    const bool oldenabled = _cWidget->GetEnabled() != 0;
    _cWidget->SetEnabled(true);
    _cWidget->Initialize();
    _cWidget->SetWidgetState( 1); // Manipulate
    _cWidget->SetEnabled(oldenabled); // Reset previous state
}   // end setInteractor


// private
void ModelPathDrawer::setModel( const RVTK::ClosestPointFinder* cpf)
{
    vtkSmartPointer<vtkPolygonalSurfacePointPlacer> pplacer = vtkPolygonalSurfacePointPlacer::New();
    pplacer->AddProp( cpf->getSurfaceActor());
    _cWidget->GetContourRepresentation()->SetPointPlacer( pplacer);

    //vtkSmartPointer<vtkBezierContourLineInterpolator> interpolator =
    //                            vtkSmartPointer<vtkBezierContourLineInterpolator>::New();

    // Doesn't appear to work!
    //vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator> interpolator =
    //                            vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New();

    vtkSmartPointer<RVTK::DijkstraShortestPathLineInterpolator> interpolator = 
                                vtkSmartPointer<RVTK::DijkstraShortestPathLineInterpolator>::New();

    interpolator->setModel( cpf);

    _cWidget->GetContourRepresentation()->SetLineInterpolator( interpolator);
}   // end setModel


// public
void ModelPathDrawer::setEventObserver( RVTK::ModelPathEventObserver* peo)
{
    vtkSmartPointer<ModelPathCommandSender> commandSender = vtkSmartPointer<ModelPathCommandSender>::New();
    commandSender->setModelPathEventObserver( peo);
    _cWidget->AddObserver( vtkCommand::StartInteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::EndInteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::InteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::PlacePointEvent, commandSender);
}   // end setEventObserver


// public
void ModelPathDrawer::setPathHandles( const std::vector<cv::Vec3f>& nodes)
{
    vtkSmartPointer<vtkContourRepresentation> rep =_cWidget->GetContourRepresentation();
    rep->ClearAllNodes();
    rep->SetClosedLoop(false);

    const int n = (int)nodes.size();
    for ( int i = 0; i < n; ++i)
    {
        const cv::Vec3f& wv = nodes[i];
        rep->AddNodeAtWorldPosition( wv[0], wv[1], wv[2]);
    }   // end for

    if ( isClosed())
        _cWidget->CloseLoop();
}   // end setPathHandles


// public
int ModelPathDrawer::getPathHandles( std::vector<cv::Vec3f>& nodes) const
{
    vtkSmartPointer<vtkContourRepresentation> rep =_cWidget->GetContourRepresentation();
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
    return _cWidget->GetContourRepresentation()->GetNumberOfNodes();
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
    const vtkSmartPointer<vtkContourRepresentation> rep = _cWidget->GetContourRepresentation();
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
    _cWidget->SetEnabled(visible);
}   // end setVisibility


// public
bool ModelPathDrawer::getVisibility() const
{
    return _cWidget->GetEnabled() != 0;
}   // end getVisibility
