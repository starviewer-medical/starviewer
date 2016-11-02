/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGQ2DVIEWER_H
#define UDGQ2DVIEWER_H

#include "qviewer.h"
#include "annotationflags.h"
#include "anatomicalplane.h"
#include "volumedisplayunit.h"

#include <QPointer>

// Fordward declarations
// Vtk
class vtkCoordinate;
class vtkImageData;
class vtkImageSlice;

namespace udg {

// Fordward declarations
class Image;
class ImageOverlay;
class Drawer;
class DrawerBitmap;
class ImagePlane;
class ImageOrientationOperationsMapper;
class VolumeReaderManager;
class QViewerCommand;
class PatientOrientation;
class BlendFilter;
class VolumeDisplayUnit;
class VolumePixelData;
class Q2DViewerAnnotationHandler;
class VolumeDisplayUnitHandlerFactory;
class GenericVolumeDisplayUnitHandler;
class TransferFunction;
class TransferFunctionModel;

/**
    Classe base per als visualitzadors 2D.

    El mode d'operació habitual serà el de visualitar un sol volum.
    Normalment per poder visualitzar un volum farem el següent
    \code
    Q2DViewer* visor = new Q2DViewer();
    visor->setInput(volum);

    En el cas que desitjem solapar dos volums haurem d'indicar el volum solapat amb el mètode setOverlayInput().
    Quan solapem volums tenim 1 manera de solapar aquests volums, aplicant un blending,
    en aquest cas hauríem de fer servir el mètode setOverlapMethod() indicant una de les opcions (de moment únicament Blend)
    \TODO acabar la doc sobre solapament

    Per defecte el visualitzador mostra la primera imatge en Axial. Per les altres vistes (Sagital i Coronal) mostraria la imatge central

    Podem escollir quines annotacions textuals i de referència apareixeran en la vista 2D a través dels flags "AnnotationFlags" a través dels mètodes
    \c enableAnnotation() o \c removeAnnotation() que faran visible o invisible l'anotació indicada.
    Per defecte el flag és \c AllAnnotation i per tant es veuen totes les anotacions per defecte.
  */
class Q2DViewer : public QViewer {
Q_OBJECT
public:
    /// Tipus de solapament dels models
    enum OverlapMethod { None, Blend };

    /// Alineament de la imatge (dreta, esquerre, centrat)
    enum AlignPosition { AlignCenter, AlignRight, AlignLeft };

    Q2DViewer(QWidget *parent = 0);
    ~Q2DViewer();

    /// Ens retorna la vista que tenim en aquells moments del volum
    OrthogonalPlane getView() const;

    /// Return the view plane on the specified input. If i is out of range, default constructed value will be returned.
    OrthogonalPlane getViewOnInput(int i) const;

    Volume* getMainInput() const;

    /// Gets the i-th input. If i is out of range, null is returned
    Volume* getInput(int i) const;

    QList<Volume*> getInputs() const;

    int getNumberOfInputs() const;

    /// Ens retorna el drawer per poder pintar-hi primitives
    /// @return Objecte drawer del viewer
    Drawer* getDrawer() const;

    /// Returns the VOI LUT that is currently applied to the image in this viewer.
    virtual VoiLut getCurrentVoiLut() const;

    /// Retorna la llesca/fase actual
    int getCurrentSlice() const;
    int getCurrentPhase() const;

    /// Gets the current slice on the specified input. If i is out of range, 0 will be returned
    int getCurrentSliceOnInput(int i) const;
    
    /// Gets the current phase on the specified input. If i is out of range, 0 will be returned
    int getCurrentPhaseOnInput(int i) const;

    /// Calcula la coordenada de la imatge que es troba per sota del cursor en coordenades de món
    /// En el cas el cursor estigui fora de la imatge, la coordenada no té cap validesa
    /// @param xyz[] La coordenada de la imatge, en sistema de coordenades de món
    /// @return Cert si el cursor es troba dins de la imatge, fals altrament
    bool getCurrentCursorImageCoordinate(double xyz[3]);

    /// Same as getCurrentCursorImageCoordinate with the chance to specify from which input we want to get the coordinate.
    /// If i is out of range, false will be returned
    bool getCurrentCursorImageCoordinateOnInput(double xyz[3], int i);
    
    /// Returns current displayed image.
    /// If some orthogonal reconstruction different from original acquisition is applied, returns null
    Image* getCurrentDisplayedImage() const;

    /// Same as getCurrentDisplayedImage() but returning the corresponding image on the specified input
    Image* getCurrentDisplayedImageOnInput(int i) const;

    /// Ens dóna el pla d'imatge actual que estem visualitzant
    /// @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
    /// per casos en que el pla "real" no és el que volem i necessitem una petita modificació
    /// ATENCIÓ: Només es donarà aquest paràmetre (amb valor true) en casos que realment se sàpiga el que s'està fent!
    /// @return El pla imatge actual
    ImagePlane* getCurrentImagePlane(bool vtkReconstructionHack = false);

    /// Returns the laterality corresponding to the current displayed image.
    /// If image is not reconstructed, image laterality is returned, or series laterality if not present
    /// If image is reconstructed, series laterality is returned
    /// If no laterality is found, en empty character will be returned
    QChar getCurrentDisplayedImageLaterality() const;

    /// Retorna el thickness. En cas que no disposem del thickness, el valor retornat serà 0.0
    double getCurrentSliceThickness() const;

    /// Retorna la orientació de pacient corresponent a la imatge que s'està visualitzant en aquell moment,
    /// és a dir, tenint en compte rotacions, flips, reconstruccions, etc.
    PatientOrientation getCurrentDisplayedImagePatientOrientation() const;

    /// Ens diu quin és el pla de projecció de la imatge que es veu en aquell moment
    /// Valors: AXIAL, SAGITAL, CORONAL, OBLIQUE o N/A
    QString getCurrentAnatomicalPlaneLabel() const;

    /// Returns current anatomical plane as AnatomicalPlaneType
    AnatomicalPlane getCurrentAnatomicalPlane() const;

    /// Retorna l'espai que hi ha entre les llesques segons la vista actual i si hi ha el thickness activat
    double getCurrentSpacingBetweenSlices();

    /// Returns the depth (z coordinate value) of the main displayed image
    double getCurrentDisplayedImageDepth() const;
    
    /// Returns the depth (z coordinate value) of the displayed image from the specified input
    double getCurrentDisplayedImageDepthOnInput(int i) const;

    /// Gets the pixel data corresponding to the current rendered image
    SliceOrientedVolumePixelData getCurrentPixelData();

    /// Gets the pixel data corresponding to the current rendered image from the specified input.
    SliceOrientedVolumePixelData getCurrentPixelDataFromInput(int i);

    /// Restores the standard rendering quality in this viewer.
    void restoreRenderingQuality();
    
    /// Ens dóna la llesca mínima/màxima de llesques, tenint en compte totes les imatges,
    /// tant com si hi ha fases com si no
    /// @return valor de la llesca mínima/màxima
    int getMinimumSlice() const;
    int getMaximumSlice() const;

    /// Returns the total number of slices on the spatial dimension that has the main input on the current view
    int getNumberOfSlices() const;

    /// Returns the total number of phases that has the main input. It only applies to the original acquisition view plane. The minimum number of phases will be 1.
    int getNumberOfPhases() const;

    /// Returns the total number of phases from the specified input. It only applies to the original acquisition view plane. The minimum number of phases will be 1.
    /// If i is out of range, 1 will be returned
    int getNumberOfPhasesFromInput(int i) const;

    /// Returns true if the number of phases is greater than 1
    bool hasPhases() const;

    /// Returns true if the number of phases is greater than 1 on the specified input. If i is out of range, false will be returned.
    bool doesInputHavePhases(int i) const;
    
    /// Returns true if the current image of the selected input is visible, false otherwise
    bool isInputVisible(int i) const;
    
    /// Ens indica si s'està aplicant o no thick slab
    bool isThickSlabActive() const;
    
    /// Ask is thickslab is active on the i-th input. If i is out of range, false will be returned
    bool isThickSlabActiveOnInput(int i) const;

    /// Returns current slab projection mode.
    VolumeDisplayUnit::SlabProjectionMode getSlabProjectionMode() const;

    /// Returns current slab thickness in mm. If thickslab is disabled, returns 0.
    double getSlabThickness() const;

    /// Returns the maximum slab thickness that can be set.
    double getMaximumSlabThickness() const;

    /// Casts the given QViewer to a Q2DViewer object
    /// If casting is successful, casted pointer to Q2DViewer will be returned, null otherwise
    static Q2DViewer* castFromQViewer(QViewer *viewer);

    /// Sets the opacity of the image actor of the volume at the given index.
    void setVolumeOpacity(int index, double opacity);

    /// Returns the transfer function of the volume at the given index.
    const TransferFunction& getVolumeTransferFunction(int index) const;
    /// Sets the transfer function of the volume at the given index.
    void setVolumeTransferFunction(int index, const TransferFunction &transferFunction);
    /// Clears the transfer function of the volume at the given index.
    void clearVolumeTransferFunction(int index);

    /// Returns the prop that represents the image in the scene.
    vtkImageSlice* getImageProp() const;

    /// Returns true if this Q2DViewer can show a display shutter in its current state, i.e. if there is a display shutter for the current image and there isn't
    /// any restriction to show display shutters.
    bool canShowDisplayShutter() const;

    /// Donada una coordenada de món, l'ajustem perquè caigui dins dels límits de l'imatge actual
    /// Això ens serveix per tools que agafen qualsevol punt de món, però necessiten que aquesta estigui
    /// dins dels límits de la imatge, com pot ser una ROI. Aquest mètode acaba d'ajustar la coordenada perquè
    /// estigui dins dels límits de la pròpia imatge
    /// @param xyz[] Coordenada que volem ajustar. Serà un paràmetre d'entrada/sortida i el seu contingut
    /// es modificarà perquè caigui dins dels límits de la imatge
    void putCoordinateInCurrentImageBounds(double xyz[3]);

    /// donat un punt 3D en espai de referència DICOM, ens dóna la projecció d'aquest punt sobre
    /// el pla actual, transformat a coordenades de món VTK
    /// @param pointToProject[]
    /// @param projectedPoint[]
    /// @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
    /// per casos en que el pla "real" no és el que volem i necessitem una petita modificació
    void projectDICOMPointToCurrentDisplayedImage(const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack = false);

    bool getDicomWorldCoordinates(const double xyz[3], double dicomWorldPosition[4]);

    /// Assigna/Retorna el volum solapat
    void setOverlayInput(Volume *volume);
    Volume* getOverlayInput();

    /// Indiquem que cal actualitzar l'Overlay actual
    void updateOverlay();

    /// Assignem l'opacitat del volum solapat.
    /// Els valors podran anar de 0.0 a 1.0, on 0.0 és transparent i 1.0 és completament opac.
    void setOverlayOpacity(double opacity);

    /// Returns the current transfer function of the main volume.
    const TransferFunction& getTransferFunction() const;

    /// Returns the transfer function model corresponding to the input(s) of this Q2DViewer.
    TransferFunctionModel* getTransferFunctionModel() const;

    /// Returns a unique string identifier of all inputs
    virtual QString getInputIdentifier() const;

    /// Returns true if this viewer contains the given volume.
    bool containsVolume(const Volume *volume) const;

    /// Returns the index of the given volume in this viewer. If this viewer doesn't contain the given volume, returns -1.
    int indexOfVolume(const Volume *volume) const;

    /// Returns VOI LUT data corresponding to the volume at the given index.
    VoiLutPresetsToolData* getVoiLutDataForVolume(int index) const;

    /// Returns the fusion balance as a value in the range [0, 100] representing the weight of the second input.
    int getFusionBalance() const;

    /// Moves the camera based on the absolute motion vector
    void absolutePan(double motionVector[3]);

public slots:
    virtual void setInput(Volume *volume);

    /// Especifica el volum d'entrada de forma asíncrona.
    /// Es pot indicar un command que s'executarà un cop el volum s'ha carregat i està a punt de ser visualitzat.
    /// Útil per poder especificar canvis al viewer (canvi de llesca, w/l, etc.) sense preocupar-se de quan s'ha carregat el volume.
    void setInputAsynchronously(Volume *volume, QViewerCommand *inputFinishedCommand = 0);
    void setInputAsynchronously(const QList<Volume*> &volumes, QViewerCommand *inputFinishedCommand = 0);

    void resetView(const OrthogonalPlane &view);

    void resetView(const AnatomicalPlane &anatomicalPlane);

    /// Restaura el visualitzador a l'estat inicial
    void restore();

    /// Esborra totes les primitives del visor
    void clearViewer();

    /// Canvia el WW del visualitzador, per tal de canviar els blancs per negres, i el negres per blancs
    void invertVoiLut();

    /// Canvia la llesca que veiem de la vista actual
    void setSlice(int value);

    /// Sets the given phase index to the main volume.
    void setPhase(int value);
    /// Sets the given phase index to the volume at the given index. If there isn't a volume at the given index, it does nothing.
    void setPhaseInVolume(int index, int phase);

    /// Indica el tipu de solapament dels volums, per defecte blending
    void setOverlapMethod(OverlapMethod method);

    /// Afegir o treure la visibilitat d'una anotació textual/gràfica
    void enableAnnotation(AnnotationFlags annotation, bool enable = true);
    void removeAnnotation(AnnotationFlags annotation);

    /// Sets the VOI LUT for this viewer.
    virtual void setVoiLut(const VoiLut &voiLut);
    /// Sets the given VOI LUT to the volume at the given index. If there isn't a volume at the given index, it does nothing.
    void setVoiLutInVolume(int index, const VoiLut &voiLut);

    /// Sets the transfer function of the main volume.
    void setTransferFunction(const TransferFunction &transferFunction);
    /// Clears the transfer function of the main volume.
    void clearTransferFunction();

    /// L'únic que fa és emetre el senyal seedPositionChanged, per poder-ho cridar desde la seedTool
    /// TODO Aquest mètode hauria de quedar obsolet
    void setSeedPosition(double pos[3]);

    /// Aplica una rotació de 90 graus en el sentit de les agulles del rellotge
    /// tantes "times" com li indiquem, per defecte sempre serà 1 "time"
    void rotateClockWise(int times = 1);

    /// Aplica una rotació de 90 graus en el sentit contrari a les agulles del rellotge
    /// tantes "times" com li indiquem, per defecte sempre serà 1 "time"
    void rotateCounterClockWise(int times = 1);

    /// Aplica un flip horitzontal/vertical sobre la imatge. El flip vertical es farà com una rotació de 180º seguida d'un flip horitzontal
    void horizontalFlip();
    void verticalFlip();

    // TODO aquests mètodes també haurien d'estar en versió QString!

    /// Sets the given slab projection mode to the main volume.
    void setSlabProjectionMode(VolumeDisplayUnit::SlabProjectionMode projectionMode);
    /// Sets the given slab projection mode to the volume at the given index. If there isn't a volume at the given index, it does nothing.
    void setSlabProjectionModeInVolume(int index, VolumeDisplayUnit::SlabProjectionMode slabProjectionMode);

    /// Sets the given slab thickness in mm to the main volume.
    void setSlabThickness(double thickness);
    /// Sets the given slab thickness in mm to the volume at the given index. If there isn't a volume at the given index, it does nothing.
    void setSlabThicknessInVolume(int index, double thickness);

    /// Disables thick slab. Acts as a shortcut for setSlabThickness(0.0).
    void disableThickSlab();

    /// Alineament de la imatge dins del visualitzador
    void alignLeft();
    void alignRight();

    /// Posa la posició d'alineament de la imatge (dreta, esquerre, centrat)
    void setAlignPosition(AlignPosition alignPosition);

    /// Aplica les transformacions 2D necessàries sobre la imatge actual perquè aquesta tingui la orientació indicada
    /// La orientació indicada ha de ser possible de conseguir mitjançant operacions de rotació i flip. En cas que no
    /// existeixin combinacions possibles, no es canviarà la orientació de la imatge
    void setImageOrientation(const PatientOrientation &desiredPatientOrientation);

    /// Fa que els ImageOverlays siguin visibles o no
    void showImageOverlays(bool enable);

    /// Fa que els shutters siguin visibles o no
    void showDisplayShutters(bool enable);

    /// Sets the fusion balance as a value in the range [0, 100] representing the weight of the second input.
    void setFusionBalance(int balance);

signals:
    /// Envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// Envia la nova fase en la que ens trobem
    void phaseChanged(int);

    /// Envia la nova vista en la que ens trobem
    void viewChanged(int);

    /// Emitted when a new patient orientation has been set
    void imageOrientationChanged(const PatientOrientation &orientation);
    
    /// Senyal que s'envia quan la llavor s'ha canviat
    /// TODO Mirar de treure-ho i posar-ho en la tool SeedTool
    void seedPositionChanged(double x, double y, double z);

    /// Emitted when the slab projection mode has changed.
    void slabProjectionModeChanged(VolumeDisplayUnit::SlabProjectionMode slabProjectionMode);

    /// S'emet quan canvia l'slab thickness
    /// @param thickness Nou valor de thickness
    void slabThicknessChanged(double thickness);

    /// Senyal que s'envia quan ha canviat l'overlay
    void overlayChanged();
    void overlayModified();

    /// Signal emitted when the viewer is restored
    void restored();

    /// Signal emitted when a set of new volumes (not dummies) have been rendered
    void newVolumesRendered();

protected:
    /// Processem l'event de resize de la finestra Qt
    virtual void resizeEvent(QResizeEvent *resize);

    void getCurrentRenderedItemBounds(double bounds[6]);

    void setDefaultOrientation(const AnatomicalPlane &anatomicalPlane);

    /// Returns the current view plane.
    virtual OrthogonalPlane getCurrentViewPlane() const;

    /// Sets the current view plane.
    virtual void setCurrentViewPlane(const OrthogonalPlane &viewPlane);

private:
    /// Updates image orientation according to the preferred presentation depending on its attributes, like modality.
    /// At this moment it is only applying to mammography (MG) images
    void updatePreferredImageOrientation();

    /// Initializes camera parameters
    void initializeCamera();

    /// Initialized the dummy display unit with the proper parameters
    void initializeDummyDisplayUnit();

    /// Adds/remove the image actors to/from the scene
    void addImageActors();
    void removeImageActors();
    
    /// Updates the displayed images in the image slices.
    void updateImageSlices();

    /// Print some information related to the volume
    void printVolumeInformation();

    /// Actualitza el pipeline del filtre de shutter segons si està habilitat o no
    void updateShutterPipeline();

    /// Updates the mask used as display shutter if display shutters should and can be shown.
    void updateDisplayShutterMask();

    /// Re-inicia els paràmetres de la càmera segons la vista actual.
    void resetCamera();

    /// Aplica el factor de rotació adient segons els girs que li indiquem. No actualitza la càmera ni l'escena, simplement
    /// es fa servir per posar els valors correctes a les variables internes que controlen com està girada la imatge.
    void rotate(int times);

    /// Sets if image should be flipped (horizontally) or not. It does not update the camera nor renders the scene.
    void setFlip(bool flip);
    
    /// Updates the camera, renders and emits the current image orientataion
    void applyImageOrientationChanges();
    
    /// Carrega un volum asíncronament
    void loadVolumeAsynchronously(Volume *volume);
    void loadVolumesAsynchronously(const QList<Volume *> &volumes);

    /// Retorna un volum "dummy"
    Volume* getDummyVolumeFromVolume(Volume *volume);

    /// Especifica quin command s'ha d'executar després d'especificar un volum com a input
    void setInputFinishedCommand(QViewerCommand *command);

    /// Elimina el command que s'hauria d'executar després de que s'especifiqui un input
    void deleteInputFinishedCommand();

    /// Si està definit, executa el command definit per després d'especificar un input al viewer
    void executeInputFinishedCommand();

    /// Updates the current image default presets values. It only applies to original acquisition plane.
    void updateCurrentImageDefaultPresetsInAllInputsOnOriginalAcquisitionPlane();

    /// Calls setNewVolumes and excutes the command while catching any exception that may be thrown.
    void setNewVolumesAndExecuteCommand(const QList<Volume*> &volumes);

    /// Elimina els bitmaps que s'hagin creat per aquest viewer
    void removeViewerBitmaps();
    
    /// Carrega en memòria els ImageOverlays del volum passat per paràmetre (sempre que no sigui un dummy) i els afegeix al Drawer
    void loadOverlays(Volume *volume);

    /// Enum to define the different dimensions an image slice could be associated to
    enum SliceDimension { SpatialDimension, TemporalDimension };
    /// Updates the image slice to be displayed on the specified dimension
    void updateSliceToDisplay(int value, SliceDimension dimension);

    /// Updates the slice to display in the secondary volumes to the closest one in the main volume.
    void updateSecondaryVolumesSlices();

    /// Returns the VolumeDisplayUnit of the given index. Returns null if there's no display unit or index is out of range
    VolumeDisplayUnit* getDisplayUnit(int index) const;
    VolumeDisplayUnit* getMainDisplayUnit() const;

    /// Returns all the current display units. The list will be empty if we have no input.
    QList<VolumeDisplayUnit*> getDisplayUnits() const;

private slots:
    /// Actualitza les transformacions de càmera (de moment rotació i flip)
    void updateCamera();

    /// Reimplementem per tal de que faci un setInputAsynchronously
    /// TODO: De moment es fa així de manera xapussa fins que no es traspassin els mètode de càrrega
    /// asíncrona a QViewer.
    virtual void setInputAndRender(Volume *volume);
    void setInputAndRender(const QList<Volume*> &volumes);

    /// Replaces the volumes displayed by this viewer by the new ones and resets the viewer.
    /// If the second parameter is false, the volumes won't be rendered.
    void setNewVolumes(const QList<Volume*> &volumes, bool setViewerStatusToVisualizingVolume = true);

    void volumeReaderJobFinished();

protected:
    /// Aquest és el segon volum afegit a solapar
    Volume *m_overlayVolume;

    /// Aquest és el blender per veure imatges fusionades
    BlendFilter* m_blender;

    /// Opacitat del volum solapat
    double m_overlayOpacity;

private:
    /// Nom del grups dins del drawer per als Overlays
    static const QString OverlaysDrawerGroup;

    /// Constant per a definir el nom d'objecte dels volums "dummy"
    static const QString DummyVolumeObjectName;

    /// Tipus de solapament dels volums en cas que en tinguem més d'un
    OverlapMethod m_overlapMethod;

    /// Factor de rotació. En sentit de les agulles del rellotge 0: 0º, 1: 90º, 2: 180º, 3: 270º.
    int m_rotateFactor;

    /// Indica si cal aplicar un flip horitzontal o no sobre la càmera
    bool m_applyFlip;

    /// Aquesta variable controla si la imatge està flipada respecte la seva orientació original. Útil per controlar annotacions.
    bool m_isImageFlipped;

    /// Especialista en dibuixar primitives
    Drawer *m_drawer;

    /// Indica quin tipus de projecció apliquem sobre l'slab
    VolumeDisplayUnit::SlabProjectionMode m_slabProjectionMode;

    /// Conté el mapeig d'operacions a fer quan voelm passar d'una orientació a un altre
    ImageOrientationOperationsMapper *m_imageOrientationOperationsMapper;

    /// Posició a on s'ha d'alinear la imatge (dreta, esquerre o centrat)
    AlignPosition m_alignPosition;

    /// Manager of the reading of volumes
    VolumeReaderManager *m_volumeReaderManager;

    QViewerCommand *m_inputFinishedCommand;

    /// Llistat d'overlays
    QList<DrawerBitmap*> m_viewerBitmaps;

    /// Controla si els overlays estan habilitats o no
    bool m_overlaysAreEnabled;
    
    /// If true, display shutters are visible when they are available and it's possible to show them.
    bool m_showDisplayShutters;

    /// Factory to create the proper display units handler
    VolumeDisplayUnitHandlerFactory *m_displayUnitsFactory;

    /// Unit to be used when no input is present or an out of index unit have been requested. This way we can always return safe default values.
    VolumeDisplayUnit *m_dummyDisplayUnit;
    
    /// VolumeDisplayUnits handler. Handles all the display units of the viewer.
    QSharedPointer<GenericVolumeDisplayUnitHandler> m_displayUnitsHandler;

    /// Handles the textual annotations of the viewer
    Q2DViewerAnnotationHandler *m_annotationsHandler;

    /// Fusion balance stored as a value in the range [0, 100] representing the weight of the second input.
    int m_fusionBalance;

};

};  //  End namespace udg

#endif
