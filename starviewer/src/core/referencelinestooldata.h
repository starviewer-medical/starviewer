/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREFERENCELINESTOOLDATA_H
#define UDGREFERENCELINESTOOLDATA_H

#include "tooldata.h"

namespace udg {

class ImagePlane;

/**
Dades corresponents a la Tool de reference lines

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ReferenceLinesToolData : public ToolData
{
Q_OBJECT
public:
    ReferenceLinesToolData(QObject *parent = 0);

    ~ReferenceLinesToolData();

    /**
     * Retorna el frame of reference UID
     * @return
     */
    QString getFrameOfReferenceUID() const;

    /**
     * Retorna el pla d'imatge
     * @return
     */
    ImagePlane *getImagePlane() const;

public slots:
    /**
     * Li assignem el frameOfReference del pla de referencia
     * @param frameOfReference
     */
    void setFrameOfReferenceUID( const QString &frameOfReference );

    /**
     * Assigna el pla de la imatge de referencia
     * @param imagePlane
     */
    void setImagePlane( ImagePlane *imagePlane );

private:
    /// El frame of reference UID del pla de referència
    QString m_frameOfReferenceUID;

    /// Pla d'imatge del pla de referència
    ImagePlane *m_referenceImagePlane;
};

}

#endif
