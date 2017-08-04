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

#include "LinePlotter.h"
using RVTK::LinePlotter;


LinePlotter::LinePlotter( double min, double max, int lineWidth, vtkLookupTable* table)
   : m_scalarMin(min), m_scalarMax(max), m_curPointID(0), m_allLineWidth(lineWidth), m_newtable(false), m_lookupTable(table)
{
	m_points = vtkPoints::New();
	m_lines = vtkCellArray::New();
	m_lineScalars = vtkFloatArray::New();

	if (m_lookupTable == NULL)
   {
      m_newtable = true;
		m_lookupTable = vtkLookupTable::New();
   }  // end if
}  // end ctor



LinePlotter::~LinePlotter()
{
   m_points->Delete();
   m_lines->Delete();
   m_lineScalars->Delete();

   if ( m_newtable)
      m_lookupTable->Delete();
}  // end dtor



void LinePlotter::plotLine( double m[3], double n[3], double scalar)
{
	m_lineScalars->SetNumberOfComponents( 1);
	m_points->InsertNextPoint( m);
	m_lineScalars->InsertNextTuple1( scalar);
	m_points->InsertNextPoint( n);
	m_lineScalars->InsertNextTuple1( scalar);

	m_lines->InsertNextCell( 2);
	m_lines->InsertCellPoint( m_curPointID++);
	m_lines->InsertCellPoint( m_curPointID++);
}  // end plotLine



void LinePlotter::plotLine( double x, double y, double z, double x2, double y2, double z2, double scalar)
{
	double m[3],n[3];
	m[0]=x; m[1]=y; m[2]=z;
	n[0]=x2; n[1]=y2; n[2]=z2;
	plotLine( m, n, scalar);
}  // end plotLine



vtkPolyData* LinePlotter::createPolyData() const
{
	vtkPolyData* polyData = vtkPolyData::New();
	polyData->SetPoints( m_points);
	polyData->SetLines( m_lines);
	polyData->GetPointData()->SetScalars( m_lineScalars);
	return polyData;
}  // end createPolyData



vtkActor* LinePlotter::createActor() const
{
	vtkPolyData* polyData = createPolyData();

	vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();

#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif

    polyData->Delete();
	mapper->SetLookupTable( m_lookupTable);

	mapper->SetColorModeToMapScalars();
	mapper->SetScalarRange( m_scalarMin, m_scalarMax);
	mapper->SetScalarModeToUsePointData();

	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);
    mapper->Delete();
	actor->GetProperty()->SetLineWidth( m_allLineWidth);
	return actor;	
}  // end createActor
