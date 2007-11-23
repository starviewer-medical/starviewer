/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQVIEWER_H
#define UDGQVIEWER_H

#include <QWidget>
// llista de captures de pantalla
#include <list>
#include <vtkImageData.h>

// Fordward declarations
class QVTKWidget;
class vtkRenderer;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkInteractorStyle;
class vtkWindowToImageFilter;
class vtkEventQtSlotConnect;

namespace udg {

/**
Classe base per a totes les finestres de visualització

@author Grup de Gràfics de Girona  ( GGG )
*/

// Fordward declarations
class Volume;
class Series;
class ToolProxy;

class QViewer : public QWidget{
Q_OBJECT
public:
    QViewer(QWidget *parent = 0);
    ~QViewer();

    /// Tipus de fitxer que pot desar
    enum FileType{ PNG , JPEG , TIFF , DICOM , PNM , META , BMP };

    /// Retorna l'interactor renderer
    virtual vtkRenderWindowInteractor *getInteractor();

    /// Retorna l'interactor style
    virtual vtkInteractorStyle *getInteractorStyle();

    /// Retorna el renderer
    virtual vtkRenderer *getRenderer() = 0;

    /// Indiquem les dades d'entrada
    virtual void setInput(Volume* volume) = 0;

    /// Ens retorna el volum d'entrada
    virtual Volume* getInput( void ) { return m_mainVolume; }

    /// retorna el vtkRenderWindow
    vtkRenderWindow *getRenderWindow();

    /**
     * Ens retorna el ToolProxy del viewer
     * @return ToolProxy del viewer
     */
    ToolProxy *getToolProxy() const;

    /// Passa coordenades de display a coordenades de món i viceversa \TODO aquest metode haurioa de ser virtual al pare
    static void computeDisplayToWorld( vtkRenderer *renderer , double x , double y , double z , double worldPoint[4] );
    static void computeWorldToDisplay( vtkRenderer *renderer , double x , double y , double z , double displayPoint[3] );

    /// Fa una captura de la vista actual i la guarda en una estructura interna
    void grabCurrentView();

    /// Desa la llista de captures en un arxiu de diversos tipus amb el nom de fitxer base \c baseName i en format especificat per \c extension. Retorna TRUE si hi havia imatges per guardar, FALSE altrament
    bool saveGrabbedViews( QString baseName , FileType extension );

    /// Retorna el nombre de vistes capturades que estan desades
    int grabbedViewsCount(){ return m_grabList.size(); }

    /// Refresca l'escena amb el RenderWindowInteractor. És sobretot per haver d'escriure menys.
    void refresh();

    /**
     * Fa zoom sobre l'escena amb el factor donat
     * @param factor Factor de zoom que volem aplicar a la càmera
     */
    void zoom( double factor );

    /**
     * Desplaça la càmera segons el vector de moviment que li passem
     * @param motionVector[] Vector de moviment que determina cap on i quant es mou la càmera
     */
    void pan( double motionVector[3] );

public slots:
    /// Gestiona els events que rep de la finestra
    virtual void eventHandler( vtkObject * obj, unsigned long event, void * client_data, void *call_data, vtkCommand * command );

#ifdef VTK_QT_5_0_SUPPORT
    /// \TODO Esborrar el metode. Metode de compatibilitat amb VTK 5.0 de vtkQtConnection
    void eventHandler( vtkObject * obj, unsigned long event, void * client_data, vtkCommand * command );
#endif

    /// Força l'execució de la visualització
    virtual void render() = 0;

    /// Elimina totes les captures de pantalla
    void clearGrabbedViews(){ m_grabList.clear(); };

    /// Activa o desactiva que el manager escolti els events per processar tools.
    virtual void setEnableTools( bool enable ) = 0;
    virtual void enableTools() = 0;
    virtual void disableTools() = 0;

    /// Interroga al tool manager per la tool demanada. Segons si aquesta tool està disponible o no el viewer farà el que calgui. Reimplementat en cada viewer
    virtual void setTool( QString ) = 0;

    /// Crida que re-inicia a l'estat incial el visor
    virtual void reset() = 0;

    void setSeries(Series *series);

public slots:
    /**
     * Activa o desactiva el menú de contexte
     */
    void enableContextMenu();
    void disableContextMenu();

signals:
    /// informem de l'event rebut. \TODO ara enviem el codi en vtkCommand, però podria (o hauria de) canviar per un mapeig nostre
    void eventReceived( unsigned long eventID );

    /// Signal que s'emet quan s'escull una altra serie per l'input
    void volumeChanged( Volume * );

    /// s'emet quan els paràmetres de la càmera han canviat
    void cameraChanged();

    /// informa que el visualitzador ha rebut un event que es considera com que aquest s'ha seleccionat
    void selected( void );

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    void contextMenuRelease();

protected:
    /// El volum a visualitzar
    Volume* m_mainVolume;

    /// El widget per poder mostrar una finestra vtk amb qt
    QVTKWidget* m_vtkWidget;

    typedef std::list< vtkImageData * > GrabListType;

    /// La llista de captures de pantalla
    GrabListType m_grabList;

    /// L'iterador de la llista de captures de pantalla
    GrabListType::iterator m_grabListIterator;

    /// Filtre per connectar el que es visualitza pel renderer en un pipeline, epr guardar les imatges en un arxiu, per exemple
    vtkWindowToImageFilter *m_windowToImageFilter;

    /// Connector d'events vtk i slots qt
    vtkEventQtSlotConnect *m_vtkQtConnections;

    /// indica si el menú de contexte està actiu o no. Per defecte sempre està actiu
    bool m_contextMenuActive;

    /// ToolProxy
    ToolProxy *m_toolProxy;

    /// Ens servirà per controlar si entre event o event s'ha mogut el mouse
    bool m_mouseHasMoved;
};

};  //  end  namespace udg {

#endif
