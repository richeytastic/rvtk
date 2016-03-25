/**
 * Add key commands to a viewer
 * Richard Palmer
 * 2012
 */

#ifndef RVTK_KEY_PRESSER_H
#define RVTK_KEY_PRESSER_H

#include "Viewer.h"
using RVTK::Viewer;
#include <string>
using std::string;
#include <iostream>
#include <boost/shared_ptr.hpp>


namespace RVTK
{

class rVTK_EXPORT KeyPresser
{
public:
    typedef boost::shared_ptr<KeyPresser> Ptr;
    static Ptr create( Viewer::Ptr);

    // If true is returned, key press will not be passed to enclosing interactor for parsing.
    // Default implementation does the following. Entries marked (HANDLED) mean that the
    // key press is handled here and is not passed up to the interactor for parsing.
    // * z - change between perspective and orthogonal projection (HANDLED)
    // * c - print the current camera details to stdout
    virtual bool handleKeyPress( const string& keySymbol);

    virtual ~KeyPresser(){}

    virtual void printUsage( std::ostream& os) const;

protected:
    KeyPresser( Viewer::Ptr);
    RVTK::Viewer::Ptr getViewer() const;

private:
    RVTK::Viewer::Ptr m_pv;// point viewer to interact with
    bool m_orth;		// True if set to orthogonal view
}; // end class

}   // end namespace

#endif
