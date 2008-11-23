/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCURSOR3DTOOLDATA_H
#define UDGCURSOR3DTOOLDATA_H

#include "tooldata.h"

namespace udg {

class ImagePlane;
        
/**
Classe que guarda les dades compartides per la tool de cursor 3D.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Cursor3DToolData : public ToolData
{
Q_OBJECT
public:
    Cursor3DToolData(QObject *parent = 0);

    ~Cursor3DToolData();

    /**
     * Retorna el frame of reference UID
     * @return
     */
    QString getFrameOfReferenceUID() const;

    /**
     * Retorna l'instance UID
     * @return
    */
    QString getInstanceUID() const;

    /**
     * Retorna el pla d'imatge
     * @return
     */
    ImagePlane *getImagePlane() const;

    /**
     * Retorna el punt origen
     * @return
     */
    double *getOriginPointPosition() const;

    /**
    * Visibilitat del cursor
    *
    */
    void setVisible( bool visibility );

    /**
    * Retorna cert si és visible, fals altrament.
    * @return
    */
    bool isVisible();

public slots:
    /**
     * Li assignem el frameOfReference del pla de referencia
     * El frame of reference només pot canviar de valor quan es canvia de sèrie.
     * Dins d'una mateixa sèrie, totes les imatges tenen el mateix frame of reference
     * @param frameOfReference
     */
    void setFrameOfReferenceUID( const QString &frameOfReference );

    /**
     * Li assignem l'instance UID
     * @param instanceUID
    */
    void setInstanceUID( const QString &instanceUID );
    
    /**
     * Assigna el pla de la imatge de referencia
     * Aquest pla pot canviar cada cop que es canvia de llesca en el viewer
     * Quan canvïi aquest valor, s'emetrà el senyal changed()
     * @param imagePlane
     */
    void setImagePlane( ImagePlane *imagePlane );

    /**
     * Assigna una posició al punt origen
     * Quan canvïi aquest valor, s'emetrà el senyal changed()
     * @param double
     */
    void setOriginPointPosition( double *position );

private:
    /// El frame of reference UID del pla de referència
    QString m_frameOfReferenceUID;

    /// Instance UID de la serie
    QString m_instanceUID;

    /// Pla d'imatge del pla de referència
    ImagePlane *m_referenceImagePlane;

    /// Punt a on es troba el cursor 3D
    double * m_originPointPosition;

    /// Visibilitat del cursor
    bool m_isVisible;
};

}

#endif
