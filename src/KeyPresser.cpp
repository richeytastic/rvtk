#include "KeyPresser.h"
using RVTK::KeyPresser;
#include <iostream>


KeyPresser::Ptr KeyPresser::create( Viewer::Ptr pv)
{
    KeyPresser *kp = new KeyPresser(pv);
    return KeyPresser::Ptr( kp);
}   // end create


KeyPresser::KeyPresser( Viewer::Ptr pv) : m_pv(pv), m_orth( false)
{}  // end ctor



bool KeyPresser::handleKeyPress( const string& ckey)
{
    bool handled = false;

    if ( ckey == "z")
    {
        m_orth = !m_orth;   // Toggle
        if ( !m_orth)
            m_pv->setPerspective( true);	// Persepective setting
        else
            m_pv->setPerspective( false);	// Orthogonnal projection
        m_pv->updateRender();
        handled = true;
    }   // end else if
    else if ( ckey == "c")
        handled = true;

    return handled;
}  // end handleKeyPress



RVTK::Viewer::Ptr KeyPresser::getViewer() const
{
    return m_pv;
}   // end getViewer



void KeyPresser::printUsage( std::ostream& os) const
{
    os << "Toggle perspective/orthogonal projection: 'z'" << endl;
    os << "Print camera details: 'c'" << endl;
}   // end printUsage
