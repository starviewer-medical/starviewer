/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdifuperfuextension.h"
#include "strokesegmentationmethod.h"
#include "volumecalculator.h"
#include "series.h"
#include "logging.h"
#include "toolmanager.h"
#include "toolproxy.h"
#include "editortool.h"
#include "editortooldata.h"
#include "diffusionperfusionsegmentationsettings.h"
#include "patientbrowsermenu.h"
#include "transferfunction.h"
#include "voilut.h"
// Qt
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QActionGroup>
// VTK
#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkImageCast.h>
#include <vtkImageThreshold.h>
#include <vtkLookupTable.h>
#include <vtkRenderer.h>
#include <vtkMetaImageWriter.h>
//itk
#include <itkRescaleIntensityImageFilter.h>
//itk-udg
#include "udgPerfusionEstimator.h"
#include "udgBinaryMaker.h"

namespace udg {

const double QDifuPerfuSegmentationExtension::RegistrationFixedStandardDeviation = 0.4;
const double QDifuPerfuSegmentationExtension::RegistrationMovingStandardDeviation = 0.4;
const int QDifuPerfuSegmentationExtension::RegistrationNumberOfSpacialSamples = 200;
const int QDifuPerfuSegmentationExtension::RegistrationFixedVariance = 2;
const int QDifuPerfuSegmentationExtension::RegistrationMovingVariance = 2;
const int QDifuPerfuSegmentationExtension::RegistrationMaximumStep = 1;
const double QDifuPerfuSegmentationExtension::RegistrationMinimumStep = 0.001;
const int QDifuPerfuSegmentationExtension::RegistrationNumberOfIterations = 300;

QDifuPerfuSegmentationExtension::QDifuPerfuSegmentationExtension(QWidget * parent)
 : QWidget(parent), m_diffusionInputVolume(0), m_perfusionInputVolume(0), m_diffusionMainVolume(0), m_perfusionMainVolume(0), m_diffusionRescaledVolume(0), m_perfusionRescaledVolume(0), m_activedMaskVolume(0), m_strokeMaskVolume(0), m_ventriclesMaskVolume(0), m_blackpointEstimatedVolume(0), m_penombraMaskVolume(0), m_penombraMaskMinValue(0), m_penombraMaskMaxValue(254), m_perfusionOverlay(0), m_strokeSegmentationMethod(0), m_strokeVolume(0.0), m_registerTransform(nullptr), m_penombraVolume(0.0)
{
    setupUi(this);
    DiffusionPerfusionSegmentationSettings().init();

    m_perfusionHueLut = vtkLookupTable::New();

    createActions();
    createConnections();
    readSettings();

    // creem el tool manager i li assignem les tools
    initializeTools();
}

QDifuPerfuSegmentationExtension::~QDifuPerfuSegmentationExtension()
{
    writeSettings();

    delete m_diffusionRescaledVolume;
    delete m_perfusionRescaledVolume;

    delete m_strokeMaskVolume;
    delete m_ventriclesMaskVolume;

    delete m_blackpointEstimatedVolume;
    delete m_penombraMaskVolume;

    if (m_perfusionOverlay) m_perfusionOverlay->Delete();

    delete m_strokeSegmentationMethod;

    m_perfusionHueLut->Delete();
}

void QDifuPerfuSegmentationExtension::initializeTools()
{
    // creem el tool manager
    m_toolManager = new ToolManager(this);
    // obtenim les accions de cada tool que volem
    m_zoomToolButton->setDefaultAction(m_toolManager->registerTool("ZoomTool"));
    m_slicingToolButton->setDefaultAction(m_toolManager->registerTool("SlicingMouseTool"));
    m_moveToolButton->setDefaultAction(m_toolManager->registerTool("TranslateTool"));
    m_windowLevelToolButton->setDefaultAction(m_toolManager->registerTool("WindowLevelTool"));
    m_seedToolButton->setDefaultAction(m_toolManager->registerTool("SeedTool"));
    m_voxelInformationToolButton->setDefaultAction(m_toolManager->registerTool("VoxelInformationTool"));
    m_editorToolButton->setDefaultAction(m_toolManager->registerTool("EditorTool"));
    m_toolManager->registerTool("VoiLutPresetsTool");
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton
    QStringList defaultTools;
    defaultTools << "VoiLutPresetsTool" << "SlicingKeyboardTool" << "SlicingMouseTool" << "TranslateTool" << "WindowLevelTool";
    m_toolManager->triggerTools(defaultTools);

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingMouseTool" << "SeedTool" << "EditorTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // inicialitzem totes les tools
    m_toolManager->setupRegisteredTools(m_diffusion2DView);
    m_toolManager->setupRegisteredTools(m_perfusion2DView);

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction(m_toolManager->registerActionTool("RotateClockWiseActionTool"));
    m_toolManager->enableRegisteredActionTools(m_diffusion2DView);
    m_toolManager->enableRegisteredActionTools(m_perfusion2DView);
}

void QDifuPerfuSegmentationExtension::createActions()
{
    // TODO el nom tant dels botons com de les icones, s'ahurien de millorar
    m_splitterLeftButton->setIcon(QIcon(":/images/icons/media-playback-backwards.svg"));
    m_splitterCenterButton->setIcon(QIcon(":/images/icons/view-split-left-right.svg"));
    m_splitterRightButton->setIcon(QIcon(":/images/icons/media-playback-start.svg"));

    m_lesionViewAction = new QAction(this);
    m_lesionViewAction->setText(tr("Lesion Overlay"));
    m_lesionViewAction->setStatusTip(tr("Enable/disable lesion view overlay"));
    m_lesionViewAction->setCheckable(true);
    m_lesionViewAction->setEnabled(false);
    m_lesionViewToolButton->setDefaultAction(m_lesionViewAction);

    m_ventriclesViewAction = new QAction(this);
    m_ventriclesViewAction->setText(tr("Ventricles Overlay"));
    m_ventriclesViewAction->setStatusTip(tr("Enable/disable ventricles view overlay"));
    m_ventriclesViewAction->setCheckable(true);
    m_ventriclesViewAction->setEnabled(false);
    m_ventriclesViewToolButton->setDefaultAction(m_ventriclesViewAction);

    m_viewOverlayActionGroup = new QActionGroup(this);
    m_viewOverlayActionGroup->setExclusive(true);
    m_viewOverlayActionGroup->addAction(m_ventriclesViewAction);
    m_viewOverlayActionGroup->addAction(m_lesionViewAction);

    //Disable buttons
    m_saveRegisteredPerfusionPushButton->setEnabled(false);
}

void QDifuPerfuSegmentationExtension::createConnections()
{
    connect(m_diffusion2DView, SIGNAL(phaseChanged(int)), m_selectedDiffusionImageSpinBox, SLOT(setValue(int)));
    connect(m_selectedDiffusionImageSpinBox, SIGNAL(valueChanged(int)), SLOT(setDiffusionImage(int)));

    connect(m_perfusion2DView, SIGNAL(phaseChanged(int)), m_selectedPerfusionImageSpinBox, SLOT(setValue(int)));
    connect(m_selectedPerfusionImageSpinBox, SIGNAL(valueChanged(int)), SLOT(setPerfusionImage(int)));
    connect(m_perfusionThresholdViewerSlider, SIGNAL(valueChanged(int)), SLOT(setPerfusionLut(int)));

    connect(m_strokeLowerValueSlider, SIGNAL(valueChanged(int)), SLOT(setStrokeLowerValue(int)));
    connect(m_strokeUpperValueSlider, SIGNAL(valueChanged(int)), SLOT(setStrokeUpperValue(int)));
/*    connect(m_strokeViewThresholdsPushButton, SIGNAL(clicked()), SLOT(viewThresholds()));
    connect(m_penombraViewThresholdsPushButton, SIGNAL(clicked()), SLOT(viewThresholds2()));*/
    connect(m_diffusion2DView, SIGNAL(seedPositionChanged(double,double,double)), SLOT(setSeedPosition(double,double,double)));
    connect(m_strokeApplyPushButton, SIGNAL(clicked()), SLOT(applyStrokeSegmentation()));
//     connect(m_strokeVolumeUpdatePushButton, SIGNAL(clicked()), SLOT(updateStrokeVolume()));

    connect(m_ventriclesApplyPushButton, SIGNAL(clicked()), SLOT(applyVentriclesMethod()));
    //connect(m_ventriclesLowerValueSlider, SIGNAL(valueChanged(int)), SLOT(applyVentriclesMethod(int)));

    connect(m_applyRegistrationPushButton, SIGNAL(clicked()), SLOT(applyRegistration()));

    connect(m_computeBlackpointEstimationPushButton, SIGNAL(clicked()), SLOT(computeBlackpointEstimation()));

    connect(m_penombraApplyPushButton, SIGNAL(clicked()), SLOT(applyPenombraSegmentation()));

    connect(m_filterDiffusionPushButton, SIGNAL(clicked()), SLOT(applyFilterDiffusionImage()));

    //connect(m_diffusion2DView , SIGNAL(eventReceived(unsigned long)), SLOT(strokeEventHandler(unsigned long)));
    //connect(m_perfusion2DView , SIGNAL(eventReceived(unsigned long)), SLOT(strokeEventHandler2 (unsigned long)));
    // caldria pel perfusion?????
    connect(m_synchroCheckBox, SIGNAL(toggled(bool)), SLOT(synchronizeSlices(bool)));

    // potser és millor fer-ho amb l'acció (signal triggered())
    connect(m_lesionViewToolButton , SIGNAL(clicked()), SLOT(viewLesionOverlay()));

    // potser és millor fer-ho amb l'acció (signal triggered())
    connect(m_ventriclesViewToolButton , SIGNAL(clicked()), SLOT(viewVentriclesOverlay()));

    /*connect(m_eraseButton , SIGNAL(clicked()), SLOT(setErase()));

    connect(m_eraseSliceButton , SIGNAL(clicked()), SLOT(setEraseSlice()));

    connect(m_paintButton , SIGNAL(clicked()), SLOT(setPaint()));

    connect(m_eraseRegionButton , SIGNAL(clicked()), SLOT(setEraseRegion()));
*/
    connect(m_splitterLeftButton, SIGNAL(clicked()), SLOT(moveViewerSplitterToLeft()));
    connect(m_splitterCenterButton, SIGNAL(clicked()), SLOT(moveViewerSplitterToCenter()));
    connect(m_splitterRightButton, SIGNAL(clicked()), SLOT(moveViewerSplitterToRight()));

    connect(m_diffusionSliceSlider, SIGNAL(valueChanged(int)) , m_diffusion2DView , SLOT(setSlice(int)));
    connect(m_perfusionSliceSlider, SIGNAL(valueChanged(int)) , m_perfusion2DView , SLOT(setSlice(int)));

    connect(m_diffusion2DView, SIGNAL(sliceChanged(int)), m_diffusionSliceSlider, SLOT(setValue(int)));
    connect(m_perfusion2DView, SIGNAL(sliceChanged(int)), m_perfusionSliceSlider, SLOT(setValue(int)));

    connect(m_perfusion2DView, SIGNAL(sliceChanged(int)), SLOT(setPerfusionSlice(int)));

    connect(m_diffusionOpacitySlider, SIGNAL(valueChanged(int)), SLOT(setDiffusionOpacity(int)));
    connect(m_perfusionOpacitySlider, SIGNAL(valueChanged(int)), SLOT(setPerfusionOpacity(int)));

    // Fem que no s'assigni automàticament l'input que s'ha seleccionat amb el menú de pacient, ja que fem tractaments adicionals
    // sobre el volum seleccionat i l'input final del visor pot diferir de l'inicial i és l'extensió qui decideix finalment quin input
    // se li vol donar a cada viewer. Capturem la senyal de quin volum s'ha escollit i a partir d'aquí fem el que calgui
    m_diffusion2DView->setAutomaticallyLoadPatientBrowserMenuSelectedInput(false);
    m_perfusion2DView->setAutomaticallyLoadPatientBrowserMenuSelectedInput(false);
    connect(m_diffusion2DView->getPatientBrowserMenu(), SIGNAL(selectedVolume(Volume*)), SLOT(setDiffusionInput(Volume*)));
    connect(m_perfusion2DView->getPatientBrowserMenu(), SIGNAL(selectedVolume(Volume*)), SLOT(setPerfusionInput(Volume*)));
    
    connect(m_penombraVolumeLineEdit, SIGNAL(textChanged(const QString&)), SLOT(computePenombraVolume()));

    connect(m_saveDiffusionVolumePushButton, SIGNAL(clicked()), SLOT(saveDiffusionVolume()));
    connect(m_saveDiffusionMaskPushButton, SIGNAL(clicked()), SLOT(saveDiffusionMask()));
    connect(m_savePerfusionVolumePushButton, SIGNAL(clicked()), SLOT(savePerfusionVolume()));
    connect(m_savePerfusionMaskPushButton, SIGNAL(clicked()), SLOT(savePerfusionMask()));
    connect(m_saveRegisteredPerfusionPushButton, SIGNAL(clicked()), SLOT(saveRegisteredPerfusionVolume()));
    connect(m_saveTransformPushButton, SIGNAL(clicked()), SLOT(saveTransform()));

    connect(m_diffusion2DView, SIGNAL(overlayModified()), SLOT(updateStrokeVolume()));
    connect(m_perfusion2DView, SIGNAL(overlayModified()), SLOT(updatePenombraVolume()));

}

void QDifuPerfuSegmentationExtension::readSettings()
{
    Settings settings;

    settings.restoreGeometry(DiffusionPerfusionSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    settings.restoreGeometry(DiffusionPerfusionSegmentationSettings::VerticalSplitterGeometry, m_viewerSplitter);
    m_savingMaskDirectory = settings.getValue(DiffusionPerfusionSegmentationSettings::SavedMaskPath).toString();
}

void QDifuPerfuSegmentationExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry(DiffusionPerfusionSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    ///Movem l'splitter a la dreta pq quan es torni obrir només es vegi la difu
    //this->moveViewerSplitterToRight();
    settings.saveGeometry(DiffusionPerfusionSegmentationSettings::VerticalSplitterGeometry, m_viewerSplitter);
    settings.setValue(DiffusionPerfusionSegmentationSettings::SavedMaskPath, m_savingMaskDirectory);
}

void QDifuPerfuSegmentationExtension::setDiffusionInput(Volume * input)
{
    if (!input)
    {
        ERROR_LOG("setDiffusionInput: null input");
        return;
    }

    m_diffusionInputVolume = input;

    m_diffusionSliceSlider->setMinimum(0);
    m_diffusionSliceSlider->setMaximum(m_diffusionInputVolume->getNumberOfSlicesPerPhase() -1);
    m_diffusionSliceSpinBox->setMinimum(0);
    m_diffusionSliceSpinBox->setMaximum(m_diffusionInputVolume->getNumberOfSlicesPerPhase() - 1);


    disconnect(m_selectedDiffusionImageSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setDiffusionImage(int)));
    m_selectedDiffusionImageSpinBox->setMaximum(m_diffusionInputVolume->getNumberOfPhases() -1);
    m_selectedDiffusionImageSpinBox->setValue(m_diffusionInputVolume->getNumberOfPhases() -1);

    m_diffusionSliceSlider->setValue(m_diffusion2DView->getCurrentSlice());
    setDiffusionImage(m_diffusionInputVolume->getNumberOfPhases() - 1);
    connect(m_selectedDiffusionImageSpinBox, SIGNAL(valueChanged(int)), SLOT(setDiffusionImage(int)));
}

void QDifuPerfuSegmentationExtension::setMaxDiffusionImage(int max)
{
    m_selectedDiffusionImageSpinBox->setMaximum(max - 1);
}

void QDifuPerfuSegmentationExtension::setDiffusionImage(int index)
{
    m_diffusionMainVolume = m_diffusionInputVolume->getPhaseVolume(index);

    double range[2];
    m_diffusionMainVolume->getScalarRange(range);
    m_diffusionMinValue = (ItkImageType::PixelType)range[0];
    m_diffusionMaxValue = (ItkImageType::PixelType)range[1];

    DEBUG_LOG(QString("diffusionMax=%1, diffusionMin=%2").arg(m_diffusionMaxValue).arg(m_diffusionMinValue));

    disconnect(m_strokeLowerValueSlider, SIGNAL(valueChanged(int)), this, SLOT(viewThresholds()));
    disconnect(m_strokeUpperValueSlider, SIGNAL(valueChanged(int)), this, SLOT(viewThresholds()));

    m_strokeLowerValueSpinBox->setMinimum(m_diffusionMinValue);
    m_strokeLowerValueSpinBox->setMaximum(m_diffusionMaxValue);
    m_strokeUpperValueSpinBox->setMinimum(m_diffusionMinValue);
    m_strokeUpperValueSpinBox->setMaximum(m_diffusionMaxValue);
    m_strokeLowerValueSlider->setMinimum(m_diffusionMinValue);
    m_strokeLowerValueSlider->setMaximum(m_diffusionMaxValue);
    m_strokeUpperValueSlider->setMinimum(m_diffusionMinValue);
    m_strokeUpperValueSlider->setMaximum(m_diffusionMaxValue);
    m_strokeLowerValueSlider->setValue(150);
    m_strokeUpperValueSlider->setValue(m_diffusionMaxValue);

    m_ventriclesLowerValueSpinBox->setMinimum(m_diffusionMinValue);
    m_ventriclesLowerValueSpinBox->setMaximum(m_diffusionMaxValue);
    m_ventriclesLowerValueSlider->setMinimum(m_diffusionMinValue);
    m_ventriclesLowerValueSlider->setMaximum(m_diffusionMaxValue);
    m_ventriclesLowerValueSlider->setValue(50);

    m_filterDiffusionPushButton->setEnabled(true);

    // TODO ara ho fem "a saco" però s'hauria de millorar
    m_diffusion2DView->setInput(m_diffusionMainVolume);
    m_diffusion2DView->render();

    connect(m_strokeLowerValueSlider, SIGNAL(valueChanged(int)), SLOT(viewThresholds(int)));
    connect(m_strokeUpperValueSlider, SIGNAL(valueChanged(int)), SLOT(viewThresholds(int)));
}

void QDifuPerfuSegmentationExtension::setPerfusionInput(Volume * input)
{
    if (!input)
    {
        ERROR_LOG("setPerfusionInput: null input");
        return;
    }

    m_perfusionImageStackedWidget->setCurrentWidget(m_selectPerfusionImagePage);

    m_perfusionInputVolume = input;

    m_selectedPerfusionImageSpinBox->setMaximum(m_perfusionInputVolume->getNumberOfPhases() -1);
    m_selectedPerfusionImageSpinBox->setValue(m_perfusionInputVolume->getNumberOfPhases() -1);

    m_perfusionSliceSlider->setMinimum(0);
    m_perfusionSliceSlider->setMaximum(m_perfusionInputVolume->getNumberOfSlicesPerPhase() -1);
    m_perfusionSliceSpinBox->setMinimum(0);
    m_perfusionSliceSpinBox->setMaximum(m_perfusionInputVolume->getNumberOfSlicesPerPhase() -1);

    m_applyRegistrationPushButton->setEnabled(true);

    m_perfusionSliceSlider->setValue(m_perfusion2DView->getCurrentSlice());
    setPerfusionImage(m_perfusionInputVolume->getNumberOfPhases() - 1);
}

void QDifuPerfuSegmentationExtension::setPerfusionLut(int threshold)
{
    m_perfusionHueLut->Delete();
    m_perfusionHueLut = vtkLookupTable::New();
    m_perfusionHueLut->SetTableRange(m_perfusionMinValue, m_perfusionMaxValue);
    m_perfusionHueLut->SetHueRange(1.0, 0.0);
    m_perfusionHueLut->SetSaturationRange(1.0, 1.0);
    m_perfusionHueLut->SetValueRange(1.0, 1.0);
    m_perfusionHueLut->SetAlphaRange(1.0, 1.0);
    m_perfusionHueLut->SetRampToLinear();
    m_perfusionHueLut->ForceBuild();    //effective built
    int nvalues=m_perfusionHueLut->GetNumberOfTableValues();

    for(int i=0;i<((threshold*nvalues)/255);i++)
    {
        double *tvalue = m_perfusionHueLut->GetTableValue(i);
        tvalue[0]=0.0;  //Posem els valors transparents
        tvalue[1]=0.0;  //Posem els valors transparents
        tvalue[2]=0.0;  //Posem els valors transparents
        tvalue[3]=0.0;  //Posem els valors transparents
        m_perfusionHueLut->SetTableValue(i, tvalue);
    }

    m_perfusionHueLut->Build();    //effective built

    vtkUnsignedCharArray * table = m_perfusionHueLut->GetTable();
    unsigned char tuple[4] = { 0, 0, 0, 0 };
    table->SetTypedTuple(0, tuple);
    table->SetTypedTuple(table->GetNumberOfTuples() - 1, tuple);

    // Transformem la vtkLookupTable a TransferFunction
    TransferFunction transferFunction(m_perfusionHueLut);
    m_perfusion2DView->setTransferFunction(transferFunction);
}

void QDifuPerfuSegmentationExtension::setMaxPerfusionImage(int max)
{
    m_selectedPerfusionImageSpinBox->setMaximum(max - 1);
}

void QDifuPerfuSegmentationExtension::setPerfusionImage(int index)
{
    m_perfusionMainVolume = m_perfusionInputVolume->getPhaseVolume(index);

    itk::MinimumMaximumImageCalculator< ItkImageType >::Pointer minmaxCalc = itk::MinimumMaximumImageCalculator< ItkImageType >::New();

    minmaxCalc->SetImage(m_perfusionMainVolume->getItkData());
    minmaxCalc->SetRegion(m_perfusionMainVolume->getItkData()->GetRequestedRegion());
    minmaxCalc->Compute();

    DEBUG_LOG(QString("ItkMax=%1, ItkMin=%2 -->(%3)").arg(minmaxCalc->GetMaximum()).arg(minmaxCalc->GetMinimum()).arg(index));

    m_perfusionMinValue = minmaxCalc->GetMinimum();
    m_perfusionMaxValue = minmaxCalc->GetMaximum();

     // TODO ara ho fem "a saco" però s'hauria de millorar
    m_perfusion2DView->setInput(m_perfusionMainVolume);

    //Ho fem per tal de que es vegi tot "blanc" i per tant en color "vius"
    m_perfusion2DView->setVoiLut(WindowLevel(1.0, m_perfusionMinValue - 1.0));
    //m_perfusion2DView->setVoiLut(WindowLevel(m_perfusionMaxValue - m_perfusionMinValue, 0.0));
    setPerfusionLut(m_perfusionThresholdViewerSlider->value());
}

void QDifuPerfuSegmentationExtension::setStrokeLowerValue(int x)
{
    if (x > m_strokeUpperValueSlider->value()) m_strokeUpperValueSlider->setValue(x);
}

void QDifuPerfuSegmentationExtension::setStrokeUpperValue(int x)
{
    if (x < m_strokeLowerValueSlider->value()) m_strokeLowerValueSlider->setValue(x);
}

void QDifuPerfuSegmentationExtension::viewThresholds()
{
    if (!m_strokeMaskVolume) m_strokeMaskVolume = new Volume();

    m_activedMaskVolume = m_strokeMaskVolume;

    vtkImageThreshold * imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInputData(m_diffusionMainVolume->getVtkData());
    //imageThreshold->SetInputData(m_diffusionInputVolume->getVtkData());
    imageThreshold->ThresholdBetween(m_strokeLowerValueSlider->value(),
                                      m_strokeUpperValueSlider->value());
    imageThreshold->SetInValue(m_diffusionMaxValue);
    imageThreshold->SetOutValue(m_diffusionMinValue);
    imageThreshold->Update();

    m_strokeMaskVolume->setImages(m_diffusionInputVolume->getImages());
    m_strokeMaskVolume->setData(imageThreshold->GetOutput());

    m_diffusion2DView->setOverlapMethod(Q2DViewer::Blend);
    m_diffusion2DView->setOverlayOpacity(m_diffusionOpacitySlider->value() / 100.0);
    m_diffusion2DView->setOverlayInput(m_strokeMaskVolume);

    m_diffusionOpacityLabel->setEnabled(true);
    m_diffusionOpacitySlider->setEnabled(true);

    m_diffusion2DView->render();
}

void QDifuPerfuSegmentationExtension::viewThresholds2()
{
    if (!m_penombraMaskVolume) m_penombraMaskVolume = new Volume();

    vtkImageThreshold * imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInputData(m_blackpointEstimatedVolume->getVtkData());
    //imageThreshold->SetInputData(m_diffusionInputVolume->getVtkData());
    imageThreshold->ThresholdBetween(m_penombraLowerValueSlider->value(), 1000000);
    imageThreshold->SetInValue(m_penombraMaskMaxValue);
    imageThreshold->SetOutValue(m_penombraMaskMinValue);
    imageThreshold->Update();

    m_penombraMaskVolume->setImages(m_diffusionMainVolume->getImages());
    m_penombraMaskVolume->setData(imageThreshold->GetOutput());

    vtkImageCast * imageCast = vtkImageCast::New();
    imageCast->SetInputData(m_penombraMaskVolume->getVtkData());
    imageCast->SetOutputScalarTypeToUnsignedChar();
    m_perfusionOverlay->SetInputData(imageCast->GetOutput());
    imageCast->Delete();


    m_perfusion2DView->render();
}

void QDifuPerfuSegmentationExtension::setSeedPosition(double x, double y, double z)
{
    m_seedPosition[0] = x;
    m_seedPosition[1] = y;
    m_seedPosition[2] = z;

    m_strokeSeedXLineEdit->setText(QString::number(m_seedPosition[0], 'f', 1));
    m_strokeSeedYLineEdit->setText(QString::number(m_seedPosition[1], 'f', 1));
    m_strokeSeedZLineEdit->setText(QString::number(m_seedPosition[2], 'f', 1));

    m_strokeApplyPushButton->setEnabled(true);
}

void QDifuPerfuSegmentationExtension::applyStrokeSegmentation()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!m_strokeSegmentationMethod) m_strokeSegmentationMethod = new StrokeSegmentationMethod();

    m_strokeSegmentationMethod->setVolume(m_diffusionMainVolume);

    if (!m_strokeMaskVolume) m_strokeMaskVolume = new Volume();

    m_strokeSegmentationMethod->setMask(m_strokeMaskVolume);
    m_strokeSegmentationMethod->setInsideMaskValue (m_diffusionMaxValue);
    m_strokeSegmentationMethod->setOutsideMaskValue(m_diffusionMinValue);
    m_strokeSegmentationMethod->setHistogramLowerLevel(m_strokeLowerValueSlider->value());
    m_strokeSegmentationMethod->setHistogramUpperLevel(m_strokeUpperValueSlider->value());
    m_strokeSegmentationMethod->setSeedPosition(m_seedPosition[0], m_seedPosition[1], m_seedPosition[2]);

    m_strokeVolume = m_strokeSegmentationMethod->applyMethod();
    m_strokeCont = (int)(m_strokeVolume / (m_diffusionMainVolume->getSpacing()[0]*m_diffusionMainVolume->getSpacing()[1]*m_diffusionMainVolume->getSpacing()[2]));

    m_diffusion2DView->setOverlapMethod(Q2DViewer::Blend);
    m_diffusion2DView->setOverlayOpacity(m_diffusionOpacitySlider->value() / 100.0);
    m_diffusion2DView->setOverlayInput(m_strokeMaskVolume);

    m_diffusionOpacityLabel->setEnabled(true);
    m_diffusionOpacitySlider->setEnabled(true);

    m_strokeVolumeLineEdit->setText(QString::number(m_strokeVolume, 'f', 2));
    m_strokeVolumeLabel->setEnabled(true);
    m_strokeVolumeLineEdit->setEnabled(true);

    m_editorToolButton->defaultAction()->trigger();
    m_editorToolButton->defaultAction()->setEnabled(true);

    m_lesionViewAction->setEnabled(true);
    m_lesionViewAction->trigger();
    this->viewLesionOverlay();

    QApplication::restoreOverrideCursor();
}

void QDifuPerfuSegmentationExtension::applyVentriclesMethod()
{
    if (!m_ventriclesMaskVolume) m_ventriclesMaskVolume = new Volume();

    vtkImageThreshold * imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInputData(m_diffusionMainVolume->getVtkData());
    imageThreshold->ThresholdBetween(m_ventriclesLowerValueSlider->value(), m_diffusionMaxValue);
    // Inverse mask --> we want < lower or > upper
    imageThreshold->SetInValue(m_diffusionMinValue);
    imageThreshold->SetOutValue(m_diffusionMaxValue);
    imageThreshold->Update();

    m_ventriclesMaskVolume->setData(imageThreshold->GetOutput());

    m_ventriclesViewAction->setEnabled(true);
    m_ventriclesViewAction->trigger();
    this->viewVentriclesOverlay();

    m_diffusionOpacityLabel->setEnabled(true);
    m_diffusionOpacitySlider->setEnabled(true);
}

void QDifuPerfuSegmentationExtension::applyRegistration()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ItkImageType::Pointer fixedImage = m_diffusionMainVolume->getItkData();
    ItkImageType::Pointer movingImage = m_perfusionMainVolume->getItkData();

//     std::cout<<"Spacing difu= "<<m_diffusionMainVolume->getItkData()->GetSpacing()<<std::endl;
//     std::cout<<"Spacing perfu= "<<m_perfusionMainVolume->getItkData()->GetSpacing()<<std::endl;

//     ItkImageType::SpacingType fixedSpacing = fixedImage->GetSpacing();
//     DEBUG_LOG("fixed spacing = " << fixedSpacing);
//     ItkImageType::SpacingType movingSpacing = movingImage->GetSpacing();
//     DEBUG_LOG("moving spacing = " << movingSpacing);

    itkRegistre3DAffine< ItkImageType, ItkImageType > registre;
    registre.SetInputImages(fixedImage, movingImage);
    registre.SetParamatersMetric(RegistrationFixedStandardDeviation,
                                  RegistrationMovingStandardDeviation,
                                  RegistrationNumberOfSpacialSamples);
    registre.SetParamatersGaussian(RegistrationFixedVariance, RegistrationMovingVariance);
    registre.SetParamatresOptimizer(RegistrationMaximumStep, RegistrationMinimumStep,
                                     RegistrationNumberOfIterations);

    if (registre.applyMethod())
    {
        typedef TransformType::InputPointType TransformPointType;
        typedef itk::ResampleImageFilter< ItkImageType, ItkImageType > ResampleGrisFilterType;
        typedef itk::RescaleIntensityImageFilter< ItkImageType, ItkImageType > RescaleFilterType;

        itkRegistre3DAffine< ItkImageType, ItkImageType >::OptimizerParametersType finalParameters;
        finalParameters = registre.getFinalParameters();

        if (!m_registerTransform) m_registerTransform = TransformType::New();
        m_registerTransform->SetParameters(finalParameters);

        // Recalculem el centre -> No ho fa bé! (???????)
        ItkImageType::SpacingType fixedSpacing = fixedImage->GetSpacing();
        ItkImageType::PointType fixedOrigin = fixedImage->GetOrigin();
        ItkImageType::SizeType fixedSize = fixedImage->GetLargestPossibleRegion().GetSize();
        TransformPointType centerFixed;
        centerFixed[0] = fixedOrigin[0] + fixedSpacing[0] * fixedSize[0] / 2.0;
        centerFixed[1] = fixedOrigin[1] + fixedSpacing[1] * fixedSize[1] / 2.0;
        centerFixed[2] = fixedOrigin[2] + fixedSpacing[2] * fixedSize[2] / 2.0;

        m_registerTransform->SetCenter(centerFixed);

        ResampleGrisFilterType::Pointer resample = ResampleGrisFilterType::New();
        resample->SetTransform(m_registerTransform);
        resample->SetInput(movingImage);
        resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
        resample->SetOutputOrigin(fixedImage->GetOrigin());
        resample->SetOutputSpacing(fixedImage->GetSpacing());
        resample->SetDefaultPixelValue(0);
        resample->Update();

        RescaleFilterType::Pointer rescalerPerfusion = RescaleFilterType::New();
        rescalerPerfusion->SetInput(resample->GetOutput());
        rescalerPerfusion->SetOutputMinimum(0);
        rescalerPerfusion->SetOutputMaximum(255);
        rescalerPerfusion->Update();

        if (!m_perfusionRescaledVolume) m_perfusionRescaledVolume = new Volume();

        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        //m_perfusionRescaledVolume->setImages(m_perfusionInputVolume->getImages());
        m_perfusionRescaledVolume->setImages(m_diffusionInputVolume->getPhaseImages(m_selectedDiffusionImageSpinBox->value()));

        m_perfusionRescaledVolume->setData(rescalerPerfusion->GetOutput());

        RescaleFilterType::Pointer rescalerDiffusion = RescaleFilterType::New();
        rescalerDiffusion->SetInput(m_diffusionMainVolume->getItkData());
        rescalerDiffusion->SetOutputMinimum(0);
        rescalerDiffusion->SetOutputMaximum(255);
        rescalerDiffusion->Update();

        if (!m_diffusionRescaledVolume) m_diffusionRescaledVolume = new Volume();

        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_diffusionRescaledVolume->setImages(m_diffusionInputVolume->getPhaseImages(m_selectedDiffusionImageSpinBox->value()));
        m_diffusionRescaledVolume->setData(rescalerDiffusion->GetOutput());

        m_perfusion2DView->setInput(m_perfusionRescaledVolume);

        m_perfusionSliceSlider->setMinimum(0);
        m_perfusionSliceSlider->setMaximum(m_perfusionRescaledVolume->getDimensions()[2] -1);
        m_perfusionSliceSpinBox->setMinimum(0);
        m_perfusionSliceSpinBox->setMaximum(m_perfusionRescaledVolume->getDimensions()[2] -1);

        vtkLookupTable * hueLut = vtkLookupTable::New();
        hueLut->SetTableRange(0.0, 255.0);
        hueLut->SetHueRange(1.0, 0.0);
        hueLut->SetSaturationRange(1.0, 1.0);
        hueLut->SetValueRange(1.0, 1.0);
        hueLut->SetAlphaRange(1.0, 1.0);
        hueLut->SetRampToLinear();
        hueLut->Build();    //effective built
        int nvalues=hueLut->GetNumberOfTableValues();

        for(int i=0;i<(m_perfusionThresholdViewerSlider->value()*nvalues)/255;i++)
        {
            double *tvalue = hueLut->GetTableValue(i);
            tvalue[0]=0.0;  //Posem els valors transparents
            tvalue[1]=0.0;  //Posem els valors transparents
            tvalue[2]=0.0;  //Posem els valors transparents
            tvalue[3]=0.0;  //Posem els valors transparents
            hueLut->SetTableValue(i, tvalue);
        }

        hueLut->Build();    //effective built

        vtkUnsignedCharArray * table = hueLut->GetTable();
        unsigned char tuple[4] = { 0, 0, 0, 0 };
        table->SetTypedTuple(0, tuple);
        table->SetTypedTuple(table->GetNumberOfTuples() - 1, tuple);

        TransferFunction hueTransferFunction(hueLut);
        m_perfusion2DView->setTransferFunction(hueTransferFunction);

        vtkImageCast * imageCast = vtkImageCast::New();
        imageCast->SetInputData(m_diffusionRescaledVolume->getVtkData());
        imageCast->SetOutputScalarTypeToUnsignedChar();

        if (!m_perfusionOverlay) m_perfusionOverlay = vtkImageActor::New();
        m_perfusionOverlay->SetInputData(imageCast->GetOutput());
        m_perfusionOverlay->SetOpacity(m_perfusionOpacitySlider->value() / 100.0);
        m_perfusionOverlay->SetZSlice(m_perfusionSliceSlider->value());
        // el posem una mica més endavant per assegurar que es vegi
        m_perfusionOverlay->SetPosition(0.0, 0.0, -1.0);

        if (!m_perfusion2DView->getRenderer()->HasViewProp(m_perfusionOverlay))
            m_perfusion2DView->getRenderer()->AddViewProp(m_perfusionOverlay);

        //m_perfusion2DView->setVoiLut(WindowLevel(m_diffusion2DView->getCurrentColorWindow(), m_diffusion2DView->getCurrentColorLevel()));
        m_perfusion2DView->setVoiLut(WindowLevel(255.0, 0.0));

        m_perfusionSliceSlider->setValue(m_diffusionSliceSlider->value());
        m_perfusion2DView->setSlice(m_perfusionSliceSlider->value());

        m_computeBlackpointEstimationPushButton->setEnabled(true);
        m_saveRegisteredPerfusionPushButton->setEnabled(true);

        m_perfusionOpacityLabel->setEnabled(true);
        m_perfusionOpacitySlider->setEnabled(true);

        imageCast->Delete();

    }
    else
    {
        QMessageBox::warning(this, tr("Registration failed!"), tr("Registration failed!"));
    }

    QApplication::restoreOverrideCursor();
}

void QDifuPerfuSegmentationExtension::computeBlackpointEstimation()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    typedef itk::RescaleIntensityImageFilter< Volume::ItkImageType, Volume::ItkImageType > RescaleFilterType;

    RescaleFilterType::Pointer rescaler2 = RescaleFilterType::New();
    RescaleFilterType::Pointer rescaler3 = RescaleFilterType::New();

    rescaler2->SetInput(m_ventriclesMaskVolume->getItkData());
    rescaler2->SetOutputMinimum(0);
    rescaler2->SetOutputMaximum(255);
    rescaler2->Update();
//     Volume * r2 = new Volume();
//     r2->setData(rescaler2->GetOutput());
//     Q2DViewerExtension * win2 = new Q2DViewerExtension();
//     win2->setInput(r2);
//     win2->show();

    rescaler3->SetInput(m_strokeMaskVolume->getItkData());
    rescaler3->SetOutputMinimum(0);
    rescaler3->SetOutputMaximum(255);
    rescaler3->Update();
//     Volume * r3 = new Volume();
//     r3->setData(rescaler3->GetOutput());
//     Q2DViewerExtension * win3 = new Q2DViewerExtension();
//     win3->setInput(r3);
//     win3->show();

    udgPerfusionEstimator< Volume::ItkImageType, Volume::ItkImageType, TransformType> blackpointEstimator;

    blackpointEstimator.SetPerfuImage(m_perfusionRescaledVolume->getItkData());
    blackpointEstimator.SetVentricleMask(rescaler2->GetOutput());
    blackpointEstimator.SetStrokeMask(rescaler3->GetOutput());
    DEBUG_LOG("pas1");

    // Aquí apliquem la transformació que ens ha donat el registre
    blackpointEstimator.SetTransform(m_registerTransform);
    DEBUG_LOG("pas2");
    blackpointEstimator.ComputeEstimation();
    DEBUG_LOG("pas3");
    Volume::ItkImageTypePointer perfuEstimatorImageResult = blackpointEstimator.GetEstimatedImage();
    DEBUG_LOG("pas4");


    m_blackpointEstimatedVolume = new Volume();
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    //m_blackpointEstimatedVolume->setImages(m_perfusionInputVolume->getImages());
    m_blackpointEstimatedVolume->setImages(m_diffusionInputVolume->getPhaseImages(m_selectedDiffusionImageSpinBox->value()));

    m_blackpointEstimatedVolume->setData(perfuEstimatorImageResult);

    connect(m_penombraLowerValueSlider, SIGNAL(valueChanged(int)), SLOT(viewThresholds2()));



//     vtkImageCast * imageCast = vtkImageCast::New();
//     imageCast->SetInputData(m_blackpointEstimatedVolume->getVtkData());
//     imageCast->SetOutputScalarTypeToUnsignedChar();
//     m_perfusionOverlay->SetInput(imageCast->GetOutput());
//     m_perfusion2DView->setWindowLevel(255.0, 0.0);


//     Volume * resultat = new Volume();
//     DEBUG_LOG("volum creat");
//     resultat->setData(perfuEstimatorImageResult);
//     DEBUG_LOG("data set");
//     Q2DViewerExtension * win = new Q2DViewerExtension();
//     DEBUG_LOG("extensió creada");
//     win->setInput(resultat);
//     DEBUG_LOG("input set");
//     win->show();
//     DEBUG_LOG("showed");

    QApplication::restoreOverrideCursor();

    m_penombraApplyPushButton->setEnabled(true);
}

void QDifuPerfuSegmentationExtension::applyPenombraSegmentation()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ItkImageType::PointType seedPoint(m_seedPosition);
    ItkImageType::IndexType seedIndex;
    m_blackpointEstimatedVolume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
//     DEBUG_LOG(QString("seedPosition: [%1,%2,%3]").arg(m_seedPosition[0]).arg(m_seedPosition[1]).arg(m_seedPosition[2]));
//     std::cout << "seedPoint: " << seedPoint << std::endl;
//     std::cout << "seedIndex: " << seedIndex << std::endl;
//     std::cout<<"BPImage sp="<<m_blackpointEstimatedVolume->getItkData()->GetSpacing()<<std::endl;
//     std::cout<<"BPImage size="<<m_blackpointEstimatedVolume->getItkData()->GetLargestPossibleRegion().GetSize()<<std::endl;

    udgBinaryMaker< ItkImageType, ItkImageType > binaritzador;
    ItkImageType::Pointer penombraMask = ItkImageType::New();
    binaritzador.SetInsideValue(m_penombraMaskMaxValue);
    // el tercer paràmetre (strokelevel) ha de ser el valor mínim de la zona a segmentar
    // el quart paràmetre (max) ha de ser el valor màxim de la zona a segmentar (255)
    binaritzador.PenombraSegmentation(m_blackpointEstimatedVolume->getItkData(), penombraMask,
                                       m_penombraLowerValueSlider->value(), 255, seedIndex);

    delete m_penombraMaskVolume;
    m_penombraMaskVolume = new Volume();
    m_penombraMaskVolume->setImages(m_diffusionInputVolume->getPhaseImages(m_selectedDiffusionImageSpinBox->value()));
    m_penombraMaskVolume->setData(penombraMask);

    vtkImageCast * imageCast = vtkImageCast::New();
    imageCast->SetInputData(m_penombraMaskVolume->getVtkData());
    imageCast->SetOutputScalarTypeToUnsignedChar();
    m_perfusionOverlay->SetInputData(imageCast->GetOutput());
    imageCast->Delete();

    //m_perfusion2DView->setInput(m_perfusionRescaledVolume);
    //m_perfusion2DView->setInput(m_diffusionMainVolume);
    m_perfusion2DView->setVoiLut(WindowLevel(1.0, m_perfusionMinValue - 1.0));
    m_perfusion2DView->setSlice(m_perfusionSliceSlider->value());
    m_perfusion2DView->setOverlapMethod(Q2DViewer::None);
    m_perfusion2DView->setOverlayInput(m_penombraMaskVolume);
    m_perfusion2DView->render();
    //Posem els 2 viewers a la mateixa llesca i els sincronitzem
    m_diffusion2DView->setSlice(m_perfusionSliceSlider->value());
    m_synchroCheckBox->setChecked(true);
    m_lesionViewToolButton->click();

    VolumeCalculator volumeCalculator;
    volumeCalculator.setInput(m_penombraMaskVolume);
    volumeCalculator.setInsideValue(m_penombraMaskMaxValue);
    m_penombraVolumeLabel->setEnabled(true);
    m_penombraLineEdit->setEnabled(true);
    m_penombraLabel->setEnabled(true);
    m_penombraVolumeLineEdit->setEnabled(true);
    m_penombraVolume = volumeCalculator.getVolume();
    m_penombraVolumeLineEdit->setText(QString::number(m_penombraVolume, 'f', 2));
    m_penombraCont = volumeCalculator.getVoxels();
    //std::cout<<"penombra cont= "<<m_penombraCont<<" // voxel size= "<<m_penombraMaskVolume->getItkData()->GetSpacing()<<std::endl;
    //std::cout<<"penombra vol= "<<m_penombraVolume<<std::endl;

    QApplication::restoreOverrideCursor();
}

void QDifuPerfuSegmentationExtension::applyFilterDiffusionImage()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);


    if (!m_strokeSegmentationMethod) m_strokeSegmentationMethod = new StrokeSegmentationMethod();

    m_strokeSegmentationMethod->setVolume(m_diffusionMainVolume);
    Volume * filteredVolume = new Volume();
    m_strokeSegmentationMethod->applyFilter(filteredVolume);

    delete m_diffusionMainVolume;
    m_diffusionMainVolume = filteredVolume;

    m_diffusion2DView->setInput(m_diffusionMainVolume);
    m_diffusion2DView->render();

    m_filterDiffusionPushButton->setEnabled(false);

    QApplication::restoreOverrideCursor();
}

void QDifuPerfuSegmentationExtension::setDiffusionOpacity(int opacity)
{
    if (m_activedMaskVolume != 0)
    {
        m_diffusion2DView->setOverlayOpacity(((double)opacity)/100.0);
        m_diffusion2DView->setOverlayInput(m_activedMaskVolume);
        m_diffusion2DView->render();
    }
}

void QDifuPerfuSegmentationExtension::setPerfusionOpacity(int opacity)
{
    m_perfusionOverlay->SetOpacity(opacity / 100.0);
    m_perfusion2DView->render();
}


void QDifuPerfuSegmentationExtension::viewLesionOverlay()
{
    if(m_strokeMaskVolume != 0)
    {
        m_activedMaskVolume = m_strokeMaskVolume;
        m_diffusion2DView->setOverlapMethod(Q2DViewer::Blend);
        m_diffusion2DView->setOverlayOpacity(((double)m_diffusionOpacitySlider->value())/100.0);
        m_diffusion2DView->setOverlayInput(m_strokeMaskVolume);
        m_diffusion2DView->render();
    }
}

void QDifuPerfuSegmentationExtension::viewVentriclesOverlay()
{
    if(m_ventriclesMaskVolume != 0)
    {
        m_activedMaskVolume = m_ventriclesMaskVolume;
        m_diffusion2DView->setOverlapMethod(Q2DViewer::Blend);
        m_diffusion2DView->setOverlayOpacity(((double)m_diffusionOpacitySlider->value())/100.0);
        m_diffusion2DView->setOverlayInput(m_ventriclesMaskVolume);
        m_diffusion2DView->render();
    }
}

void QDifuPerfuSegmentationExtension::moveViewerSplitterToLeft()
{
    DEBUG_LOG("Move L");
    QList<int> splitterSize = m_viewerSplitter->sizes();
    int suma = splitterSize[0]+splitterSize[1];
    splitterSize[0]=0;
    splitterSize[1]=suma;
    m_viewerSplitter->setSizes(splitterSize);
}

void QDifuPerfuSegmentationExtension::moveViewerSplitterToRight()
{
    DEBUG_LOG("Move R");
    QList<int> splitterSize = m_viewerSplitter->sizes();
    int suma = splitterSize[0]+splitterSize[1];
    splitterSize[0]=suma;
    splitterSize[1]=0;
    m_viewerSplitter->setSizes(splitterSize);
}

void QDifuPerfuSegmentationExtension::moveViewerSplitterToCenter()
{
    DEBUG_LOG("Move C");
    QList<int> splitterSize = m_viewerSplitter->sizes();
    int suma = splitterSize[0]+splitterSize[1];
    splitterSize[0]=suma/2;
    splitterSize[1]=suma - suma/2;
    m_viewerSplitter->setSizes(splitterSize);
}

void QDifuPerfuSegmentationExtension::synchronizeSlices(bool sync)
{
    if (sync)
    {
        connect(m_diffusionSliceSlider, SIGNAL(valueChanged(int)), m_perfusionSliceSlider, SLOT(setValue(int)));
        connect(m_perfusionSliceSlider, SIGNAL(valueChanged(int)), m_diffusionSliceSlider, SLOT(setValue(int)));
    }
    else
    {
        disconnect(m_diffusionSliceSlider, SIGNAL(valueChanged(int)), m_perfusionSliceSlider, SLOT(setValue(int)));
        disconnect(m_perfusionSliceSlider, SIGNAL(valueChanged(int)), m_diffusionSliceSlider, SLOT(setValue(int)));
    }
}

void QDifuPerfuSegmentationExtension::setPerfusionSlice(int slice)
{
    if (m_perfusionOverlay) m_perfusionOverlay->SetZSlice(slice);
    m_perfusion2DView->render();
}

void QDifuPerfuSegmentationExtension::computePenombraVolume()
{
    m_penombraLineEdit->clear();
    m_penombraLineEdit->insert(QString("%1").arg(m_penombraVolume - m_strokeVolume, 0, 'f', 2));
}

void QDifuPerfuSegmentationExtension::saveDiffusionMask()
{
    if(m_strokeMaskVolume)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Diffusion Mask Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "mhd")
            {
                fileName += ".mhd";
            }
            //Forcem que la màscara que gaurdem el dins sigui 255 i el fora 0
            vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
            imageThreshold->SetInputData(m_strokeMaskVolume->getVtkData());
            // només els que valen m_diffusionMaxValue
            imageThreshold->ThresholdBetween(m_diffusionMaxValue, m_diffusionMaxValue); 
            imageThreshold->SetInValue(255);
            imageThreshold->SetOutValue(0);
    
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
            writer->SetFileName(qPrintable(fileName));
            writer->SetFileDimensionality(3);
            writer->SetInputConnection(imageThreshold->GetOutputPort());
            writer->Write();
    
            writer->Delete();
            imageThreshold->Delete();
        }
    }
}

void QDifuPerfuSegmentationExtension::savePerfusionMask()
{
    if(m_penombraMaskVolume)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Perfusion Mask Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "mhd")
            {
                fileName += ".mhd";
            }
            //Forcem que la màscara que gaurdem el dins sigui 255 i el fora 0
            vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
            imageThreshold->SetInputData(m_penombraMaskVolume->getVtkData());
            // només els que valen m_penombraMaskMaxValue
            imageThreshold->ThresholdBetween(m_penombraMaskMaxValue, m_penombraMaskMaxValue); 
            imageThreshold->SetInValue(255);
            imageThreshold->SetOutValue(0);
    
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
            writer->SetFileName(qPrintable(fileName));
            writer->SetFileDimensionality(3);
            writer->SetInputConnection(imageThreshold->GetOutputPort());
            writer->Write();
    
            writer->Delete();
            imageThreshold->Delete();
        }
    }
}

void QDifuPerfuSegmentationExtension::saveDiffusionVolume()
{
    if(m_diffusionMainVolume)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Diffusion Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "mhd")
            {
                fileName += ".mhd";
            }
    
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
            writer->SetFileName(qPrintable(fileName));
            writer->SetFileDimensionality(3);
            writer->SetInputData(m_diffusionMainVolume->getVtkData());
            writer->Write();
    
            writer->Delete();
        }
    }
}

void QDifuPerfuSegmentationExtension::savePerfusionVolume()
{
    if(m_perfusionMainVolume)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Perfusion Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "mhd")
            {
                fileName += ".mhd";
            }
    
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
            writer->SetFileName(qPrintable(fileName));
            writer->SetFileDimensionality(3);
            writer->SetInputData(m_perfusionMainVolume->getVtkData());
            writer->Write();
    
            writer->Delete();
        }
    }
}

void QDifuPerfuSegmentationExtension::saveRegisteredPerfusionVolume()
{
    if(m_perfusionRescaledVolume)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Perfusion Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "mhd")
            {
                fileName += ".mhd";
            }
    
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            vtkMetaImageWriter *writer = vtkMetaImageWriter::New();
            writer->SetFileName(qPrintable(fileName));
            writer->SetFileDimensionality(3);
            writer->SetInputData(m_perfusionRescaledVolume->getVtkData());
            writer->Write();
    
            writer->Delete();
        }
    }
}

void QDifuPerfuSegmentationExtension::saveTransform()
{
    if(m_registerTransform)
    {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Transform file") , m_savingMaskDirectory, tr("Transform Files (*.tf)"));
        if (!fileName.isEmpty())
        {
            if(QFileInfo(fileName).suffix() != "tf")
            {
                fileName += ".tf";
            }
            m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
            ofstream fout(qPrintable(fileName));
            //DEBUG_LOG(qPrintable(fileName));
    /*
            //old fashion
            for(unsigned int i=0;i<landmarkRegTransform->GetNumberOfParameters();i++)
            {
                fout<<landmarkRegTransform->GetParameters()[i]<<std::endl;
            }
            //fout<<landmarkRegTransform->GetParameters()<<std::endl;
            fout<<landmarkRegTransform->GetCenter()[0]<<" "<<landmarkRegTransform->GetCenter()[1]<<" "<<landmarkRegTransform->GetCenter()[2]<<" "<<std::endl;
            */
            unsigned int i,j;
            for(i=0;i<3;i++)
            {
                for(j=0;j<3;j++)
                {
                    fout<<m_registerTransform->GetMatrix()[i][j]<<std::endl;
                }
            }
            for(i=0;i<3;i++)
            {
                fout<<m_registerTransform->GetOffset()[i]<<std::endl;
            }
            fout.close();
        }
    }
}

double QDifuPerfuSegmentationExtension::calculateDiffusionMaskVolume()
{
    if (!m_strokeMaskVolume) 
        return 0.0;

    double spacing[3];
    m_strokeMaskVolume->getSpacing(spacing);
    double volume = 1.0;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    int cont;
    EditorToolData* edToolData = static_cast<EditorToolData*> (m_diffusion2DView->getToolProxy()->getTool("EditorTool")->getToolData());
    if(edToolData!=0)
    {
        cont = edToolData->getVolumeVoxels();
    }
    else
    {
        DEBUG_LOG("No existeix la editor tool");
        cont = 0;
    }

    m_strokeVolume = volume*(double)cont;

    return m_strokeVolume;
}

double QDifuPerfuSegmentationExtension::calculatePerfusionMaskVolume()
{
    if (!m_penombraMaskVolume) 
        return 0.0;

    double spacing[3];
    m_penombraMaskVolume->getSpacing(spacing);
    double volume = 1.0;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    int cont;
    EditorToolData* edToolData = static_cast<EditorToolData*> (m_perfusion2DView->getToolProxy()->getTool("EditorTool")->getToolData());
    if(edToolData!=0)
    {
        cont = edToolData->getVolumeVoxels();
    }
    else
    {
        DEBUG_LOG("No existeix la editor tool");
        cont = 0;
    }

    m_penombraVolume = volume*(double)cont;

    return m_penombraVolume;
}

void QDifuPerfuSegmentationExtension::updateStrokeVolume()
{
    if(m_strokeVolumeLineEdit->isEnabled() && m_activedMaskVolume == m_strokeMaskVolume)
    {
        m_strokeVolume = this->calculateDiffusionMaskVolume();
        m_strokeVolumeLineEdit->clear();
        m_strokeVolumeLineEdit->insert(QString("%1").arg(m_strokeVolume, 0, 'f', 2));
    }
}

void QDifuPerfuSegmentationExtension::updatePenombraVolume()
{
    if(m_penombraVolumeLineEdit->isEnabled())
    {
        //this->viewThresholds2();
        m_penombraVolume = this->calculatePerfusionMaskVolume();
        m_penombraVolumeLineEdit->clear();
        m_penombraVolumeLineEdit->insert(QString("%1").arg(m_penombraVolume, 0, 'f', 2));
        m_penombraLineEdit->clear();
        m_penombraLineEdit->insert(QString("%1").arg(m_penombraVolume - m_strokeVolume, 0, 'f', 2));

        vtkImageCast * imageCast = vtkImageCast::New();
        imageCast->SetInputData(m_penombraMaskVolume->getVtkData());
        imageCast->SetOutputScalarTypeToUnsignedChar();
        m_perfusionOverlay->SetInputData(imageCast->GetOutput());
        imageCast->Delete();

        m_perfusion2DView->setVoiLut(WindowLevel(1.0, m_perfusionMinValue - 1.0));
        m_perfusion2DView->setOverlapMethod(Q2DViewer::None);
        m_perfusion2DView->setOverlayInput(m_penombraMaskVolume);
        m_perfusion2DView->render();

    }
}

}


