/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qtabaxisview.h"

#include <QSlider>
#include <QSpinBox>

#include "volume.h"
#include "q2dviewer.h"
#include "q3dviewer.h"

namespace udg {

QTabAxisView::QTabAxisView(QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
}

QTabAxisView::~QTabAxisView()
{
}

void QTabAxisView::setInput(Volume* newImageData)
{
    
    this->m_coronalView->setInput( newImageData ); // XZ
    this->m_sagitalView->setInput( newImageData ); // YZ
    this->m_axialView->setInput( newImageData ); // XY
 
    this->m_coronalView->setView( Q2DViewer::Coronal );
    this->m_sagitalView->setView( Q2DViewer::Sagittal );
    this->m_axialView->setView( Q2DViewer::Axial );
  
    this->m_coronalView->render();
    this->m_sagitalView->render();
    this->m_axialView->render();

    createConnections();
    
    int *size = newImageData->getVtkData()->GetDimensions();

    this->m_xAxisSlider->setMaximum( size[0]-1 );// Sagital
    this->m_yAxisSlider->setMaximum( size[1]-1 );// coronal
    this->m_zAxisSlider->setMaximum( size[2]-1 );// axial
    
    this->m_xAxisSpinBox->setMaximum( size[0]-1 );// sagital
    this->m_yAxisSpinBox->setMaximum( size[1]-1 );// coronal
    this->m_zAxisSpinBox->setMaximum( size[2]-1 );// axial

    // Posem tots els sliders i valors spinboxs a z/2. Els sliders automaticament s'hi posaran
    // igual que les imatges tb. (gracies a la propagacio de signals-slots)
    this->m_xAxisSpinBox->setValue( size[0]/2 );// sagital
    this->m_yAxisSpinBox->setValue( size[1]/2 );// coronal
    this->m_zAxisSpinBox->setValue( size[2]/2);// axial
    
    // vista 3D
    this->m_3DView->setInput( newImageData );
    this->m_3DView->setRenderFunctionToRayCasting();
    this->m_3DView->render();

    
} 

void QTabAxisView::createConnections()
{
    connect( m_zAxisSlider , SIGNAL( valueChanged(int) ) , m_zAxisSpinBox , SLOT( setValue(int) ) );
    connect( m_zAxisSpinBox , SIGNAL( valueChanged(int) ) , m_axialView , SLOT( setSlice(int) ) );
    connect( m_axialView , SIGNAL( sliceChanged(int) ) , m_zAxisSlider , SLOT( setValue(int) ) );

    connect( m_xAxisSlider , SIGNAL( valueChanged(int) ) , m_xAxisSpinBox , SLOT( setValue(int) ) );
    connect( m_xAxisSpinBox , SIGNAL( valueChanged(int) ) , m_sagitalView , SLOT( setSlice(int) ) );
    connect( m_sagitalView , SIGNAL( sliceChanged(int) ) , m_xAxisSlider , SLOT( setValue(int) ) );

    connect( m_yAxisSlider , SIGNAL( valueChanged(int) ) , m_yAxisSpinBox , SLOT( setValue(int) ) );
    connect( m_yAxisSpinBox , SIGNAL( valueChanged(int) ) , m_coronalView , SLOT( setSlice(int) ) );
    connect( m_coronalView , SIGNAL( sliceChanged(int) ) , m_yAxisSlider , SLOT( setValue(int) ) );
}

};  // end namespace udg {
