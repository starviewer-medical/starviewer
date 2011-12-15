#include "magnifyingglasstooldata.h"

#include "q2dviewer.h"

namespace udg {

MagnifyingGlassToolData::MagnifyingGlassToolData(QObject *parent)
 : ToolData(parent)
{
    m_zoomFactor = 4;
    initializeViewer();
    m_viewerHasBeenShown = false;
}

MagnifyingGlassToolData::~MagnifyingGlassToolData()
{
}

void MagnifyingGlassToolData::setZoomFactor(double factor)
{
    m_zoomFactor = factor;
}

double MagnifyingGlassToolData::getZoomFactor() const
{
    return m_zoomFactor;
}

void MagnifyingGlassToolData::initializeViewer()
{
    m_2DMagnifyingGlassViewer = new Q2DViewer((QWidget*)this->parent());
    m_2DMagnifyingGlassViewer->setWindowFlags(Qt::SplashScreen);
    m_2DMagnifyingGlassViewer->setGeometry(320, 320, 0, 0);
    m_2DMagnifyingGlassViewer->setMinimumSize(320, 320);
    m_2DMagnifyingGlassViewer->setWindowTitle(tr("Magnifying Glass"));
    
    m_2DMagnifyingGlassViewer->removeAnnotation(Q2DViewer::AllAnnotation);
}

Q2DViewer* MagnifyingGlassToolData::get2DMagnifyingGlassViewer() const
{
    return m_2DMagnifyingGlassViewer;
}

void MagnifyingGlassToolData::setViewerShown(bool shown)
{
    m_viewerHasBeenShown = shown;
}

bool MagnifyingGlassToolData::viewerHasBeenShown() const
{
    return m_viewerHasBeenShown;
}

}
