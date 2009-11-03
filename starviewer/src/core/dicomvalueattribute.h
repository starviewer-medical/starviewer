/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMVALUEATTRIBUTE_H
#define UDGDICOMVALUEATTRIBUTE_H

#include <QVariant>
#include "dicomattribute.h"

namespace udg {


class DICOMValueAttribute : public DICOMAttribute
{
Q_OBJECT
public:
    
    DICOMValueAttribute();

    ~DICOMValueAttribute();
   
    bool isValueAttribute();

    bool isSequenceAttribute();

    void setValue( int value );
    void setValue( unsigned int value );
    void setValue( double value );
    void setValue( QString value );
    
    int getValueAsInt();
    unsigned int getValueAsUnsignedInt();
    double getValueAsDouble();
    QString getValueAsQString();

private:

    QVariant m_value;

};

}

#endif
