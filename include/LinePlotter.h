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

#ifndef RVTK_LINE_PLOTTER_H
#define RVTK_LINE_PLOTTER_H
#include "VTKTypes.h"

namespace RVTK
{

class rVTK_EXPORT LinePlotter
{
public:
	LinePlotter( double min = 0.0, double max = 1.0, int lineWidth = 1, vtkLookupTable* lookupTable = NULL);
   ~LinePlotter();

	void plotLine( double m[3], double n[3], double scalar);
	void plotLine( double x, double y, double z, double x2, double y2, double z2, double scalar);
	
	vtkActor* createActor() const;
	vtkPolyData* createPolyData() const;

private:
	double m_scalarMin, m_scalarMax ;
	int m_curPointID ;
	int m_allLineWidth ;
    bool m_newtable;

	vtkLookupTable* m_lookupTable ;
	vtkPoints* m_points;
	vtkCellArray* m_lines;
	vtkFloatArray* m_lineScalars ;
}; // end class LinePlotter

}   // end namespace

#endif
