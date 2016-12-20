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

#ifndef UDGVOXELINFORMATIONTOOL_H
#define UDGVOXELINFORMATIONTOOL_H

#include "tool.h"

#include "vector3.h"

#include <QPointer>

namespace udg {

class QViewer;
class Q2DViewer;
class DrawerText;
class Volume;

/**
    Tool per mostrar la informació del voxel (coordenades i valor) en un visualitzador 2D
  */
class VoxelInformationTool : public Tool {
Q_OBJECT
public:
    VoxelInformationTool(QViewer *viewer, QObject *parent = 0);
    ~VoxelInformationTool();

    void handleEvent(unsigned long eventID);

private slots:
    /// Actualitza la informació de voxel que ha de mostrar el caption
    /// d'acord amb la posició on estigui el cursor
    void updateCaption();

    /// Crear l'actor on es mostrarà la informació del voxel
    void createCaption();

private:
    /// Returns the voxel value in a properly formatted string corresponding to the given worldCoordinate on the i-th viewer's input
    QString computeVoxelValueOnInput(const Vector3 &worldCoordinate, int i);

    /// Calcula quin és el punt on col·locarem el caption i la justificació del texte corresponent
    /// segons la posició en la que es trobi el punter del mouse.
    Vector3 computeCaptionAttachmentPointAndTextAlignment(QString &horizontalJustification, QString &verticalJustification);

private:
    /// 2DViewer amb el que operem
    Q2DViewer *m_2DViewer;

    /// El texte per mostrar les annotacions de voxel
    QPointer<DrawerText> m_caption;
};

}

#endif
