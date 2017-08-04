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

#include "VtkModel.h"
#include "VtkActorCreator.h"
#include "VtkTools.h"
#include "ClosestPointFinder.h"
using RVTK::VtkModel;
using RFeatures::ObjModel;
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkProperty.h>
#include <iostream>


// public static
VtkModel::Ptr VtkModel::create(const ObjModel::Ptr model)
{
    return VtkModel::Ptr( new VtkModel(model));
}   // end create


// private
VtkModel::VtkModel( const ObjModel::Ptr model) : _omodel(model)
{
    RVTK::VtkActorCreator actorCreator;

    actorCreator.setVtkUniqueVertexIdSet( &_uvidxs);
    actorCreator.setVtkUniqueFaceIdSet( &_ufidxs);
    actorCreator.setObjToVTKUniqueVertexMap( &_uvmappings);     // Object to VTK unique vertex index mapping
    actorCreator.setObjToVTKUniqueFaceMap( &_ufmappings);       // Object to VTK face index mapping
    actorCreator.setObjToVTKUniqueVertexRMap( &_ruvmappings);   // VTK to Object unique vertex index mapping
    actorCreator.setObjToVTKUniqueFaceRMap( &_rufmappings);     // VTK to Object face index mapping
    // Face mappings currently not used
    //actorCreator.setObjToVTKFaceMap( &_fmappings);
    //actorCreator.setObjToVTKFaceRMap( &_rfmappings);

    _surfaceActor = actorCreator.generateSurfaceActor( model);
    _surfaceActor->GetProperty()->SetLineWidth(0.7);  // OpenGL only
    _surfaceActor->GetProperty()->SetPointSize(1.3);
    //_surfaceActor->GetProperty()->SetInterpolationToPhong();

    actorCreator.generateTexturedActors( model, _texturedActors);

    _unqcpf = new RVTK::ClosestPointFinder( _surfaceActor);

    /*
    const int ncols = 500;
    const vtkColor3ub scol( 255, 255,   0);
    const vtkColor3ub fcol( 0,     0, 255);
    vtkSmartPointer<vtkLookupTable> coloursLut = RVTK::createColoursLookupTable( ncols, scol, fcol);
    RVTK::setLookupTable( _surfaceActor, coloursLut);

    // Set the curvature information on the actor
    CurvFaceValuer cfv( model);
    vtkSmartPointer<vtkFloatArray> cvals = _actorCreator->createFaceLookupTableIndices( &cfv);
    cvals->SetName( "Curvature");
    RVTK::getPolyData(_surfaceActor)->GetCellData()->AddArray( cvals);

    // Set the colours (white) for the basic actor
    const int ncells = RVTK::getPolyData( _surfaceActor)->GetPolys()->GetNumberOfCells();
                    
    _basicColours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    _transColours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    _basicColours->SetNumberOfComponents(4);
    _transColours->SetNumberOfComponents(4);
    _basicColours->SetName("BaseColours");
    _transColours->SetName("Transparency");
    const unsigned char ucwhite[4] = {255,255,255,255}; // R,G,B,Opacity
    const unsigned char uctrans[4] = {255,255,255,45}; // R,G,B,Opacity
    for ( int i = 0; i < ncells; ++i)
    {
        _basicColours->InsertNextTupleValue( ucwhite);
        _transColours->InsertNextTupleValue( uctrans);
    }   // end for

    RVTK::getPolyData( _surfaceActor)->GetCellData()->AddArray( _basicColours);
    RVTK::getPolyData( _surfaceActor)->GetCellData()->AddArray( _transColours);
    */
}   // end ctor


// public
VtkModel::~VtkModel()
{
    // Delete the closest point finder
    if ( _unqcpf)
        delete _unqcpf;
}   // end dtor


// public
int VtkModel::getVtkBasicIdxFromObjFaceIdx( int objFaceIdx) const
{
    return _ufmappings.at( objFaceIdx);
}   // end getVtkBasicIdxFromObjFaceIdx


// public
int VtkModel::getVtkIdxFromObjUniqueVtx( int objUvidx) const
{
    return _uvmappings.at( objUvidx);
}   // end getVtkIdxFromObjUniqueVtx


// public
int VtkModel::getObjIdxFromVtkUniqueVtx( int vtkUvidx) const
{
    return _ruvmappings.at( vtkUvidx);
}   // end getObjIdxFromVtkUniqueVtx


// public
int VtkModel::getObjIdxFromVtkUniqueFace( int vtkFaceIdx) const
{
    return _rufmappings.at( vtkFaceIdx);
}   // end getObjIdxFromVtkUniqueFace


// public
int VtkModel::getClosestObjUniqueVertexIdx( const double v[3]) const
{
    return _ruvmappings.at( _unqcpf->getClosestVertex(v));
}   // end getClosestObjUniqueVertexIdx

// public
int VtkModel::getClosestObjUniqueVertexIdx( const cv::Vec3f& v) const
{
    return _ruvmappings.at( _unqcpf->getClosestVertex(v));
}   // end getClosestObjUniqueVertexIdx

// public
int VtkModel::getClosestObjFaceIdx( const double v[3]) const
{
    return _rufmappings.at( _unqcpf->getClosestCell(v));
}   // end getClosestObjFaceIdx

// public
int VtkModel::getClosestObjFaceIdx( const cv::Vec3f& v) const
{
    double v0[3] = {v[0], v[1], v[2]};
    return getClosestObjFaceIdx(v0);
}   // end getClosestObjFaceIdx


// public
cv::Vec3f VtkModel::getClosestPoint( const cv::Vec3f& v) const
{
    cv::Vec3f outv;
    _unqcpf->getClosestCell( v, &outv);
    return outv;
}   // end getClosestPoint


/*
// public
void VtkModel::colourObjectFace( int fid, const cv::Vec4b& rgba)
{
    // Get the VTK poly ID for the correct face on the _surfaceActor
    const int cid = _actorCreator->getVtkBasicIdxFromObjFaceIdx(fid);
    const unsigned char ucol[4] = {rgba[0],rgba[1],rgba[2],rgba[3]}; // R,G,B,Opacity
    _transColours->SetTupleValue( cid, ucol);
}   // end colourObjectFace
*/


/*
// public
VtkModel::Ptr VtkModel::segmentOnContour( const vtkSmartPointer<vtkPoints>& boundaryPts) const
{
    vtkPolyData* pdata = RVTK::getPolyData( _surfaceActor);

    vtkSmartPointer<vtkImplicitSelectionLoop> isloop = vtkSmartPointer<vtkImplicitSelectionLoop>::New();
    isloop->SetLoop( boundaryPts);

    vtkSmartPointer<vtkExtractPolyDataGeometry> gclipper = vtkSmartPointer<vtkExtractPolyDataGeometry>::New();
#if VTK_MAJOR_VERSION == 6
    gclipper->SetInputData(pdata);
#else
    gclipper->SetInput(pdata);
#endif
    gclipper->SetImplicitFunction( isloop);
    gclipper->Update();
    pdata = gclipper->GetOutput();

    const int numCells = pdata->GetNumberOfCells();
    if ( numCells <= 0)
        return VtkModel::Ptr(); // Unable to perform segmentation

    // Get the unique vertex indices from the extracted cells
    std::vector<int> uniqVtxIds;
    const int numUniqVtxs = (int)_objModel->getNumUniqueVertices();
    for ( int i = 0; i < numCells; ++i)
    {
        vtkCell* cell = pdata->GetCell(i);
        assert( cell->GetNumberOfPoints() == 3);

        const int u0 = cell->GetPointId(0);
        const int u1 = cell->GetPointId(1);
        const int u2 = cell->GetPointId(2);
        assert( u0 >= 0 && u0 < numUniqVtxs);
        assert( u1 >= 0 && u1 < numUniqVtxs);
        assert( u2 >= 0 && u2 < numUniqVtxs);

        uniqVtxIds.push_back(u0);
        uniqVtxIds.push_back(u1);
        uniqVtxIds.push_back(u2);
    }   // end for

    ObjModel::Ptr obj = _objModel->createFromUniqueVertices( uniqVtxIds);
    if ( !obj)
        return VtkModel::Ptr();

    // Use only the largest component of obj
    RFeatures::ObjComponentFinder ocf( obj);
    const std::list<ObjModel::Ptr>& components = ocf.find();
    std::cout << "Object components: " << components.size() << std::endl;
    const ObjModel::Ptr cmodel = *components.begin();
    std::vector<double> faceCurvature;
    cmodel->calculateCurvature(faceCurvature);
    return VtkModel::create( cmodel, faceCurvature);
}   // end segmentOnContour
*/

