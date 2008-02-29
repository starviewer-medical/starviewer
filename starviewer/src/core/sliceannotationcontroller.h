/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSLICEANNOTATIONCONTROLLER_H
#define UDGSLICEANNOTATIONCONTROLLER_H

#include <QObject>
#include <QMultiMap>

class vtkProp;

namespace udg {

/**
Aquesta classe s'encarrega de controlar la visibilitat de les annotacions en un entorn de visualització 2D amb vistes axial, sagital i coronal. Conté tres mapes (un per cada vista) que relacionen la llesca amb les annotacions contingudes per un visualitzador 2D.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SliceAnnotationController : public QObject
{
Q_OBJECT
public:
    SliceAnnotationController(QObject *parent = 0);

    ~SliceAnnotationController();

    /// Afegeix un actor a la llesca i vista indicades
    void addActor( vtkProp *actor, int slice, int view );

public slots:
    /// Actualitza la llesca/vista actual en la que es troba el visor 2D associat i els actors corresponents que cal visualitzar
    void setCurrentSlice( int slice );
    void setCurrentView( int view );

    /// Elimina totes les annotacions dels multimaps
    void removeThemAll();

private:
    /// Fa invisibles/visibles els actors d'una llesca i vista donats
    void clearSliceAndView( int slice, int view );
    void raiseSliceAndView( int slice, int view );

    /// Tipus de la llista que ens retornarà el QMultiMap
    typedef QList< vtkProp * > PropsListType;

    /// Donada una llesca i una vista ens retorna la corresponent llista d'actors
    PropsListType getActorsList( int slice, int view );

    /// Amb aquests maps hi guardem les vinculacions de les llesques amb els actors creats. Fent-ho amb vtkProp ens permetrà fer servir qualsevol tipu d'actor (2D i 3D) i assemblies
    QMultiMap< int, vtkProp * > m_axialAnnotations;
    QMultiMap< int, vtkProp * > m_sagitalAnnotations;
    QMultiMap< int, vtkProp * > m_coronalAnnotations;

    /// llesca sobre la que es troba el visor 2D associat
    int m_currentSlice;

    /// vista actual del visor 2D associat
    int m_currentView;
};

}

#endif
