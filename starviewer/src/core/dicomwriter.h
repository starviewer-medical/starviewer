/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMWRITER_H
#define UDGDICOMWRITER_H

#include <QObject>
#include <QString>

namespace udg {

class DICOMValueAttribute;
class DICOMSequenceAttribute;

/**

Interfície per l'escriptura de fitxers DICOM.
El mètode \sa newInstance és l'encarregat de crear una instància de les classes d'implementació que es decideix
tenint en compte un parametre de configuració.

@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>

*/
class DICOMWriter : public QObject
{
Q_OBJECT

public:

    /// Crea una nova instància d'alguna de les classes que implementa la interfície
    static DICOMWriter * newInstance();

    /// Assignar obtenir el path del fitxer
    void setPath(QString path);
    QString getPath();

    /// Afegir un nou atribut basic al fitxer DICOM
    virtual void addValueAttribute(DICOMValueAttribute * attribute);

    /// Afegir una sequencia al fitxer DICOM
    virtual void addSequenceAttribute(DICOMSequenceAttribute * attribute);

    /// Generar el fitxer
    virtual bool write();

protected:
    /// Per instanciar nous objectes s'ha de fer ús del mètode \sa newInstance
    DICOMWriter(QObject *parent = 0){}

private:
    QString m_path;

};

}

#endif // DICOMWRITER_H
