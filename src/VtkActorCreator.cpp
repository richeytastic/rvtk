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

#include <VtkActorCreator.h>
#include <VtkTools.h>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <ObjModelTriangleMeshParser.h> // RFeatures
using RFeatures::ObjModel;
using RVTK::VtkActorCreator;


namespace {

void init()
{
    static bool initCalled = false;
    if (!initCalled)
    {
        // Add static initialisation here...
        vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
        initCalled = true;
    }   // end if
}   // end ctor


vtkActor* makeActor( vtkSmartPointer<vtkPolyData> pd)
{
    /*
    pd = RVTK::generateNormals( pd);
    pd->BuildLinks();   // Required to use vtkPolyData::GetPointCells
    pd->BuildCells();
    */
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData( pd);
    vtkActor* actor = vtkActor::New();
    actor->SetMapper( mapper);
    return actor;
}   // end makeActor


vtkSmartPointer<vtkPoints> createVerts( const std::vector<cv::Vec3f>& vtxs, vtkCellArray* vertices)
{
    const int n = (int)vtxs.size();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &vtxs[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for
    return points;
}   // end createVerts


vtkSmartPointer<vtkPoints> createVerts( const ObjModel* model, vtkCellArray* vertices)
{
    assert( model->hasSequentialVertexIds());
    const int n = model->numVtxs();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &model->vtx( i)[0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for
    return points;
}   // end createVerts


vtkSmartPointer<vtkCellArray> createPolys( const ObjModel* model)
{
    assert( model->hasSequentialFaceIds());
    const int n = model->numPolys();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    for ( int f = 0; f < n; ++f)
    {
        const int* vidxs = model->fvidxs(f);
        polys->InsertNextCell( 3);
        polys->InsertCellPoint( vidxs[0]);
        polys->InsertCellPoint( vidxs[1]);
        polys->InsertCellPoint( vidxs[2]);
    }   // end for
    return polys;
}   // end createPolys


vtkSmartPointer<vtkPoints> createLines( const std::vector<cv::Vec3f>& vtxs,
                                        vtkCellArray* vertices, vtkCellArray* lines,
                                        bool joinLoop)
{
    const int n = (int)vtxs.size();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &vtxs[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for

    if ( joinLoop)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(n-1);
        lines->InsertCellPoint(0);
    }   // end if

    for ( int i = 1; i < n; ++i)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(i-1);
        lines->InsertCellPoint(i);
    }   // end for

    return points;
}   // end createLines


vtkSmartPointer<vtkPoints> createLinePairs( const std::vector<cv::Vec3f>& lps,
                                        vtkCellArray* vertices, vtkCellArray* lines)
{
    const int n = (int)lps.size();
    assert( n % 2 == 0);
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &lps[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for

    const int m = n/2;
    for ( int i = 0; i < m; ++i)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(2*i+0);
        lines->InsertCellPoint(2*i+1);
    }   // end for

    return points;
}   // end createLinePairs

}   // end namespace


vtkActor* VtkActorCreator::generateSurfaceActor( const ObjModel* model)
{
    assert( model->hasSequentialIds());
    if ( !model->hasSequentialIds())
    {
        std::cerr << "[ERROR] RVTK::VtkActorCreator::generateSurfaceActor: Vertex/Face IDs must be in sequential order!" << std::endl;
        return nullptr;
    }   // end if

    init();
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createVerts( model, vertices);
    vtkSmartPointer<vtkCellArray> faces = createPolys( model);

    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    //pd->SetVerts( vertices);  // NOPE! Can't seem to get rid of the vertices when visualising!
    pd->SetPoints( points);
    pd->SetPolys( faces);

    return makeActor(pd);
}   // end generateSurfaceActor



vtkActor* VtkActorCreator::generatePointsActor( const ObjModel* model)
{
    if ( !model->hasSequentialVertexIds())
    {
        std::cerr << "[ERROR] RVTK::VtkActorCreator::generatePointsActor: Vertex IDs must be in sequential order!" << std::endl;
        return nullptr;
    }   // end if

    init();
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createVerts( model, vertices);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    return makeActor(pd);
}   // end generatePointsActor


vtkActor* VtkActorCreator::generatePointsActor( const std::vector<cv::Vec3f>& vtxs)
{
    init();
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createVerts( vtxs, vertices);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    return makeActor(pd);
}   // end generatePointsActor



vtkActor* VtkActorCreator::generateLineActor( const std::vector<cv::Vec3f>& vtxs, bool joinLoop)
{
    init();
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createLines( vtxs, vertices, lines, joinLoop);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    pd->SetPolys( lines);
    return makeActor(pd);
}   // end generateLineActor


vtkActor* VtkActorCreator::generateLineActor( const std::list<cv::Vec3f>& vtxs, bool joinLoop)
{
    std::vector<cv::Vec3f> vpts(vtxs.begin(), vtxs.end());
    return generateLineActor( vpts, joinLoop);
}   // end generateLineActor


vtkActor* VtkActorCreator::generateLinePairsActor( const std::vector<cv::Vec3f>& lps)
{
    init();
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createLinePairs( lps, vertices, lines);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    pd->SetPolys( lines);
    return makeActor(pd);
}   // end generateLinePairsActor


vtkActor* VtkActorCreator::generateActor( const ObjModel* model, vtkSmartPointer<vtkTexture>& texture)
{
    texture = nullptr;

    if ( model->numMats() > 1)  // Can't create if more than one material!
    {
        std::cerr << "[ERROR] RVTK::VtkActorCreator::generateActor: Model has more than one material! Merge first." << std::endl;
        return nullptr;
    }   // end if

    if ( !model->hasSequentialIds())
    {
        std::cerr << "[ERROR] RVTK::VtkActorCreator::generateActor: Model IDs must be in sequential order!" << std::endl;
        return nullptr;
    }   // end if

    if ( model->numMats() == 0)
    {
        std::cerr << "[INFO] RVTK::VtkActorCreator::generateActor: Model has no materials; generating surface actor." << std::endl;
        return generateSurfaceActor( model);
    }   // end if

    const int MID = *model->materialIds().begin();   // The one and only material ID
    texture = RVTK::convertToTexture( model->texture(MID)); // Texture for return
    assert(texture);

    const int NP = 3*model->numPolys();

    init();
    vtkSmartPointer<vtkCellArray> faces = vtkSmartPointer<vtkCellArray>::New();
    // Note here for the number of points that it is approx. THREE TIMES the required number because we are mapping
    // texture coords to correspond with the points and there need to be 3 texture coords per triangle.
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( NP);
    vtkSmartPointer<vtkFloatArray> uvs = vtkSmartPointer<vtkFloatArray>::New();
    uvs->SetNumberOfComponents(2);
    uvs->SetNumberOfTuples( NP);
    uvs->SetName( "TCoords_0");

    int vtkPointId = 0;
    const int nfaces = model->numPolys();
    for ( int fid = 0; fid < nfaces; ++fid)    // Over all polygons
    {
        faces->InsertNextCell(3);
        const int* vtxs = model->fvidxs(fid);
        const int* uvids = model->faceUVs(fid);

        if ( uvids)
        {
            for ( int i = 0; i < 3; ++i)
            {
                const int vid = vtxs[i];
                faces->InsertCellPoint( vtkPointId);
                points->SetPoint( vtkPointId, &(model->vtx( vid)[0]));
                const cv::Vec2f& uv = model->uv( MID, uvids[i]);
                uvs->SetTuple2( vtkPointId, uv[0], uv[1]);
                vtkPointId++;
            }   // end for
        }   // end if
        else
        {
            for ( int i = 0; i < 3; ++i)
            {
                const int vid = vtxs[i];
                faces->InsertCellPoint( vtkPointId);
                points->SetPoint( vtkPointId, &(model->vtx( vid)[0]));
                uvs->SetTuple2( vtkPointId, 0.0, 0.0);
                vtkPointId++;
            }   // end for
        }   // end else
    }   // end for

    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetPoints( points);
    pd->SetPolys( faces);
    pd->GetPointData()->SetTCoords( uvs);

    vtkActor* actor = makeActor(pd);
    actor->SetTexture( texture);

    // Set ambient lighting for proper texture lighting
    actor->GetProperty()->SetAmbient(1.0);
    actor->GetProperty()->SetDiffuse(0.0);
    actor->GetProperty()->SetSpecular(0.0);

    return actor;
}   // end generateActor
