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

#ifndef RVTK_SCALAR_LEGEND_H
#define RVTK_SCALAR_LEGEND_H

#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkMapper.h>
#include <vtkRenderer.h>
#include <vtkColor.h>
#include <string>
#include "rVTK_Export.h"

namespace RVTK
{

class rVTK_EXPORT ScalarLegend
{
public:
    explicit ScalarLegend( vtkRenderer*);
    virtual ~ScalarLegend();

    void setTitle( const std::string& title);
    void setColours( int ncols, const vtkColor3ub& scol, const vtkColor3ub& fcol);
    void setLookupTable( vtkMapper*, float minv, float maxv);

    void show();
    void hide();

private:
    vtkRenderer* _ren;
    vtkScalarBarActor* _legend;
    vtkLookupTable* _lut;
};  // end class

}   // end namespace

#endif
