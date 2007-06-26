/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "sliceannotationcontroller.h"
#include "logging.h"

#include <vtkProp.h>

namespace udg {

SliceAnnotationController::SliceAnnotationController(QObject *parent)
 : QObject(parent)
{
}


SliceAnnotationController::~SliceAnnotationController()
{
}

SliceAnnotationController::PropsListType SliceAnnotationController::getActorsList( int slice, int view )
{
    PropsListType list;
    list.clear();
    switch( view )
    {
    case 0: // axial
        list = m_axialAnnotations.values( slice );
    break;

    case 1: // sagittal
        list = m_sagitalAnnotations.values( slice );
    break;

    case 2: // coronal
        list = m_coronalAnnotations.values( slice );
    break;

    default:
        DEBUG_LOG( "Valor no esperat" );
    break;
    }
    return list;
}

void SliceAnnotationController::addActor( vtkProp *actor, int slice, int view )
{
    bool ok = true;
    switch( view )
    {
    case 0: // axial
        m_axialAnnotations.insert( slice, actor );
    break;

    case 1: // sagittal
        m_sagitalAnnotations.insert( slice, actor );
    break;

    case 2: // coronal
        m_coronalAnnotations.insert( slice, actor );
    break;

    default:
        DEBUG_LOG( "Valor no esperat" );
        ok = false;
    break;
    }
    if( ok )
    {
        // si l'actor l'estem afegint en la llesca i vista actuals serà per defecte visible, altrament no
        if( slice == m_currentSlice && view == m_currentSlice )
            actor->VisibilityOn();
        else
            actor->VisibilityOff();
    }
}

void SliceAnnotationController::clearSliceAndView( int slice, int view )
{
    PropsListType listToClear = this->getActorsList( slice, view );
    foreach( vtkProp *actor, listToClear )
    {
        actor->VisibilityOff();
    }
}

void SliceAnnotationController::raiseSliceAndView( int slice, int view )
{
    PropsListType listToView = this->getActorsList( slice, view );
    foreach( vtkProp *actor, listToView )
    {
        actor->VisibilityOn();
    }
}

void SliceAnnotationController::setCurrentSlice( int slice )
{
    // Primer em insvisibles els de la llesca en la que ens trobàvem fins ara, en la corresponent vista
    clearSliceAndView( m_currentSlice, m_currentView );
    // I ara fem visibles els de la nova llesca, en la corresponent vista
    raiseSliceAndView( slice, m_currentView );
    // actualitzem la llesca
    m_currentSlice = slice;
}

void SliceAnnotationController::setCurrentView( int view )
{
    if( m_currentView != view )
    {
        // s'ha canviat de vista, per tant cal netejar la última vista. Quan es faci el set slice de la nova vista, ja es faran visibles els que toquin
        clearSliceAndView( m_currentSlice, m_currentView );
        //actualitzem la niva vista
        m_currentView = view;
    }
    //else: continuem a la mateixa vista, per tant no cal fer res
}

void SliceAnnotationController::removeThemAll()
{
    // recorrem cadscun dels maps i primer retirem l'actor de l'escena i després l'esborrem de la llista
    QMutableMapIterator<int, vtkProp *> axialIterator( m_axialAnnotations );
    axialIterator.toFront();
    while( axialIterator.hasNext() )
    {
        axialIterator.next();
//         m_2DViewer->getRenderer()->RemoveActor( axialIterator.value() );
        axialIterator.remove();
    }

    QMutableMapIterator<int, vtkProp *> sagitalIterator( m_sagitalAnnotations );
    sagitalIterator.toFront();
    while( sagitalIterator.hasNext() )
    {
        sagitalIterator.next();
//         m_2DViewer->getRenderer()->RemoveActor( sagitalIterator.value() );
        sagitalIterator.remove();
    }

    QMutableMapIterator<int, vtkProp *> coronalIterator( m_coronalAnnotations );
    coronalIterator.toFront();
    while( coronalIterator.hasNext() )
    {
        coronalIterator.next();
//         m_2DViewer->getRenderer()->RemoveActor( coronalIterator.value() );
        coronalIterator.remove();
    }
    // refresquem l'escena
//     m_2DViewer->refresh();
}

}
