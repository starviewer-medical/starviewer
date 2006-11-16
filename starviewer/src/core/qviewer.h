/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQVIEWER_H
#define UDGQVIEWER_H

#include <QWidget>
#include "point.h"
// llista de captures de pantalla
#include <list>
#include <vtkImageData.h>

// Fordward declarations
class QVTKWidget;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkWindowToImageFilter;
class vtkEventQtSlotConnect;

namespace udg {

/**
Classe base per a totes les finestres de visualització

@author Grup de Gràfics de Girona  ( GGG )
*/

// Fordward declarations
class Volume;

class QViewer : public QWidget{
Q_OBJECT
public:
    QViewer(QWidget *parent = 0);
    ~QViewer();

    /// Tipus de fitxer que pot desar
    enum FileType{ PNG , JPEG , TIFF , DICOM , PNM , META , BMP };

    /// Retorna l'interactor renderer
    virtual vtkRenderWindowInteractor *getInteractor();

    /// Retorna el renderer
    virtual vtkRenderer *getRenderer() = 0;

    /// Indiquem les dades d'entrada
    virtual void setInput(Volume* volume) = 0;

    /// Ens retorna el volum d'entrada
    virtual Volume* getInput( void ) { return m_mainVolume; }

    /// Ens retorna el punt del model que es correspon amb el punt de la finestra sobre el qual es troba el cursor desde l'últim event
    Point getModelPointFromCursor() const { return m_modelPointFromCursor; }

    /// Retorna la posició sobre la que es troba el cursor ( coordenades de mon )
    void getCurrentCursorPosition( double xyz[3] )
    {
        xyz[0] = m_currentCursorPosition[0];
        xyz[1] = m_currentCursorPosition[1];
        xyz[2] = m_currentCursorPosition[2];
    }

    /// Retorna el valor de la imatge que hi ha sota el cursor
    double getCurrentImageValue() const { return m_currentImageValue; }

    /// Passa coordenades de display a coordenades de món i viceversa \TODO aquest metode haurioa de ser virtual al pare
    static void computeDisplayToWorld( vtkRenderer *renderer , double x , double y , double z , double worldPoint[3] );
    static void computeWorldToDisplay( vtkRenderer *renderer , double x , double y , double z , double displayPoint[3] );

    /// Fa una captura de la vista actual i la guarda en una estructura interna
    void grabCurrentView();

    /// Desa la llista de captures en un arxiu de diversos tipus amb el nom de fitxer base \c baseName i en format especificat per \c extension. Retorna TRUE si hi havia imatges per guardar, FALSE altrament
    bool saveGrabbedViews( const char *baseName , FileType extension );

    /// Retorna el nombre de vistes capturades que estan desades
    int grabbedViewsCount(){ return m_grabList.size(); }

public slots:
    /// Gestiona els events que rep de la finestra
    virtual void eventHandler( vtkObject * obj, unsigned long event, void * client_data, void *call_data, vtkCommand * command );

    /// Força l'execució de la visualització
    virtual void render() = 0;

    /// Elimina totes les captures de pantalla
    void clearGrabbedViews(){ m_grabList.clear(); };

    /// Activa o desactiva que el manager escolti els events per processar tools.
    virtual void setEnableTools( bool enable ) = 0;
    virtual void enableTools() = 0;
    virtual void disableTools() = 0;

    /// Crida que re-inicia a l'estat incial el visor
    virtual void reset() = 0;

signals:
    /// informem de l'event rebut. \TODO ara enviem el codi en vtkCommand, però podria (o hauria de) canviar per un mapeig nostre
    void eventReceived( unsigned long eventID );

protected:
    /// El volum a visualitzar
    Volume* m_mainVolume;

    /// El widget per poder mostrar una finestra vtk amb qt
    QVTKWidget* m_vtkWidget;

    /// El punt del model que es correspon amb el punt de la finestra sobre el qual es troba el cursor desde l'últim event
    Point m_modelPointFromCursor;

    /// Posició sobre la que es troba el ratolí
    double m_currentCursorPosition[3];

    /// Valor de la imatge corresponent a la posició on es troba el ratolí. Quan la posició està fora del model li assignarem un valor "d'invalidesa" \TODO definir aquest valor, de moment fem servir -1 ( erròniament )
    double m_currentImageValue;

    typedef std::list< vtkImageData * > GrabListType;

    /// La llista de captures de pantalla
    GrabListType m_grabList;

    /// L'iterador de la llista de captures de pantalla
    GrabListType::iterator m_grabListIterator;

    /// Filtre per connectar el que es visualitza pel renderer en un pipeline, epr guardar les imatges en un arxiu, per exemple
    vtkWindowToImageFilter *m_windowToImageFilter;

    /// Connector d'events vtk i slots qt
    vtkEventQtSlotConnect *m_vtkQtConnections;
};

};  //  end  namespace udg {

#endif
