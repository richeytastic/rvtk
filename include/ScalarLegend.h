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

    // Set simple colour range from minCol to maxCol over ncols.
    void setColours( const vtkColor3ub& minCol, const vtkColor3ub& maxCol, int ncols);

    // Specify a colour midway between the start and end colours.
    // If two values for the number of colours are used, ncols0 and ncols1,
    // ncols0 specifies the number of colour values below the middle value
    // and ncols1 specifies the number of colour values above the middle value.
    void setColours( const vtkColor3ub& minCol,  // Colour at min value
                     const vtkColor3ub& midCol,  // Colour at midway value
                     const vtkColor3ub& maxCol,  // Colour at max value
                     int ncols0, int ncols1=0);

    void setLookupTable( vtkMapper*, float minv, float maxv);

    int getNumColours() const;

    void show();
    void hide();
    bool isShown() const;

private:
    vtkRenderer* _ren;
    vtkScalarBarActor* _legend;
    vtkLookupTable* _lut;
    bool _shown;
};  // end class

}   // end namespace

#endif
