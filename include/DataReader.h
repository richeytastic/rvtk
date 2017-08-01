/**
 * Generic data reader class for VTK.
 *
 * Richard Palmer
 * 24 May 2011
 */

#ifndef RVTK_DATA_READER_H
#define RVTK_DATA_READER_H
// Disable warnings about MSVC compiler not implementing exception specifications
#ifdef _WIN32
#pragma warning( disable : 4290)
#endif

#include "rVTK_Export.h"
#include <string>
using std::string;
#include <exception>
#include "VTKTypes.h"


namespace RVTK
{

class rVTK_EXPORT DataReaderException: public std::exception
{
public:
   DataReaderException( const string& invalidext);
   ~DataReaderException() throw(){}
   virtual const char* what() const throw();
private:
   string ext;
}; // end DataReaderException


class rVTK_EXPORT DataReader
{
public:
   DataReader( const string &fname) throw (DataReaderException);
   inline vtkAlgorithmOutput* GetOutputPort(){ return m_reader->GetOutputPort();}
private:
   vtkAlgorithm* m_reader;
}; // end DataReader

}   // end namespace


#endif
