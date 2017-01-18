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

#ifndef UDGCURSOR3DTOOLDATA_H
#define UDGCURSOR3DTOOLDATA_H

#include "tooldata.h"

#include <QSharedPointer>

namespace udg {

class ImagePlane;

/**
    Classe que guarda les dades compartides per la tool de cursor 3D.
  */
class Cursor3DToolData : public ToolData {
Q_OBJECT
public:
    Cursor3DToolData(QObject *parent = 0);
    ~Cursor3DToolData();

    /// Retorna el frame of reference UID
    QString getFrameOfReferenceUID() const;

    /// Retorna l'instance UID
    QString getInstanceUID() const;

    /// Retorna el pla d'imatge
    QSharedPointer<ImagePlane> getImagePlane() const;

    /// Retorna el punt origen
    double* getOriginPointPosition() const;

    /// Indica que tots els cursors s'han d'amagar
    void hideCursors();

signals:
    /// Indica que el cursor s'ha d'amagar
    void turnOffCursor();

public slots:
    /// Li assignem el frameOfReference del pla de referencia
    /// El frame of reference només pot canviar de valor quan es canvia de sèrie.
    /// Dins d'una mateixa sèrie, totes les imatges tenen el mateix frame of reference
    /// @param frameOfReference
    void setFrameOfReferenceUID(const QString &frameOfReference);

    /// Li assignem l'instance UID
    /// @param instanceUID
    void setInstanceUID(const QString &instanceUID);

    /// Assigna el pla de la imatge de referencia
    /// Aquest pla pot canviar cada cop que es canvia de llesca en el viewer
    /// Quan canvïi aquest valor, s'emetrà el senyal changed()
    /// @param imagePlane
    void setImagePlane(QSharedPointer<ImagePlane> imagePlane);

    /// Assigna una posició al punt origen
    /// Quan canvïi aquest valor, s'emetrà el senyal changed()
    /// @param double
    void setOriginPointPosition(double position[3]);

private:
    /// El frame of reference UID del pla de referència
    QString m_frameOfReferenceUID;

    /// Instance UID de la serie
    QString m_instanceUID;

    /// Pla d'imatge del pla de referència
    QSharedPointer<ImagePlane> m_referenceImagePlane;

    /// Punt a on es troba el cursor 3D
    double* m_originPointPosition;

    /// Visibilitat del cursor
    bool m_isVisible;
};

}

#endif
