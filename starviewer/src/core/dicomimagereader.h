/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMIMAGEREADER_H
#define UDGDICOMIMAGEREADER_H

#include <QObject>

namespace udg {

class Image;

/**
Interfície per lectors d'imatge DICOM. A partir d'un buffer de memòria allotjat prèviament i un conjunt d'Images carrega les dades DICOM en el buffer proporcionat.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class DICOMImageReader : public QObject
{
Q_OBJECT
public:
    DICOMImageReader(QObject *parent = 0);

    ~DICOMImageReader();

    /**
     * Li donem les imatges que volem que es carreguin
     * @param imageList Llista d'imatges.
     */
    void setInputImages( const QList<Image *> &imageList );

    /**
     * Li assignem el buffer on es copiaran les dades de les imatges que anem llegint
     * @param buffer
     */
    void setBufferPointer( const void *buffer );

    /**
     * Carrega les imatges d'entrada al buffer proporcionat.
     * @return Fals en cas que hi hagi algun error
     */
    virtual bool load() = 0;

signals:
    /**
     * Senyals que que s'envien per indicar el progrés de la lectura
     */
    void started();
    void progress(int);
    void finished();

protected:
    /// Buffer on es copiaran les dades que anem llegint
    void *m_imageBuffer;

    /// llista d'imatges que carregarem
    QList<Image *> m_inputImageList;
};

}

#endif
