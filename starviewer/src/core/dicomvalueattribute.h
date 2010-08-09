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

/** Classe heredada de \class DICOMAttribute encarregada de representar el atributs DICOM de tipus valor.
  */
class DICOMValueAttribute : public DICOMAttribute {

public:
    /// Llista dels tipus de valor vàlids.
    enum ValueRepresentation{ Unknown, String, ByteArray, Int, Uint, Float, Double, Date, Time };

    DICOMValueAttribute();

    ~DICOMValueAttribute();
   
    /// Retorna sempre cert
    virtual bool isValueAttribute();

    /// Retorna sempre false
    virtual bool isSequenceAttribute();
    
    /// Retorna el contingut de l'atribut formatat
    QString toString();

    /// Mètodes per assignar el valor segons el tipus
    void setValue(int value);
    void setValue(unsigned int value);
    void setValue(float value);
    void setValue(double value);
    void setValue(const QString &value);
    void setValue(const QByteArray &value);
    void setValue(const QDate &value);
    void setValue(const QTime &value);
    
    /// Mètodes per obtenir el valor segons el tipus
    int getValueAsInt();
    unsigned int getValueAsUnsignedInt();
    float getValueAsFloat();
    double getValueAsDouble();
    QString getValueAsQString();
    QByteArray getValueAsByteArray();
    QDate getValueAsDate();
    QTime getValueAsTime();

    /// Retorna el tipus del valor
    ValueRepresentation getValueRepresentation();

protected:
    /// Assigna el tipus del valor
    void setValueRepresentation(ValueRepresentation value);

private:
    /// Representa el valor
    QVariant m_value;

    /// Tipus del valor representat
    ValueRepresentation m_valueRepresentation;

};

}

#endif
