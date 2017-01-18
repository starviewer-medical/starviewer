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

#ifndef UDGREFERENCELINESTOOLDATA_H
#define UDGREFERENCELINESTOOLDATA_H

#include "tooldata.h"

namespace udg {

class ImagePlane;

/**
    Dades corresponents a la Tool de reference lines
  */
class ReferenceLinesToolData : public ToolData {
Q_OBJECT
public:
    ReferenceLinesToolData(QObject *parent = 0);
    ~ReferenceLinesToolData();

    /// Retorna el frame of reference UID
    /// @return
    QString getFrameOfReferenceUID() const;

    /// Retorna els plans a projectar
    /// @return
    const QList<QSharedPointer<ImagePlane>>& getPlanesToProject() const;

public slots:
    /// Li assignem el frameOfReference del pla de referencia
    /// El frame of reference només pot canviar de valor quan es canvia de sèrie.
    /// Dins d'una mateixa sèrie, totes les imatges tenen el mateix frame of reference
    /// @param frameOfReference
    void setFrameOfReferenceUID(const QString &frameOfReference);

    /// Assigna els plans de la serie de referencia
    /// @param imagePlane
    void setPlanesToProject(QList<QSharedPointer<ImagePlane>> planes);
    void setPlanesToProject(QSharedPointer<ImagePlane> plane);

private:
    /// El frame of reference UID del pla de referència
    QString m_frameOfReferenceUID;

    /// Llista de plans a projectar
    QList<QSharedPointer<ImagePlane>> m_planesToProject;
};

}

#endif
