/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWER_H
#define UDGQ2DVIEWER_H

#include "qviewer.h"

// Fordward declarations
// vtk
class vtkTextActor;
class vtkCornerAnnotation;
class vtkAxisActor2D;
class vtkCoordinate;
class vtkScalarBarActor;
class vtkImageBlend;
class vtkImageActor;
class vtkImageData;
// grayscale pipeline
class vtkImageMapToWindowLevelColors;
// Thick Slab
class vtkProjectionImageFilter;

namespace udg {

// Fordward declarations
class Volume;
class Image;
class Drawer;
class ImagePlane;
// TODO això estarà temporalment pel tema de penjar correctament les imatges de mamo
class HangingProtocolManager;

/**

Classe base per als visualitzadors 2D.

El mode d'operació habitual serà el de visualitar un sol volum.
Normalment per poder visualitzar un volum farem el següent
\code
Q2DViewer* visor = new Q2DViewer();
visor->setInput( volum );

En el cas que desitjem solapar dos volums haurem d'indicar el volum solapat amb el mètode setOverlayInput().
Quan solapem volums tenim 3 maneres de solapar aquests volums, amb un patró de checkerboard, aplicant un blending o un rectilinearWipe,
en aquest cas hauríem de fer servir el mètode setOverlay() indicant una de les opcions, Blend, CheckerBoard o RectilinearWipe
\TODO acabar la doc sobre solapament

Per defecte el visualitzador mostra la primera imatge en Axial. Per les altres vistes (Sagital i Coronal) mostraria la imatge central

Podem escollir quines annotacions textuals i de referència apareixeran en la vista 2D a través dels flags "AnnotationFlags" definits com enums.
Aquests flags es poden passar en el constructor o els podem modificar a través dels mètodes \c addAnnotation() o \c removeAnnotation()
que faran visible o invisible l'anotació indicada. Per defecte el flag és \c AllAnnotation i per tant es veuen totes les anotacions per defecte.

@author Grup de Gràfics de Girona  ( GGG )
*/
class Q2DViewer : public QViewer{
Q_OBJECT
public:
    /// tipus de fusió dels models
    enum OverlayType{ None, Blend , CheckerBoard , RectilinearWipe };

    /// Alineament de la imatge (dreta, esquerre, centrat)
    enum AlignPosition{ AlignCenter, AlignRight, AlignLeft };

    /// Aquests flags els farem servir per decidir quines anotacions seran visibles i quines no
    enum AnnotationFlag{ NoAnnotation = 0x0 , WindowInformationAnnotation = 0x1 , PatientOrientationAnnotation = 0x2 , RulersAnnotation = 0x4 , SliceAnnotation = 0x8, PatientInformationAnnotation = 0x10, AcquisitionInformationAnnotation = 0x20, ScalarBarAnnotation = 0x40,
    AllAnnotation = 0x7F };
    Q_DECLARE_FLAGS(AnnotationFlags, AnnotationFlag)

    Q2DViewer( QWidget *parent = 0 );
    ~Q2DViewer();

    virtual void setInput( Volume *volume );

    /// Ens retorna la vista que tenim en aquells moments del volum
    CameraOrientationType getView() const;

    /// Afegim el volum solapat
    void setOverlayInput( Volume *volume );

    /// Afegim el volum solapat
    Volume *getOverlayInput( void ) { return m_overlayVolume; }

    /// Diem al viewer que s'ha modificat l'overlay per tal que refresqui correctament
    void isOverlayModified();

    /// Canviem l'opacitat del volum solapat
    /// TODO refactoritzar el mètode a setOverlayOpacity() que és l'expressió correcta
    void setOpacityOverlay( double op );

    // Mètodes específics checkerboard
    /// Obtenim el nombre de divisions
    /// TODO set/getDivisions no es fa servir enlloc, es podria
    /// donar com a funcionalitat obsoleta i eliminar-la, ja que ara mateix únicament fa nosa
    int *getDivisions();
    void getDivisions( int data[3] );

    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int data[3] );
    void setDivisions( int x , int y , int z );

    /// Obté el window level actual de la imatge
    /// TODO els mètodes no es criden enlloc, mirar si són necessaris o no
    double getCurrentColorWindow();
    double getCurrentColorLevel();
    void getCurrentWindowLevel( double wl[2] );

    /// Retorna la llesca/fase actual
    int getCurrentSlice() const;
    int getCurrentPhase() const;

    /**
     * Ens retorna el drawer per poder pintar-hi primitives
     * @return Objecte drawer del viewer
     */
    Drawer *getDrawer() const;

    /**
     * Calcula la posició del cursor en coordenades de món
     * @param xyz[] La posició que obtenim
     * @return Cert si el cursor està sobre una posició vàlida de la imatge, fals altrament
     */
    bool getCurrentCursorPosition( double xyz[3] );

    /**
     * Retorna el valor del voxel allà on es troba el cursor en aquell moment
     * @return valor del voxel
     */
    double getCurrentImageValue();

    /**
     * Ens retorna la imatge que s'està visualitzant en aquell moment
     * TODO pot ser que aquest mètode sigui purament de conveniència per la tool de reference lines i desaparegui
     * en un futur o sigui substituit per un altre
     * @return Objecte Image corresponent al que es visualitza en aquell moment. NULL si no es veu cap imatge
     */
    Image *getCurrentDisplayedImage() const;

    /**
     * Ens dóna el pla d'imatge actual que estem visualitzant
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     * ATENCIÓ: Només es donarà aquest paràmetre (amb valor true) en casos que realment se sàpiga el que s'està fent!
     * @return El pla imatge actual
     */
    ImagePlane *getCurrentImagePlane( bool vtkReconstructionHack = false );

    /**
     * Ens dóna el pla d'imatge de la llesca
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     * ATENCIÓ: Només es donarà aquest paràmetre (amb valor true) en casos que realment se sàpiga el que s'està fent!
     * @return El pla imatge de la llesca
     */
    ImagePlane *getImagePlane( int sliceNumber, int phaseNumber, bool vtkReconstructionHack = false );

    /**
     * donat un punt 3D en espai de referència DICOM, ens dóna la projecció d'aquest punt sobre
     * el pla actual, transformat a coordenades de món VTK
     * @param pointToProject[]
     * @param projectedPoint[]
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     */
    void projectDICOMPointToCurrentDisplayedImage( const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack = false );

    /// Retorna el thickness
    double getThickness();

    /**
     * Ens retorna el rang actual de llesques, tenint en compte totes les imatges,
     * tant com si hi ha fases com si no
     * @param min valor mínim
     * @param max valor màxim
     */
    /// TODO podria ser protected o private, ningú necessita aquestes dades fora del visor
    void getSliceRange(int &min, int &max);
    int *getSliceRange();

    /**
     * Ens dóna la llesca mínima/màxima de llesques, tenint en compte totes les imatges,
     * tant com si hi ha fases com si no
     * @return valor de la llesca mínima/màxima
     */
    int getMinimumSlice();
    int getMaximumSlice();

    /**
     * Ens indica si s'està aplicant o no thick slab
     * @return
     */
    bool isThickSlabActive() const;

    /**
     * Obtenim el mode de projecció del thickslab.
     * Si el thickslab no està actiu, el valor és indefinit
     * @return
     */
    int getSlabProjectionMode() const;

    /**
     * Obtenim el gruix de l'slab
     * Si el thickslab no està actiu, el valor és indefinit
     * @return
     */
    int getSlabThickness() const;

    /// Donada les coordenades x,y d'on s'ha clicat de la pantalla, retorna el punt més proper i que caigui dins del model.
    double *pointInModel( int screen_x, int screen_y );

    ///Retorna la informació de la llesca actual del visualitzador
    vtkImageData *getCurrentSlabProjection();

    /// Busca la llesca que es troba més a prop del punt i retorna la distancia
    int getNearestSlice( double point[3], double &distance );

    /// Retorna un vector de 4 strings en el que tenim quatre elements que representen les etiquetes
    /// indicant on està la dreta/esquerra, cap/peu, davant/darrere del pacient
    /// El primer element correspon a la esquerra de la imatge, el segon el damunt, el tercer a la dreta i el quart a sota
    /// Si tenim una imatge axial pura la llista seria R,H,L,F (Right, Head, Left, Feet )
    QVector<QString> getCurrentDisplayedImageOrientationLabels() const;

    /// Ens diu quin és el pla de projecció de la imatge que es veu en aquell moment
    /// Valors: AXIAL, SAGITAL, CORONAL, OBLIQUE o N/A
    QString getCurrentPlaneProjectionLabel() const;

    /// Retorna el filtre de mapeig de window level.
    /// TODO El fan servir les extensions que necessiten aplicar una escala de colors
    /// sobre la imatge. Caldria pensar en un mètode de poder aplicar color sense exposar
    /// aquest component intern a l'exterior
    vtkImageMapToWindowLevelColors *getWindowLevelMapper() const;

public slots:
    void resetView( CameraOrientationType view );
    void resetViewToAxial();
    void resetViewToCoronal();
    void resetViewToSagital();

    /// Restaura el visualitzador a l'estat inicial
    void restore();

    /// Esborra totes les primitives del visor
    void clearViewer();

    /// Canvia el WW del visualitzador, per tal de canviar els blancs per negres, i el negres per blancs
    void invertWindowLevel();

    virtual void render();
    // TODO donar aquest per deprecated i fer servir restore() en comptes
    void reset();

    /// canvia la llesca que veiem de la vista actual
    void setSlice(int value);

    /// canvia la fase en que es veuen les llesques si n'hi ha
    void setPhase( int value );

    /// indica el tipu de solapament dels volums, per defecte checkerboard
    void setOverlay( OverlayType overlay );
    void setNoOverlay();
    void setOverlayToBlend();
    void setOverlayToCheckerBoard();
    void setOverlayToRectilinearWipe();

    /// Afegir o treure la visibilitat d'una anotació textual/gràfica
    void enableAnnotation( AnnotationFlags annotation, bool enable = true );
    void removeAnnotation( AnnotationFlags annotation );

    void setWindowLevel(double window, double level);

    /// L'únic que fa és emetre el senyal seedPositionChanged, per poder-ho cridar desde la seedTool
    /// TODO aquest mètode hauria de quedar obsolet
    void setSeedPosition( double pos[3] );

    /// Aplica una rotació de 90 graus en el sentit de les agulles del rellotge
    /// tantes "times" com li indiquem, per defecte sempre serà 1 "time"
    void rotateClockWise( int times = 1 );

    /// Aplica una rotació de 90 graus en el sentit contrari a les agulles del rellotge
    /// tantes "times" com li indiquem, per defecte sempre serà 1 "time"
    void rotateCounterClockWise( int times = 1 );

    /// Aplica una rotació absoluta de (90º * factor) mod 360 sobre la imatge. Per tant es fa la rotació com si la imatge estigués amb una rotació inicial de 0º
    void setRotationFactor( int factor );

    /// Aplica un flip horitzontal/vertical sobre la imatge. El flip vertical es farà com una rotació de 180º seguida d'un flip horitzontal
    void horizontalFlip();
    void verticalFlip();

    /// Re-inicia la càmera en la vista actual. Posa els paràmetres de rotació, zoom, desplaçament, flip, etc. als seus valors inicials
    void resetCamera();

    // TODO aquests mètodes també haurien d'estar en versió QString!
    /**
     * Li indiquem quin mode de projecció volem aplicar sobre l'slab
     * @param projectionMode valor que identifica quina projecció apliquem
     */
    void setSlabProjectionMode( int projectionMode );

    /**
     * Indiquem el gruix de l'slab
     * @param thickness Nombre de llesques que formen l'slab
     */
    void setSlabThickness( int thickness );

    /**
     * Activem a desactivem l'aplicació del thick slab sobre la imatge
     * @param enable
     */
    void enableThickSlab( bool enable = true );

    /// Alineament de la imatge dins del visualitzador
    void alignLeft();
    void alignRight();

    // Posa la posició d'alineament de la imatge (dreta, esquerre, centrat )
    void setAlignPosition( AlignPosition alignPosition );

signals:
    /// envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// envia la nova fase en la que ens trobem
    void phaseChanged(int);

    /// envia la nova vista en la que ens trobem
    void viewChanged(int);

    /// indica el nou window level
    void windowLevelChanged( double window , double level );

    /// Senyal que s'envia quan la llavor s'ha canviat
    /// TODO mirar de treure-ho i posar-ho en la tool SeedTool
    void seedPositionChanged(double,double,double);

    /**
     * S'emet quan canvia l'slab thickness
     * @param thickness nou valor de thickness
     */
    void slabThicknessChanged(int thickness);

    /// Senyal que s'envia quan ha canviat l'overlay
    void overlayChanged();
    void overlayModified();

protected:
    /// Processem l'event de resize de la finestra Qt
    virtual void resizeEvent( QResizeEvent *resize );

private:
    /**
     * Refresca la visibilitat de les annotacions en funció dels flags que tenim
     */
    void refreshAnnotations();

    /// Actualitzem les dades de les annotacions, per defecte totes, sinó, només les especificades
    void updateAnnotationsInformation( AnnotationFlags annotation = Q2DViewer::AllAnnotation );

    /// Desglossem les actualitzacions de les diferents informacions que es mostren per pantalla
	void updatePatientAnnotationInformation();
    void updateSliceAnnotationInformation();

    /// Refresca els valors de les annotacions de llesca. Si els valors referents
    /// a les fases són < 2 no es printarà informació de fases
    /// Si hi ha thick slab, mostrarà el rang d'aquest
    void updateSliceAnnotation( int currentSlice, int maxSlice, int currentPhase = 0, int maxPhase = 0 );

    /// Crea i inicialitza totes les anotacions que apareixeran per pantalla
    void createAnnotations();

    /// crea els indicadors d'escala
    void createRulers();

    /// Crea la barra de valors
    void createScalarBar();

    /// Actualitza la barra de valors
    void updateScalarBar();

    /// crea les anotacions de l'orientació del pacient
    void createOrientationAnnotations();

    /// Afegeix tots els actors a l'escena
    void addActors();

    /// Helper method, donada una etiqueta d'orientació, ens retorna aquesta etiqueta però amb els valors oposats.
    /// Per exemple, si l'etiqueta que ens donen és RPI (Right-Posterior,Inferior), el valor retornat seria LAS (Left-Anterior-Superior)
    /// Les etiquetes vàlides i els seus oposats són les següents:
    /// R:L (Right-Left), A:P (Anterior-Posterior), S:I (Superior-Inferior), H:F(Head-Feet)
    static QString getOppositeOrientationLabel( const QString &label );

    /// A partir de l'string d'orientació del pacient mapeja les anotacions correctes segons com estem mirant el model. A això li afecta també si la vista és axial, sagital o coronal
    void mapOrientationStringToAnnotation();

    /// Actualitza les característiques dels actors dels viewports
    void updateDisplayExtent();

    /// Aplica el pipeline d'escala de grisos segons la modality, voi i presentation lut's que s'hagin calculat.
    /// Això permet que el càlcul s'hagi fet en un presentation state, per exemple
    /// TODO És públic únicament perquè el fa servir el presentation state attacher. Podria ser protected o private.
    void applyGrayscalePipeline();

    /// thick slab
    void computeRangeAndSlice( int newSlabThickness );

    ///  Valida el valor d'slice donat i actualitza les variables membres pertinents, com m_currentSlice o m_firstSlabSlice
    void checkAndUpdateSliceValue( int value );

private slots:
    /// Actualitza les transformacions de càmera ( de moment rotació i flip )
    void updateCamera();

    /// Actualitza els rulers
    void updateRulers();

protected:
    /// Actor d'imatge
    vtkImageActor *m_imageActor;

    /// conserva la vista actual
    CameraOrientationType m_lastView;

    /// La llesca actual que estem visualitzant
    int m_currentSlice;

    /// La fase (instant de temps) de les llesques
    int m_currentPhase;

    /// Aquest és el segon volum afegit a solapar
    Volume *m_overlayVolume;

    /// Aquest és el blender per veure imatges fusionades
    vtkImageBlend* m_blender;

    /// Opacitat del segon volume
    double m_opacityOverlay;

    /// El nombre de divisions per cada dimensió
    int m_divisions[3];

    /// Annotacions de texte referents a informació de la sèrie
    /// (nom de pacient, protocol,descripció de sèrie, data de l'estudi, etc)
    /// i altre informació rellevant ( nº imatge, ww/wl, etc )
    vtkCornerAnnotation *m_cornerAnnotations;

private:
    /// flag que ens indica quines anotacions es veuran per la finestra
    AnnotationFlags m_enabledAnnotations;

    /// Tipus de solapament dels volums en cas que en tinguem més d'un
    OverlayType m_overlay;

    /// Els strings amb els textes de cada part de la imatge
    QString m_lowerLeftText, m_lowerRightText, m_upperLeftText, m_upperRightText;

    /// Aquest string indica les anotacions que ens donen les referències del pacient ( Right,Left,Posterior,Anterior,Inferior,Superior) TODO aquesta variable no s'està fent servir :( decidir si ens pot resultar útil o no
    QString m_patientOrientationText[4];

    /// Marcadors que indicaran les mides relatives del model en les dimensions x,y i z ( ample , alçada i profunditat ). Al ser visor 2D en veurem només dues. Aquestes variaran en funció de la vista en la que ens trobem.
    vtkAxisActor2D *m_sideRuler, *m_bottomRuler;

    /// Coordenades fixes dels rulers que els ajustaran a un dels extrems inferiors/superiors o laterals de la pantalla
    vtkCoordinate *m_anchoredRulerCoordinates;

    /// coordenades dels extrems per cada dimensió del ruler
    double m_rulerExtent[6];

    /// Textes adicionals d'anotoació
    vtkTextActor *m_patientOrientationTextActor[4];

    /// Mides (x,y) de la imatge que mostrarem com informació adicional
    int m_imageSizeInformation[2];

    /// Barra que mostra l'escala de colors del model que estem visualitzant \TODO quan tinguem models fusionats tindrem una o dues barres d'escala de colors?
    vtkScalarBarActor *m_scalarBar;

    /// Factor de rotació. En sentit de les agulles del rellotge 0: 0º, 1: 90º, 2: 180º, 3: 270º.
    int m_rotateFactor;

    /// ampliació tractament dinàmic
    /// Nombre de fases
    int m_numberOfPhases;

    /// Valor màxim al que pot arribar m_currentSlice
    int m_maxSliceValue;

    /// Indica si cal aplicar un flip horitzontal o no sobre la càmera
    bool m_applyFlip;

    /// Aquesta variable controla si la imatge està flipada respecte la seva orientació original. Útil per controlar annotacions.
    bool m_isImageFlipped;

    /// Especialista en dibuixar primitives
    Drawer *m_drawer;

    /// objectes per a les transformacions en el pipeline d'escala de grisos
    vtkImageMapToWindowLevelColors *m_windowLevelLUTMapper;

    // Secció "ThickSlab"
    /// Nombre de llesques que composen el thickSlab
    int m_slabThickness;

    /// Filtre per composar les imatges del thick slab
    vtkProjectionImageFilter *m_thickSlabProjectionFilter;

    /// Variables per controlar el rang de llesques en el que es troba l'slab
    int m_firstSlabSlice, m_lastSlabSlice;

    /// Variable per controlar si el thickSlab s'està aplicant o no.
    /// Perquè sigui true, m_slabThickness > 1, altrament false
    bool m_thickSlabActive;

    /// Indica quin tipus de projecció apliquem sobre l'slab
    int m_slabProjectionMode;

    // TODO això estarà temporalment pel tema de penjar correctament les imatges de mamo
    HangingProtocolManager *m_hangingProtocolManager;

    /// Posició a on s'ha d'alinear la imatge ( dreta, esquerre o centrat )
    AlignPosition m_alignPosition;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(Q2DViewer::AnnotationFlags)
};  //  end  namespace udg

#endif
