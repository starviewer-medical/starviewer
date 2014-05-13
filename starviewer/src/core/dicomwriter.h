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

#ifndef UDGDICOMWRITER_H
#define UDGDICOMWRITER_H

#include <QString>

namespace udg {

class DICOMValueAttribute;
class DICOMSequenceAttribute;

/**
    Interfície per l'escriptura de fitxers DICOM.
    El mètode \sa newInstance és l'encarregat de crear una instància de les classes d'implementació que es decideix
    tenint en compte un parametre de configuració.
  */
class DICOMWriter {

public:
    virtual ~DICOMWriter();

    /// Crea una nova instància d'alguna de les classes que implementa la interfície
    static DICOMWriter* newInstance();

    /// Assignar obtenir el path del fitxer
    void setPath(const QString &path);
    QString getPath();

    /// Afegir un nou atribut basic al fitxer DICOM
    virtual void addValueAttribute(DICOMValueAttribute *attribute) = 0;

    /// Afegir una sequencia al fitxer DICOM
    virtual bool addSequenceAttribute(DICOMSequenceAttribute *attribute) = 0;

    /// Generar el fitxer
    virtual bool write() = 0;

protected:
    /// Per instanciar nous objectes s'ha de fer ús del mètode \sa newInstance
    DICOMWriter(){}

private:
    QString m_path;

};

}

#endif // DICOMWRITER_H
