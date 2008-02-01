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
#include <QList>
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

// Fordward declarations
class Volume;
class Series;
class ToolProxy;
class WindowLevelPresetsToolData;

/**
Classe base per a totes les finestres de visualització

@author Grup de Gràfics de Girona  ( GGG )
*/
class QViewer : public QWidget{
Q_OBJECT
public:
    QViewer(QWidget *parent = 0);
    ~QViewer();

    /// Per definir els diferents plans on podem operar
    enum PlaneType{ AxialPlane = 2, SagitalPlane = 0, CoronalPlane = 1, Top2DPlane = 3 };

    /// Definim les diverses orientacions que podem tenir: Axial: XY, Sagital: YZ, Coronal: XZ, Other: orientació arbitrària
    enum CameraOrientationType{ Axial = AxialPlane, Sagital = SagitalPlane, Coronal = CoronalPlane, Other };

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
     * Mètodes per obtenir l'actual i/o l'anterior coordenada del cursor
     * del ratolí durant l'últim event
     * Les coordenades retornades estan en sistema de coordenades de Display
     */
    int getEventPositionX();
    int getEventPositionY();
    int getLastEventPositionX();
    int getLastEventPositionY();

    /**
     * Ens diu si el viewer és actiu en aquell moment
     * @return Cert si actiu, fals altrament
     */
    bool isActive() const;

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

    /**
     * Fem un zoom del requadre definit pels paràmetres topLeft i rightBottom en coordenades de món
     * perquè s'ajusti a la mida de la finestra
     * @param topLeftX coordenada X superior esquerra del requadre
     * @param topLeftY coordenada Y superior esquerra del requadre
     * @param bottomRightX coordenada X inferior dreta del requadre
     * @param bottomRightY coordenada Y inferior dreta del requadre
     */
    void scaleToFit( double topLeftX, double topLeftY, double bottomRightX, double bottomRightY );

    /**
     * Fem un zoom del requadre definit pels paràmetres topLeft i rightBottom en coordenades de món i en 3D
     * perquè s'ajusti a la mida de la finestra
     * @param topLeftX coordenada X superior esquerra del requadre
     * @param topLeftY coordenada Y superior esquerra del requadre
     * @param topLeftZ coordenada Z superior esquerra del requadre
     * @param bottomRightX coordenada X inferior dreta del requadre
     * @param bottomRightY coordenada Y inferior dreta del requadre
     * @param bottomRightZ coordenada Z inferior dreta del requadre
     * @param marginRate valor en percentatge del marge que es vol deixar
     */
    void scaleToFit3D( double topLeftX, double topLeftY, double topLeftZ, double bottomRightX, double bottomRightY, double bottomRightZ, double marginRate );

    /**
     * Ens retorna l'objecte que conté tota la informació referent al window level
     * que es pot aplicar sobre aquest visor
     * @return L'objecte WindowLevelPresetsToolData
     */
    WindowLevelPresetsToolData *getWindowLevelData() const;

    /**
     * Li assignem el window level data externament
     * @param windowLevelData
     */
    void setWindowLevelData( WindowLevelPresetsToolData *windowLevelData );

public slots:
    /// Gestiona els events que rep de la finestra
    virtual void eventHandler( vtkObject * obj, unsigned long event, void * client_data, void *call_data, vtkCommand * command );

#ifdef VTK_QT_5_0_SUPPORT
    /// \TODO Esborrar el metode. Metode de compatibilitat amb VTK 5.0 de vtkQtConnection
    void eventHandler( vtkObject * obj, unsigned long event, void * client_data, vtkCommand * command );
#endif

    /// Força l'execució de la visualització
    virtual void render() = 0;

    /**
     * Assignem si aquest visualitzador és actiu, és a dir, amb el que s'està interactuant
     * @param active
     */
    void setActive( bool active );

    /// Elimina totes les captures de pantalla
    void clearGrabbedViews(){ m_grabList.clear(); };

    /// Activa o desactiva que el manager escolti els events per processar tools.
    /// \deprecated quan s'integri el nou model de tools
    virtual void setEnableTools( bool enable ){};
    virtual void enableTools(){};
    virtual void disableTools(){};

    /// Interroga al tool manager per la tool demanada. Segons si aquesta tool està disponible o no el viewer farà el que calgui. Reimplementat en cada viewer
    /// \deprecated quan s'integri el nou model de tools
    virtual void setTool( QString ){};

    /// Crida que re-inicia a l'estat incial el visor
    virtual void reset() = 0;

    void setSeries(Series *series);

    /// Obté el window/level per defecte
    void getDefaultWindowLevel( double wl[2] );

    /// Obté el window level actual de la imatge
    virtual void getCurrentWindowLevel( double wl[2] ){};

    /// Li indiquem quina vista volem del volum: Axial, Coronal o Sagital
    virtual void resetView( CameraOrientationType view ){};
    virtual void resetViewToAxial(){};
    virtual void resetViewToSagital(){};
    virtual void resetViewToCoronal(){};

public slots:
    /**
     * Activa o desactiva el menú de contexte
     */
    void enableContextMenu();
    void disableContextMenu();

    /// Ajusta ÚNICAMENT els valors de window i level per defecte. No els aplica sobre la imatge.
    /// Mètode de conveniència pels presentation states
    void setDefaultWindowLevel( double window, double level );

    /// Ajusta el window/level
    virtual void setWindowLevel( double window , double level ){};

    /// Reseteja el window level al que tingui per defecte el volum
    virtual void resetWindowLevelToDefault(){};

signals:
    /// informem de l'event rebut. \TODO ara enviem el codi en vtkCommand, però podria (o hauria de) canviar per un mapeig nostre
    void eventReceived( unsigned long eventID );

    /// Signal que s'emet quan s'escull una altra serie per l'input
    void volumeChanged( Volume * );

    /// s'emet quan els paràmetres de la càmera han canviat
    void cameraChanged();

    /// informa que el visualitzador ha rebut un event que es considera com que aquest s'ha seleccionat
    void selected( void );

    /// informa que s'ha canviat el zoom
    void zoomFactorChanged( double );

    /// informa que s'ha mogut la imatge
    void panChanged( double * );

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);

    void contextMenuRelease();

    /**
     * Actualitza les dades contingudes a m_windowLevelData
     */
    void updateWindowLevelData();

    /**
     * Fixem la orientació de la càmera del renderer principal
     * Si el paràmetre donat no és un dels valors enumerats vàlids, no farà res
     * @param orientation Orientació, valors enumerats que podran ser Axial, Sagital o Coronal
     */
    void setCameraOrientation( int orientation );

protected:
    /// El volum a visualitzar
    Volume* m_mainVolume;

    /// El widget per poder mostrar una finestra vtk amb qt
    QVTKWidget* m_vtkWidget;

    /// La llista de captures de pantalla
    QList<vtkImageData *> m_grabList;

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

    /// Dades de valors predeterminats de window level i dels valors actuals que s'apliquen
    WindowLevelPresetsToolData *m_windowLevelData;

    /// Valors dels window level per defecte. Pot venir donat pel DICOM o assignat per nosaltres a un valor estàndar de constrast
    double m_defaultWindow, m_defaultLevel;

private:
    /// Indica si el viewer és actiu o no
    bool m_isActive;
};

};  //  end  namespace udg {

#endif
