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
