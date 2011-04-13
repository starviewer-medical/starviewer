#ifndef UDGDICOMWRITER_CPP
#define UDGDICOMWRITER_CPP

#include "dicomwriter.h"
#include "dicomwriterdcmtk.h"

namespace udg {

DICOMWriter::~DICOMWriter()
{

}

DICOMWriter * DICOMWriter::newInstance() {
    return new DICOMWriterDCMTK();
}

void DICOMWriter::setPath(const QString &path)
{
    m_path = path;
}

QString DICOMWriter::getPath()
{
    return m_path;
}

}

#endif
