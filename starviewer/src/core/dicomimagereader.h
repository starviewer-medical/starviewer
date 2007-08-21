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
     * Li diem quin és el nombre de bytes que hi ha d'haver entre llesca i llesca
     * @param increment
     */
    void setSliceByteIncrement( unsigned int increment );

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
    /**
     * Comprova si tenim els inputs necessaris i correctes per poder carregar les imatges
     * @return Cert si tot està a punt, fals altrament
     */
    bool readyToLoad();

protected:
    /// Buffer on es copiaran les dades que anem llegint
    unsigned char *m_imageBuffer;

    /// Nombre de bytes entre llesca i llesca
    unsigned int m_sliceByteIncrement;

    /// llista d'imatges que carregarem
    QList<Image *> m_inputImageList;
};

}

#endif
