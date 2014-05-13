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

#ifndef UDGGLUTESSELLATOR_H
#define UDGGLUTESSELLATOR_H

#include "vector3.h"

#include <QList>
#include <QtOpenGL>

#ifdef Q_OS_MACX
   #include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#ifdef Q_OS_WIN32
#define GLU_TESS_CALLBACK __stdcall
#else
#define GLU_TESS_CALLBACK
#endif

namespace udg {

/**
    Tessel·lador basat en GLU per triangular un polígon donat. De moment només accepta polígons sense forats.
 */
class GluTessellator {

public:

    /// Estructura triangle amb els índexs dels vèrtexs que formen el triangle.
    struct Triangle
    {
        int indices[3];
    };

public:

    GluTessellator();
    ~GluTessellator();

    /// Triangula el polígon donat.
    void tessellate(const QList<Vector3> &vertices);
    /// Retorna els vèrtexs dels triangles.
    const QList<Vector3>& getVertices() const;
    /// Retorna els triangles.
    const QList<Triangle>& getTriangles() const;

private:

    ///@{
    /// Callbacks per anar rebent els triangles i vèrtexs generats.
    static void GLU_TESS_CALLBACK nextVertex(int index, GluTessellator *tessellator);
    static void GLU_TESS_CALLBACK edgeFlag(GLboolean flag);
    static void GLU_TESS_CALLBACK newVertex(GLdouble coordinates[3], int neighbourIndices[4], GLfloat neighbourWeights[4], int *newIndex, GluTessellator *tessellator);
    static void GLU_TESS_CALLBACK error(GLenum errorNumber);
    ///@}

    /// Registra les callbacks amb GLU.
    void registerCallbacks();

private:

    /// El tesselador de GLU que fa la feina.
    GLUtesselator *m_tessellator;

    /// Vèrtexs dels triangles.
    QList<Vector3> m_vertices;
    /// Triangles generats.
    QList<Triangle> m_triangles;

    /// Triangle generat actual.
    Triangle m_currentTriangle;
    /// Índex actual del triangle generat actual.
    int m_currentTriangleCurrentIndex;

};

}

#endif
