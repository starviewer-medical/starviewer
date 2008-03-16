/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERPRESENTATIONSTATEATTACHER_H
#define UDGQ2DVIEWERPRESENTATIONSTATEATTACHER_H

#include <QObject>
#include <QColor>

// Per la llista de datasets per fer els attachments necessaris segons la fase del pipeline
#include "dcmtk/dcmdata/dcfilefo.h"

//dcmtk fwd decls
class DVPresentationState;
class DcmDataset;
class vtkWindowLevelLookupTable;
class vtkImageShiftScale;

namespace udg {

/**
Adjunta un presentation state a un Q2DViewer i és capaç d'habilitar-lo i deshabilitar-lo sobre aquest

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Q2DViewer;
class Q2DViewerBlackBoard;
class Volume;

class Q2DViewerPresentationStateAttacher : public QObject
{
Q_OBJECT
public:
    Q2DViewerPresentationStateAttacher( Q2DViewer *viewer, const QString &presentationStateFile, QObject *parent = 0 );

    ~Q2DViewerPresentationStateAttacher();

    /// Li dóna un nou arxiu de presentation state i fa l'attachment. Si existeix un attachment anterior fa un detach previament
    void setPresentationStateFileAndAttach( const QString &presentationStateFile );

    /// Adjunta/desadjunta el presentation state aplicant-lo/desaplicant-lo sobre el 2DViewer
    bool attach();
    bool detach();

private:
    /// El viewer sobre el qual apliquem el presentation state
    Q2DViewer *m_2DViewer;

    /// nom de l'arxiu del presentation state
    QString m_presentationStateFilename;

    /// El gestor del presentation state
    DVPresentationState *m_presentationStateHandler;

    /// Gestor de baix nivell per accedir a les dades del presentation state
    DcmDataset *m_presentationStateData;

    /// Volum que té el 2DViewer
    Volume *m_viewerInput;

    /// Per gestionar les anotacions
    Q2DViewerBlackBoard *m_board;

    /// coordenades DE MÓN de l'area de display que ens dóna el presentation state
    double m_displayedAreaWorldTopLeft[2], m_displayedAreaWorldBottomRight[2];

    /// llista de datasets(imatges) que formen el volum que adjuntem al presentation state
    QList<DcmFileFormat *> m_attachedDatasetsList;
    /*
        GRAYSCALE PIPELINE
        Només apliquem les del propi presentation state!
    */

    /// aplica la pipeline de les transformacions grayscale.
    void applyGrayscaleTransformPipeline();

    /**
     * Aplica la transformació de modalitat sobre la imatge. Ens podem trobar amb que tenim un presentation state associat o no.
     1.- En el cas que no hi hagi un presentation state associat ens podem trobar amb les següents situacions:

        1.1. La imatge original té una modality LUT. La llegim i l'apliquem al principi del pipeline. El seu input són les dades originals del volum.

        1.2. La imatge original conté rescale slope i rescale intercept. No cal fer res, el lector d'itk (GDCM) ja aplica aquesta transformació automàticament quan llegim el volum.

        1.3. No hi ha cap informació referent a modality LUTs. No cal fer res. Aplicar transformació identitat

     2.- En en cas que tinguem un presentation state associat, pot passar el següent

        2.1. El PS té una modality LUT. ídem 1.1

        2.2. El PS conté rescale slope i rescale intercept. Cal aplicar un filtre de rescale (vtImageShiftScale). \TODO Com que la imatge que llegim ja té aplicat el rescale/intercept de la imatge, no sabem si cal contrestar primer la transformació que ens vé de "gratis".

        2.3. ídem 1.3 \TODO però no sabem si en el cas que tinguem un rescalat de "gratis" caldria desfer-lo, és a dir, donar els raw pixels!

    Els canvis de la modality LUT s'apliquen a totes les imatges contingudes en el volum
     */
    void applyModalityLUT();
    ///
    void applyMaskSubstraction();

    /**
     *  Aplica l'ajustament de finestra sobre la imatge. Ens podem trobar amb que tenim un presentation state associat o no.

     1.- En el cas que no hi hagi un presentation state associat ens podem trobar amb les següents situacions:

        1.1. La imatge original té una VOI LUT. La llegim i l'apliquem com a input del window level mapper. \TODO problema: no sabem ben bé què passa si abans teníem una modality LUT. En principi hauríem de fer servir el mapeig sobre l'anterior lut.

        1.2. La imatge original conté valors de window level. Apliquem aquests valors sobre el window level mapper.

        1.3. No hi ha cap informació referent a VOI LUTs. No cal fer res. Aplicar transformació identitat, és a dir el window serà el rang de dades i el level el window/2.

     2.- En en cas que tinguem un presentation state associat, actuem igual que en 1.x. Els valors del presentation state prevalen sobre els de la imatge.

    Es pot tenir més d'una VOI LUT (ja sigui en format de LUT o de window level). Això significa que tenim diverses opcions de presentació. Es pot agafar una per defecte però l'aplicació hauria de mostrar la possiblitat d'escollir entre aquestes.

    Els canvis de la VOI LUT es poden aplicar a sub-conjunts d'imatges referenciades. Això es donarà en el cas d'imatges multi-frame.

    De cares al connectathon només es tracta una sola VOI LUT i imatges mono-frame, però hem de tenir en compte que l'estàndar DICOM contempla les possibilitats abans mencionades.
     */
    void applyVOILUT();
    ///
    void applyPresentationLUT();

    /// ajustament ww/wl
    double m_window, m_level;

    vtkWindowLevelLookupTable *parseLookupTable( int type );

    /// objectes per a les transformacions en el pipeline d'escala de grisos
    vtkImageShiftScale *m_modalityLUTRescale;

    /// Les diferents look up tables que ens podem trobar durant tot el procés.
    vtkWindowLevelLookupTable *m_modalityLut, *m_windowLevelLut, *m_presentationLut;

    double m_modalityRange[2];

    //
    // Transformacions espacials i annotacions
    //

    /// Retalla una part de la imatge
    void applyShutterTransformation();

    /// Ens dóna el color recomenat de les anotacions d'un layer
    QColor getRecommendedColor( int layer );

    /// aplica les annotacions gràfiques i textuals DEFINIDES EN L'ESPAI DE PIXEL a aplicar ABANS de les transformacions espacials
    void applyPreSpatialTransformAnnotation();

    /// específic de les transformacions espacials
    void applySpatialTransformation();

    /// processa els objectes de text del presentation state d'un layer assignats a una llesca
    void processTextObjects( int layer, int slice );

    /// aplica les annotacions gràfiques i textuals RELATIVES A LA DISPLAYED AREA a aplicar DESPRÉS de les transformacions espacials
    void applyPostSpatialTransformAnnotation();

    /// específic de l'area de display
    void applyDisplayedAreaTransformation();

    /// TODO els overlays encara no s'apliquen, només es llegeixen
    void applyOverlayPlanes();

    /// \TODO només llegeix els objectes existents, encara no renderitza res
    void renderCurveObjects();
};

}

#endif
