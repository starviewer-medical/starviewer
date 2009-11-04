/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMWRITERDCMTK_CPP
#define UDGDICOMWRITERDCMTK_CPP

#include "dicomwriterdcmtk.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceattribute.h"

namespace udg {

DICOMWriterDCMTK::DICOMWriterDCMTK(QObject *parent)
{
}

DICOMWriterDCMTK::~DICOMWriterDCMTK()
{
}

void DICOMWriterDCMTK::addValueAttribute(DICOMValueAttribute * attribute)
{

}

void DICOMWriterDCMTK::addSequenceAttribute(DICOMSequenceAttribute * attribute)
{

}

bool DICOMWriterDCMTK::write()
{
    return false;
}

}

#endif
