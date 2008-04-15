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
     * Retorna els plans a projectar
     * @return
     */
    QList<ImagePlane *> getPlanesToProject() const;

public slots:
    /**
     * Li assignem el frameOfReference del pla de referencia
     * El frame of reference només pot canviar de valor quan es canvia de sèrie.
     * Dins d'una mateixa sèrie, totes les imatges tenen el mateix frame of reference
     * @param frameOfReference
     */
    void setFrameOfReferenceUID( const QString &frameOfReference );

    /**
     * Assigna els plans de la serie de referencia
     * @param imagePlane
     */
    void setPlanesToProject( QList<ImagePlane *> planes );
    void setPlanesToProject( ImagePlane *plane );

private:
    /// El frame of reference UID del pla de referència
    QString m_frameOfReferenceUID;

    /// Llista de plans a projectar
    QList<ImagePlane *> m_planesToProject;
};

}

#endif
