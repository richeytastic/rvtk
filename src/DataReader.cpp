#include "DataReader.h"
using namespace RVTK;

#include <vtkBYUReader.h>
#include <vtkPolyDataReader.h>
#include <sstream>
using std::ostringstream;


DataReaderException::DataReaderException( const string& invalidext) : ext(invalidext){}


const char* DataReaderException::what() const throw()
{
   ostringstream oss;
   oss << "Files with extension " << ext << " are not supported by DataReaders!";
   return oss.str().c_str();
}  // end what



DataReader::DataReader( const string &fname) throw(DataReaderException)
{
   string ext = fname.substr(fname.rfind('.'));

   if (ext.compare( ".g") == 0)
   {
      vtkBYUReader* r = vtkBYUReader::New();
      r->SetFileName( fname.c_str());
      m_reader = r;
   }  // end if
   else if (ext.compare( ".vtk") == 0)
   {
      vtkPolyDataReader* r = vtkPolyDataReader::New();
      r->SetFileName( fname.c_str());
      m_reader = r;
   }  // end else
   else
   {
      throw DataReaderException(ext);
   }  // end else
}  // end ctor

