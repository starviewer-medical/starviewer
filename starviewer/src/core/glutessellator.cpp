#include "glutessellator.h"

#include "logging.h"

namespace udg {

GluTessellator::GluTessellator()
{
    m_tessellator = gluNewTess();

    if (!m_tessellator)
    {
        DEBUG_LOG("Creation of tessellation object failed.");
        return;
    }

    registerCallbacks();
}

GluTessellator::~GluTessellator()
{
    gluDeleteTess(m_tessellator);
}

void GluTessellator::tessellate(const QList<Vector3> &vertices)
{
    if (!m_tessellator)
    {
        return;
    }

    m_vertices = vertices;
    m_triangles.clear();
    m_currentTriangleCurrentIndex = 0;

    gluTessBeginPolygon(m_tessellator, this);
    gluTessBeginContour(m_tessellator);
    for (int i = 0; i < vertices.size(); i++)
    {
        GLdouble vertex[3] = { vertices[i].x, vertices[i].y, vertices[i].z };
        gluTessVertex(m_tessellator, vertex, reinterpret_cast<void*>(i));
    }
    gluTessEndContour(m_tessellator);
    gluTessEndPolygon(m_tessellator);
}

const QList<Vector3>& GluTessellator::getVertices() const
{
    return m_vertices;
}

const QList<GluTessellator::Triangle>& GluTessellator::getTriangles() const
{
    return m_triangles;
}

void GluTessellator::nextVertex(int index, GluTessellator *tessellator)
{
    tessellator->m_currentTriangle.indices[tessellator->m_currentTriangleCurrentIndex] = index;
    tessellator->m_currentTriangleCurrentIndex++;
    if (tessellator->m_currentTriangleCurrentIndex == 3)
    {
        tessellator->m_triangles.append(tessellator->m_currentTriangle);
        tessellator->m_currentTriangleCurrentIndex = 0;
    }
}

void GluTessellator::edgeFlag(GLboolean flag)
{
    Q_UNUSED(flag)
}

void GluTessellator::newVertex(GLdouble coordinates[3], int neighbourIndices[4], GLfloat neighbourWeights[4], int *newIndex, GluTessellator *tessellator)
{
    Q_UNUSED(neighbourIndices)
    Q_UNUSED(neighbourWeights)
    *newIndex = tessellator->m_vertices.size();
    tessellator->m_vertices.append(Vector3(coordinates[0], coordinates[1], coordinates[2]));
}

void GluTessellator::error(GLenum errorNumber)
{
    DEBUG_LOG(reinterpret_cast<const char*>(gluErrorString(errorNumber)));
}

void GluTessellator::registerCallbacks()
{
    typedef void (GLU_TESS_CALLBACK *GluTessCallbackType)();
    gluTessCallback(m_tessellator, GLU_TESS_VERTEX_DATA, reinterpret_cast<GluTessCallbackType>(&nextVertex));
    // Aquest ens serveix només perquè registrant-lo forcem GLU a retornar triangles individuals
    gluTessCallback(m_tessellator, GLU_TESS_EDGE_FLAG, reinterpret_cast<GluTessCallbackType>(&edgeFlag));
    gluTessCallback(m_tessellator, GLU_TESS_COMBINE_DATA, reinterpret_cast<GluTessCallbackType>(&newVertex));
    gluTessCallback(m_tessellator, GLU_TESS_ERROR, reinterpret_cast<GluTessCallbackType>(&error));
}

}
