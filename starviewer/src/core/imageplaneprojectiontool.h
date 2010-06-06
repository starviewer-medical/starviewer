/***************************************************************************
 *   Copyright (C) 2010 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGEPLANEPROJECTIONTOOL_H
#define UDGIMAGEPLANEPROJECTIONTOOL_H

#include "tool.h"

// Qt's
#include <QPointer>
#include <QMap>

class QStringList;
class vtkImageReslice;

namespace udg {

class ToolConfiguration;
class ToolData;
class ImagePlaneProjectionToolData;
class Q2DViewer;
class ImagePlane;
class DrawerLine;
class Volume;

class ImagePlaneProjectionTool : public Tool
{
Q_OBJECT
public:
    ImagePlaneProjectionTool( QViewer *viewer, QObject *parent = 0 );
    ~ImagePlaneProjectionTool();

    void handleEvent( long unsigned eventID );

    /// Assignem una configuracio
    void setConfiguration( ToolConfiguration *configuration );

    /// Sobrescriu la funció del pare per assignar-li noves connexions
    void setToolData( ToolData *data );

    /// Indica si la tool està habilitada o no
    void setEnabled( bool enabled );

    /// Indica el número d'imatges a generar al volum de reconstrucció
    /// assignat a un visor amb la tool configurada com a consumidor
    void setNumImagesReconstruction( int numImages );

private slots:
    /// Inicialitza el pla projectat per cada línia de l'actual viewer amb la tool configurada com a productor
    /// Actualitza el volum compartit amb la resta de visors amb la tool activa
    /// Es crida cada cop que el viewer canvïi d'input 
    void initializeImagePlanesUpdteVolume( Volume *volume );
    
    /// Inicialitza el pla projectat per cada línia de l'actual viewer amb la tool configurada com a productor&consumidor
    /// Es crida cada cop que es modifica el volum de les dades compartides
    void initializeImagePlanesCheckingData();
    
    /// Actualitza el pla projectat de cada línia de l'actual viewer amb la tool configurada com a productor 
    /// i ho modifica a les dades compartides
    /// Es crida cada cop que al viewer canvïi d'input, l'imatge o l'slab
    void updateProjections();

    /// Comprova si s'ha modificat el pla projectat per la línia associada a l'actual viewer amb la tool configurada com a consumidor 
    void checkProjectedLineBindUpdated( QString nameProjectedLine );

    /// Comprova si cal actualitzar el pla projectat per alguna línia associada perquè un altre viewer amb la tool configurada com
    /// a productor ha manipulat una línia amb la mateixa orientació
    void checkImagePlaneBindUpdated( QString nameProjectedLine );

    /// Comprova si s'ha modificat el gruix per alguna línia associada perquè l'usuari ha modifcat el número d'imatges a mostrar
    /// a la reconstrucció al visor consumidor.
    void checkThicknessProjectedLineUpdated( QString nameProjectedLine );

private:
    /// Activa les funcionalitats de la tool necessàries segons el tipus de configuració
    /// (connect(signals - slots))
    void enableConnections();

    /// Desactiva les funcionalitats de la tool segons el tipus de configuració
    /// (disconnect(signals - slots))
    void disableConnections();

    /// Comprova que els valors assignats a la configuració siguin correctes
    void checkValuesConfiguration( ToolConfiguration *configuration );
    void checkConfigurationProducer( ToolConfiguration *configuration );
    void checkConfigurationConsumer( ToolConfiguration *configuration );

    /// Aplica els valors de configuració a la tool
    void applyConfiguration();

    /// Inicialitza el reslice del viewer amb la tool configurada com a consumidor
    void initReslice( Volume *volume );

    /// Inicialitza les linies projectades al viewer quan la tool està configurada com a productor
    void initProjectedLines();

    // Inicialitza el pla projectat per una linia de projecció de la tool
    // S'assigna un espaiat, dimensions i límits en funció del tipus d'orientació
    void initializeImagePlane( DrawerLine *projectedLine, Volume *volume );

    /// Actualitza la projecció de la línia indicada amb el pla indicat
    /// També actualitza el pla projectat per la línia a les dades compartides de la tool
    void updateProjection( DrawerLine *projectedLine, ImagePlane *imagePlane );
    void updateProjection( DrawerLine *projectedLine, ImagePlane *imagePlane, bool projectedLineDrawed, bool updateToolData );

    /// Modifica el reslice de l'actual viewer amb la tool configurada com a consumidor perquè
    /// mostri el pla que toca
    void updateReslice( Volume *volume );

    /// Actualitza el viewer amb la tool configurada com a consumidor perquè mostri el pla projectat per la seva línia associada
    void showImagePlaneProjectedLineBind();

    /// Determina si s'està manipulant la línia projectada o no
    void detectManipulationProjectedLine();

    /// Rota la línia de projecció
    void rotateProjectedLine();

    /// Obté l'eix de rotació del pla projectat sobre el viewer
    void getRotationAxisImagePlane( ImagePlane *imagePlane, double axis[3] );

    /// Rota el pla projectat sobre el viewer pel seu centre
    void rotateMiddleImagePlane( ImagePlane *imagePlane, double degrees , double rotationAxis[3] );

    /// Desplaça la línia de projecció
    void pushProjectedLine();

    void releaseProjectedLine();

    /// Mostra o amaga les línies per indicar el gruix del número d'imatges que formen el volum
    /// reconstruït pel visor consumidor
    void applyThicknessProjectedLine( QString nameProjectedLine, DrawerLine *projectedLine );

private:
    /// El volum al que se li practica l'MPR
    Volume *m_volume;
    
    /// Dades específiques de la tool
    ImagePlaneProjectionToolData *m_myData;
    
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Indica l'estat de la tool habilitada o deshabilitada
    bool m_enabled;

    /// Linies projectades: cadascuna indica la intersecció entre un pla projectat que representen i el pla actual del Viewer
    /// Per cada línia es guarda el nom identificador i la seva orientació inicial
    QMap< DrawerLine *, QStringList > m_projectedLines;

    /// Plans projectats per cadascuna de les línies incloses al viewer producer
    /// Per cada línia existirà un nom identificatiu i a aquest se li associarà el pla que projecta
    QMap< QString, ImagePlane *> m_imagePlanes;

    /// Línia de projecció que està manipulant l'usuari
    DrawerLine *m_pickedProjectedLine;

    /// Nom de la línia de projecció que està manipulant l'usuari
    QString m_pickedProjectedLineName;

    /// Nom identificatiu de la linia de projecció que fa modificar el pla mostrat al viewer amb la tool configurada 
    /// com a consumidor. (La línia estarà mostrada a un viewer amb la tool configurada com a productor)
    QString m_nameProjectedLineBind;

    /// Reslice del viewer amb la tool configurada com a consumidor
    vtkImageReslice *m_reslice;

    /// Estat en el que es troba la manipulació de la línia projectada que es manipula
    enum { NONE , ROTATING , PUSHING };
    int m_state;

    /// Per controlar el moviment de la línia projectada a partir de l'interacció de l'usuari
    double m_initialPickX , m_initialPickY;

    /// Indica el gruix a tenir en compte al generar la reconstrucció
    int m_thickness;

    /// Línia que ens marca el límit superior del thickness indicat per l'usuari.
    QPointer<DrawerLine> m_upLineThickness;

    /// Línia que ens marca el límit inferior del thickness indicat per l'usuari.
    QPointer<DrawerLine> m_downLineThickness;
};
}  // end namespace udg

#endif
