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

#include <ModelPathDrawer.h>
#include <VtkTools.h>
#include <algorithm>
#include <vtkCommand.h>
#include <vtkProperty.h>
#include <vtkObjectFactory.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataCollection.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include <vtkPolygonalSurfaceContourLineInterpolator.h>
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

    void setModelPathEventObserver( const ModelPathDrawer* mpd, RVTK::ModelPathEventObserver* peo) { _mpd = mpd; _peo = peo; }

    virtual void Execute( vtkObject *vtkNotUsed(caller), unsigned long eventId, void *vtkNotUsed(callData))
    {
        if ( !_peo)
            return;

        switch ( eventId)
        {
            case vtkCommand::StartInteractionEvent:
                _peo->startInteraction( _mpd);
                break;
            case vtkCommand::EndInteractionEvent:
                _peo->endInteraction( _mpd);
                break;
            case vtkCommand::InteractionEvent:
                _peo->interactionEvent( _mpd);
                break;
            default:
                break;
        }   // end switch
    }   // end Execute

private:
    ModelPathCommandSender() : _peo(NULL) {}
    const ModelPathDrawer *_mpd;
    RVTK::ModelPathEventObserver* _peo;
};  // end class


class ModelPathDrawer::Deleter
{ public:
    void operator()( ModelPathDrawer *d) { delete d;}
};  // end class

// public
ModelPathDrawer::Ptr ModelPathDrawer::create( vtkSmartPointer<vtkRenderWindowInteractor> rwi) { return Ptr( new ModelPathDrawer( rwi), Deleter());}

// private
ModelPathDrawer::ModelPathDrawer( vtkSmartPointer<vtkRenderWindowInteractor> rwi) : _closedLoop(false)
{
    _cWidget = vtkSmartPointer<vtkContourWidget>::New();
    _cWidget->SetInteractor(rwi);
    getRep()->GetProperty()->SetRenderPointsAsSpheres(true);
}   // end ctor

// private
ModelPathDrawer::~ModelPathDrawer(){}


// private
vtkSmartPointer<vtkOrientedGlyphContourRepresentation> ModelPathDrawer::getRep()
{
    return vtkOrientedGlyphContourRepresentation::SafeDownCast( _cWidget->GetRepresentation());
}   // end getRep

const vtkSmartPointer<vtkOrientedGlyphContourRepresentation> ModelPathDrawer::getRep() const
{
    return vtkOrientedGlyphContourRepresentation::SafeDownCast( _cWidget->GetRepresentation());
}   // end getRep

void ModelPathDrawer::setLineWidth( double lwidth) { getRep()->GetLinesProperty()->SetLineWidth(lwidth); }
void ModelPathDrawer::setLineColour( double r, double g, double b) { getRep()->GetLinesProperty()->SetColor(r, g, b); }
void ModelPathDrawer::setPointSize( double sz) { getRep()->GetProperty()->SetPointSize( sz); }
void ModelPathDrawer::setPointColour( double r, double g, double b) { getRep()->GetProperty()->SetColor(r, g, b); }
void ModelPathDrawer::setVisibility( bool visible) { _cWidget->SetEnabled(visible);}
bool ModelPathDrawer::getVisibility() const { return _cWidget->GetEnabled() != 0;}
void ModelPathDrawer::setProcessEvents( bool pevents) { _cWidget->SetProcessEvents(pevents);}
bool ModelPathDrawer::getProcessEvents() const { return _cWidget->GetProcessEvents();}
int ModelPathDrawer::getNumHandles() const { return getRep()->GetNumberOfNodes();}
void ModelPathDrawer::setClosed( bool closed) { _closedLoop = closed;}
bool ModelPathDrawer::isClosed() const { return _closedLoop;}


// public
void ModelPathDrawer::setModel( const vtkActor* inactor)
{
    /*
    // Dijkstra interpolator won't accept not triangle cells
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->SetInputData( RVTK::getPolyData(inactor));
    triangleFilter->Update();
    vtkSmartPointer<vtkPolyData> tpd = triangleFilter->GetOutput();
    */
    vtkPolyData* tpd = RVTK::getPolyData(inactor);

    vtkSmartPointer<vtkPolygonalSurfacePointPlacer> pointPlacer = vtkSmartPointer<vtkPolygonalSurfacePointPlacer>::New();
    pointPlacer->AddProp( const_cast<vtkActor*>(inactor));
    pointPlacer->GetPolys()->AddItem( tpd);
    getRep()->SetPointPlacer( pointPlacer);

    vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator> interpolator = vtkSmartPointer<vtkPolygonalSurfaceContourLineInterpolator>::New();
    interpolator->GetPolys()->AddItem( tpd);
    getRep()->SetLineInterpolator( interpolator);
}   // end setModel


// public
void ModelPathDrawer::addEventObserver( RVTK::ModelPathEventObserver* peo)
{
    vtkSmartPointer<ModelPathCommandSender> commandSender = vtkSmartPointer<ModelPathCommandSender>::New();
    commandSender->setModelPathEventObserver( this, peo);
    _cWidget->AddObserver( vtkCommand::StartInteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::EndInteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::InteractionEvent, commandSender);
    _cWidget->AddObserver( vtkCommand::PlacePointEvent, commandSender);
}   // end addEventObserver


// public
void ModelPathDrawer::setPathHandles( const std::vector<cv::Vec3f>& nodes)
{
    getRep()->ClearAllNodes();
    getRep()->SetClosedLoop(false);

    const int n = (int)nodes.size();
    for ( int i = 0; i < n; ++i)
    {
        const cv::Vec3f& wv = nodes[i];
        getRep()->AddNodeAtWorldPosition( wv[0], wv[1], wv[2]);
    }   // end for

    if ( isClosed())
        _cWidget->CloseLoop();
}   // end setPathHandles


// public
int ModelPathDrawer::getPathHandles( std::vector<cv::Vec3f>& nodes) const
{
    const int n = getRep()->GetNumberOfNodes();
    int j = (int)nodes.size();
    nodes.resize( j + n);
    for ( int i = 0; i < n; ++i, ++j)
    {
        double v[3];
        getRep()->GetNthNodeWorldPosition( i, v);
        nodes[j] = cv::Vec3f(v[0], v[1], v[2]);
    }   // end for
    return n;
}   // end getPathHandles


// public
int ModelPathDrawer::getAllPathVertices( std::vector<cv::Vec3f>& vs) const
{
    const int numNodes = getRep()->GetNumberOfNodes();
    int count = 0;
    double x[3];
    for ( int i = 0; i < numNodes; ++i)
    {
        getRep()->GetNthNodeWorldPosition( i, x);
        vs.push_back( cv::Vec3f(x[0], x[1], x[2]));
        count++;

        const int n = getRep()->GetNumberOfIntermediatePoints( i); // From i to i+1
        for ( int j = 0; j < n; ++j)
        {
            getRep()->GetIntermediatePointWorldPosition( i, j, x);
            vs.push_back( cv::Vec3f(x[0], x[1], x[2]));
        }   // end for - intermediate points
        count += n;
    }   // end for - nodes

    return count;
}   // end getAllPathVertices
