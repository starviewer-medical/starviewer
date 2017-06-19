/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGDICOMVALUEATTRIBUTE_H
#define UDGDICOMVALUEATTRIBUTE_H

#include "dicomattribute.h"
#include <QVariant>

namespace udg {

/**
    Classe heredada de \class DICOMAttribute encarregada de representar el atributs DICOM de tipus valor.
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
    QString toString(bool verbose = false);

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
