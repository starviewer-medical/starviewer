/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMVALUEATTRIBUTE_H
#define UDGDICOMVALUEATTRIBUTE_H

#include "dicomattribute.h"
#include <QVariant>


namespace udg {


class DICOMValueAttribute : public DICOMAttribute {

public:

    enum ValueRepresentation{ Unknown, String, ByteArray, Int, Uint, Float, Double, Date, Time };

    DICOMValueAttribute();

    ~DICOMValueAttribute();
   
    virtual bool isValueAttribute();

    virtual bool isSequenceAttribute();
    
    QString toString();

    void setValue(int value);
    void setValue(unsigned int value);
    void setValue(float value);
    void setValue(double value);
    void setValue(QString value);
    void setValue(QByteArray value);
    void setValue(QDate value);
    void setValue(QTime value);
    
    int getValueAsInt();
    unsigned int getValueAsUnsignedInt();
    float getValueAsFloat();
    double getValueAsDouble();
    QString getValueAsQString();
    QByteArray getValueAsByteArray();
    QDate getValueAsDate();
    QTime getValueAsTime();

    ValueRepresentation getValueRepresentation();

protected:
    void setValueRepresentation(ValueRepresentation value);

private:
    QVariant m_value;

    ValueRepresentation m_valueRepresentation;

};

}

#endif
