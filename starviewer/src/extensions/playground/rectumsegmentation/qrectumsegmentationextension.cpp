/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qrectumsegmentationextension.h"

#include "rectumSegmentationMethod.h"
#include "volume.h"
#include "logging.h"
#include "q2dviewer.h"
#include "toolmanager.h"
#include "toolproxy.h"
#include "editortool.h"
#include "editortooldata.h"
#include "rectumsegmentationsettings.h"
#include "patientbrowsermenu.h"
#include "volumepixeldataiterator.h"

//Qt
#include <QString>
#include <QAction>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QButtonGroup>

// VTK
#include <vtkRenderer.h>
#include <vtkImageMask.h>
#include <vtkImageThreshold.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkImageIterator.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkContourGrid.h>
#include <vtkCommand.h>
#include <vtkMetaImageWriter.h>

// ITK
#include <itkBinaryThresholdImageFilter.h>

// prova isomètric
#include <itkLinearInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>

//prova recte
#include <itkRescaleIntensityImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkImageFileWriter.h>

namespace udg {

QRectumSegmentationExtension::QRectumSegmentationExtension(QWidget *parent)
 : QWidget(parent), m_mainVolume(0), m_lesionMaskVolume(0), m_imageThreshold(0), m_filteredVolume(0), m_isSeed(false), m_isMask(false),   m_isLeftButtonPressed(false), m_volume(0.0), m_isRegionSet(false), m_isRegionSetting(false)
{
    setupUi(this);
    RectumSegmentationSettings().init();

    m_segMethod = new rectumSegmentationMethod();
    squareRegionActor = vtkActor::New();

    m_seedPosition = QVector<double> (3);

    initializeTools();
    createConnections();
    readSettings();
}

QRectumSegmentationExtension::~QRectumSegmentationExtension()
{
    writeSettings();
    delete m_segMethod;
    squareRegionActor->Delete();
    if(m_filteredVolume != 0)
    {
        delete m_filteredVolume;
    }
    if(m_lesionMaskVolume != 0)
    {
        delete m_lesionMaskVolume;
    }
    if(m_imageThreshold != 0)
    {
        m_imageThreshold->Delete();
    }
}

void QRectumSegmentationExtension::initializeTools()
{
    m_regionAction = new QAction(m_regionToolButton);
    m_regionAction->setText(tr("RegionTool"));
    m_regionAction->setStatusTip(tr("Enable/Disable region tool"));
    m_regionAction->setCheckable(true);
    m_regionAction->setEnabled(true);
    m_regionAction->setIcon(QIcon(":/images/icons/roi.svg"));
    m_regionToolButton->setDefaultAction(m_regionAction);

    //Apanyo que hem de fer per tal de detectar quan s'activa una tool 
    // que no es pugui fer servir amb l'eina de marcar regió (que no és una Tool estàndar)
    m_toolsButtonGroup = new QButtonGroup(this);
    m_toolsButtonGroup->setExclusive(false);
    m_toolsButtonGroup->addButton(m_zoomToolButton, 1);
    m_toolsButtonGroup->addButton(m_slicingToolButton, 2);
    m_toolsButtonGroup->addButton(m_editorToolButton, 3);
    m_toolsButtonGroup->addButton(m_seedToolButton, 4);
    m_toolsButtonGroup->addButton(m_regionToolButton, 0);

    // Tools
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
    m_toolManager->registerTool("SlicingKeyboardTool");

    // Activem les tools que volem tenir per defecte, això és com si clickéssim a cadascun dels ToolButton 
    QStringList defaultTools;
    defaultTools << "SlicingMouseTool" << "TranslateTool" << "WindowLevelTool" << "SlicingKeyboardTool";
    m_toolManager->triggerTools(defaultTools);

    // definim els grups exclusius
    QStringList leftButtonExclusiveTools;
    leftButtonExclusiveTools << "ZoomTool" << "SlicingMouseTool" << "EditorTool" << "SeedTool";
    m_toolManager->addExclusiveToolsGroup("LeftButtonGroup", leftButtonExclusiveTools);

    QStringList rightButtonExclusiveTools;
    rightButtonExclusiveTools << "WindowLevelTool";
    m_toolManager->addExclusiveToolsGroup("RightButtonGroup", rightButtonExclusiveTools);

    QStringList middleButtonExclusiveTools;
    middleButtonExclusiveTools << "TranslateTool";
    m_toolManager->addExclusiveToolsGroup("MiddleButtonGroup", middleButtonExclusiveTools);

    // inicialització de les tools
    m_toolManager->setupRegisteredTools(m_2DView);

    // Action Tools
    m_rotateClockWiseToolButton->setDefaultAction(m_toolManager->registerActionTool("RotateClockWiseActionTool"));
    m_toolManager->enableRegisteredActionTools(m_2DView);
}

void QRectumSegmentationExtension::createConnections()
{
    connect(m_filterPushButton, SIGNAL(clicked()), SLOT(ApplyFilterMainImage()));
    connect(m_applyMethodButton, SIGNAL(clicked()), SLOT(ApplyMethod()));
    connect(m_updateVolumeButton, SIGNAL(clicked()), SLOT(updateVolume()));
    connect(m_viewThresholdButton, SIGNAL(clicked()), SLOT(viewThresholds()));
    connect(m_2DView, SIGNAL(eventReceived(unsigned long)), SLOT(strokeEventHandler(unsigned long)));
    connect(m_sliceViewSlider, SIGNAL(valueChanged(int)) , m_2DView , SLOT(setSlice(int)));
    connect(m_2DView, SIGNAL(sliceChanged(int)) , m_sliceViewSlider , SLOT(setValue(int)));
    connect(m_lowerValueSlider, SIGNAL(valueChanged(int)), SLOT(setLowerValue(int)));
    connect(m_upperValueSlider, SIGNAL(valueChanged(int)), SLOT(setUpperValue(int)));
    connect(m_opacitySlider, SIGNAL(valueChanged(int)), SLOT(setOpacity(int)));
    connect(m_2DView, SIGNAL(seedPositionChanged(double, double, double)), SLOT(setSeedPosition(double, double, double)));
    // Fem que no s'assigni automàticament l'input que s'ha seleccionat amb el menú de pacient, ja que fem tractaments adicionals
    // sobre el volum seleccionat i l'input final del visor pot diferir de l'inicial i és l'extensió qui decideix finalment quin input
    // se li vol donar a cada viewer. Capturem la senyal de quin volum s'ha escollit i a partir d'aquí fem el que calgui
    m_2DView->setAutomaticallyLoadPatientBrowserMenuSelectedInput(false);
    connect(m_2DView->getPatientBrowserMenu(), SIGNAL(selectedVolume(Volume*)), SLOT(setInput(Volume*)));
    connect(m_2DView, SIGNAL(overlayChanged()), SLOT(updateVolumeForced()));
    connect(m_2DView, SIGNAL(overlayModified()), SLOT(updateVolume()));
    connect(m_saveMaskPushButton, SIGNAL(clicked()), SLOT(saveActivedMaskVolume()));
    connect(m_save3DPushButton, SIGNAL(clicked()), SLOT(saveSegmentation3DVolume()));
    connect(m_viewROICheckBox, SIGNAL(stateChanged(int)), SLOT(viewRegionState(int)));
    connect(m_toolsButtonGroup, SIGNAL(buttonReleased(int)), SLOT(toolChanged(int)));
}

void QRectumSegmentationExtension::setInput(Volume *input)
{
    m_mainVolume = input;
    m_2DView->setInput(input);
    this->updateInputFeatures(input);
    m_2DView->removeAnnotation(AllAnnotations);
    m_2DView->setOverlapMethod(Q2DViewer::Blend);
}

void QRectumSegmentationExtension::updateInputFeatures(Volume *input)
{
    int* dim;
    dim = input->getDimensions();
    m_sliceViewSlider->setMinimum(0);
    m_sliceViewSlider->setMaximum(dim[2]-1);
    m_sliceSpinBox->setMinimum(0);
    m_sliceSpinBox->setMaximum(dim[2]-1);
    m_sliceViewSlider->setValue(m_2DView->getCurrentSlice());

    // Posem els nivells de dins i fora de la màscara per tal que es vegi correcte
    double range[2];
    input->getScalarRange(range);
    m_insideValue = range[1] - range[0];
    m_outsideValue = range[0];

    typedef itk::ImageRegionConstIterator<Volume::ItkImageType> ConstIterator;
    ConstIterator iter(input->getItkData(), input->getItkData()->GetBufferedRegion());

    m_minValue = iter.Get();
    m_maxValue = m_minValue;

    Volume::ItkImageType::PixelType value;

    while (!iter.IsAtEnd())
    {
        value = iter.Get();

        if (value < m_minValue) { m_minValue = value; }
        if (value > m_maxValue) { m_maxValue = value; }

        ++iter;
    }
    m_lowerValueSpinBox->setMinimum(m_minValue);
    m_lowerValueSpinBox->setMaximum(m_maxValue);
    m_upperValueSpinBox->setMinimum(m_minValue);
    m_upperValueSpinBox->setMaximum(m_maxValue);
    m_lowerValueSlider->setMinimum(m_minValue);
    m_lowerValueSlider->setMaximum(m_maxValue);
    m_upperValueSlider->setMinimum(m_minValue);
    m_upperValueSlider->setMaximum(m_maxValue);
    //empirical values!!
    //m_lowerValueSlider->setValue(35);   //Ara ho fem pel QtDesigner
    //m_upperValueSlider->setValue(170);  //Ara ho fem pel QtDesigner
}

void QRectumSegmentationExtension::ApplyFilterMainImage()
{
    if(m_filteredVolume == 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_segMethod->setVolume(m_mainVolume);
        m_filteredVolume = new Volume();
        //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
        m_filteredVolume->setImages(m_mainVolume->getImages());
        m_segMethod->applyFilter(m_filteredVolume);
        m_segMethod->setVolume(m_filteredVolume);
        m_2DView->setInput(m_filteredVolume);
        m_2DView->render();
        //delete m_mainVolume;
        QApplication::restoreOverrideCursor();
    }
}

void QRectumSegmentationExtension::ApplyMethod()
{
    if(!m_isSeed || !m_isMask){
        QMessageBox::critical(this , tr("StarViewer") , tr("ERROR: seed or mask undefined"));
        return;
    }
    if(!m_isRegionSet)
    {
        QMessageBox::warning(this , QObject::tr("Starviewer") , QObject::tr("undefined ROI"));
        return;
    }

    if(m_filteredVolume == 0)
    {
        m_segMethod->setVolume(m_mainVolume);
    }
    else
    {
        m_segMethod->setVolume(m_filteredVolume);
    }
    if(m_lesionMaskVolume == 0)
    {
        m_lesionMaskVolume = new Volume();
    }
    m_lesionMaskVolume->setImages(m_mainVolume->getImages());
    m_segMethod->setMask(m_lesionMaskVolume);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_segMethod->setInsideMaskValue (m_insideValue);
    m_segMethod->setOutsideMaskValue(m_outsideValue);
    m_segMethod->setMultiplier(m_multiplierEdit->text().toDouble());

    m_segMethod->setSeedPosition(m_seedPosition[0],m_seedPosition[1],m_seedPosition[2]);
    int x[2];
    int y[2];
    if(m_initialRegionPoint[0]<m_finalRegionPoint[0])
    {
        x[0]=(m_initialRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
        y[0]=(m_finalRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
    }
    else
    {
        x[0]=(m_finalRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
        y[0]=(m_initialRegionPoint[0]-m_mainVolume->getOrigin()[0])/m_mainVolume->getSpacing()[0];
    }
    if(m_initialRegionPoint[1]<m_finalRegionPoint[1])
    {
        x[1]=(m_initialRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
        y[1]=(m_finalRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
    }
    else
    {
        x[1]=(m_finalRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
        y[1]=(m_initialRegionPoint[1]-m_mainVolume->getOrigin()[1])/m_mainVolume->getSpacing()[1];
    }
    m_segMethod->setMinROI(x);
    m_segMethod->setMaxROI(y);

    m_volume = m_segMethod->applyMethod();

    this->viewLesionOverlay();

    m_resultsLineEdit->clear();
    m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    m_resultsLineEdit->setEnabled(true);
    m_resultsLabel->setEnabled(true);
    m_updateVolumeButton->setEnabled(true);
    m_regionToolButton->setChecked(false);
    m_editorToolButton->defaultAction()->trigger();
    QApplication::restoreOverrideCursor();
}

void QRectumSegmentationExtension::strokeEventHandler(unsigned long id)
{
    switch (id)
    {
    case vtkCommand::MouseMoveEvent:
        onMouseMoveEventHandler();
    break;

    case vtkCommand::LeftButtonPressEvent:
        leftButtonEventHandler();
    break;

    case vtkCommand::LeftButtonReleaseEvent:
        leftButtonReleaseHandler();
    break;

    case vtkCommand::RightButtonPressEvent:
    break;

    default:
    break;
    }
}

void QRectumSegmentationExtension::onMouseMoveEventHandler()
{
    if(m_regionToolButton->isChecked())    //Només en cas que estiguem en la selecció de regió d'interés
    {
        setMovingRegionOfInterest();
    }
}

void QRectumSegmentationExtension::leftButtonEventHandler()
{
    m_isLeftButtonPressed = true;

    if(m_regionToolButton->isChecked())
    {
        setRegionOfInterest();
    }
}

void QRectumSegmentationExtension::leftButtonReleaseHandler()
{
    setLeftButtonOff();
    if(m_regionToolButton->isChecked())    //Només en cas que estiguem en la selecció de regió d'interés
    {
        setReleaseRegionOfInterest();
    }
}

void QRectumSegmentationExtension::setSeedPosition(double x, double y, double z)
{
    m_seedPosition[0] = x;
    m_seedPosition[1] = y;
    m_seedPosition[2] = z;

    m_seedXLineEdit->setText(QString::number(m_seedPosition[0], 'f', 1));
    m_seedYLineEdit->setText(QString::number(m_seedPosition[1], 'f', 1));
    m_seedZLineEdit->setText(QString::number(m_seedPosition[2], 'f', 1));
    m_isSeed=true;
    if(m_isMask)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QRectumSegmentationExtension::setRegionOfInterest()
{
    double pos[3];
    m_2DView->getEventWorldCoordinate(pos);
    m_initialRegionPoint[0]= pos[0];
    m_initialRegionPoint[1]= pos[1];
    m_isRegionSetting = true;
}

void QRectumSegmentationExtension::setMovingRegionOfInterest()
{
    if(m_isRegionSetting)
    {
        double pos[3];
        double spacing[3];
        m_mainVolume->getSpacing(spacing);
        m_2DView->getEventWorldCoordinate(pos);
        m_finalRegionPoint[0]= pos[0];
        m_finalRegionPoint[1]= pos[1];

        vtkPoints *points = vtkPoints::New();
        points->SetNumberOfPoints(4);
        points->SetPoint(0, m_initialRegionPoint[0], m_initialRegionPoint[1], m_mainVolume->getOrigin()[2]-1);
        points->SetPoint(1, m_initialRegionPoint[0], m_finalRegionPoint[1], m_mainVolume->getOrigin()[2]-1);
        points->SetPoint(2, m_finalRegionPoint[0], m_finalRegionPoint[1], m_mainVolume->getOrigin()[2]-1);
        points->SetPoint(3, m_finalRegionPoint[0], m_initialRegionPoint[1], m_mainVolume->getOrigin()[2]-1);

        vtkIdType pointIds[4];

        pointIds[0] = 0;
        pointIds[1] = 1;
        pointIds[2] = 2;
        pointIds[3] = 3;

        vtkUnstructuredGrid *grid = vtkUnstructuredGrid::New();

        grid->Allocate(1);
        grid->SetPoints(points);

        grid->InsertNextCell(VTK_QUAD,4,pointIds);

        squareRegionActor->GetProperty()->SetColor(0.45, 0.23, 0.26);
        squareRegionActor->GetProperty()->SetOpacity(0.2);

        vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
        squareMapper->SetInputData(grid);

        squareRegionActor->SetMapper(squareMapper);

        squareRegionActor->VisibilityOn();
        m_2DView->getRenderer()->AddViewProp(squareRegionActor);
        m_2DView->getRenderer()->ResetCameraClippingRange();
        m_2DView->render();

        squareMapper->Delete();
        points->Delete();
        grid->Delete();
    }
}

void QRectumSegmentationExtension::setReleaseRegionOfInterest()
{
    m_isRegionSet=true;
    m_isRegionSetting=false;
    m_viewROICheckBox->setEnabled(true);
    m_viewROICheckBox->setChecked(true);
}

void QRectumSegmentationExtension::viewRegionState(int st)
{
    if(st==Qt::Unchecked)
    {
        squareRegionActor->VisibilityOff();
        m_2DView->render();
    }
    else
    {
        squareRegionActor->VisibilityOn();
        m_2DView->render();
    }
}

void QRectumSegmentationExtension::setLeftButtonOff()
{
    m_isLeftButtonPressed = false;
}

void QRectumSegmentationExtension::setOpacity(int op)
{
    if(m_lesionMaskVolume != 0)
    {
        m_2DView->setOverlayOpacity(((double)op)/100.0);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->render();
    }
}

void QRectumSegmentationExtension::setLowerValue(int x)
{
    if(x>m_upperValueSlider->value())
    {
        m_upperValueSlider->setValue(x);
    }
    this->viewThresholds();
    m_segMethod->setHistogramLowerLevel (x);
    m_isMask=true;
    if(m_isSeed)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

void QRectumSegmentationExtension::setUpperValue(int x)
{
    if(x<m_lowerValueSlider->value())
    {
        m_lowerValueSlider->setValue(x);
    }
    this->viewThresholds();
    m_segMethod->setHistogramUpperLevel (x);
    m_isMask=true;
    if(m_isSeed)
    {
        m_applyMethodButton->setEnabled(true);
    }
}

///Aquesta funció l'usem quan la toole d'edició no sabem si està actualitzada
/// Això passa quan inserim un nou overlay (per això està conectat amb el signal overlayChanged)
void QRectumSegmentationExtension::updateVolumeForced()
{
    if(m_resultsLineEdit->isEnabled())
    {
        int ext[6];
        int i,j,k;
        int cont = 0;
        m_2DView->getOverlayInput()->getExtent(ext);
    
        VolumePixelDataIterator it = m_2DView->getOverlayInput()->getIterator();
        for(i=ext[0];i<=ext[1];i++)
        {
            for(j=ext[2];j<=ext[3];j++)
            {
                for(k=ext[4];k<=ext[5];k++)
                {
                    if (it.get<int>() == m_insideValue)
                    {
                        cont++;
                    }
                    ++it;
                }
            }
        }
        double spacing[3];
        m_lesionMaskVolume->getSpacing(spacing);
        double volume = 1.0;

        for(unsigned int i=0;i<Volume::VDimension;i++)
        {
            volume *= spacing[i];
        }

        m_volume = volume*(double)cont;
        m_resultsLineEdit->clear();
        m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    }
}

void QRectumSegmentationExtension::updateVolume()
{
    if(m_resultsLineEdit->isEnabled())
    {
        m_volume = this->calculateMaskVolume();
        m_resultsLineEdit->clear();
        m_resultsLineEdit->insert(QString("%1").arg(m_volume, 0, 'f', 2));
    }
}

void QRectumSegmentationExtension::viewThresholds()
{
    if(m_lesionMaskVolume == 0)
    {
        m_lesionMaskVolume = new Volume();
    }
    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInputData(m_mainVolume->getVtkData());
    imageThreshold->ThresholdBetween(m_lowerValueSlider->value(),  m_upperValueSlider->value());
    imageThreshold->SetInValue(m_insideValue);
    imageThreshold->SetOutValue(m_outsideValue);
    imageThreshold->Update();

    m_lesionMaskVolume->setData(imageThreshold->GetOutput());

    this->viewLesionOverlay();

//     m_2DView->setOverlapMethod(Q2DViewer::Blend);
//     m_2DView->setOverlayOpacity(((double)m_opacitySlider->value())/100.0);
//     m_2DView->setOverlayInput(m_lesionMaskVolume);
//     m_2DView->render();

}

void QRectumSegmentationExtension::viewLesionOverlay()
{
    if(m_lesionMaskVolume != 0)
    {
        m_2DView->setOverlayOpacity(((double)m_opacitySlider->value())/100.0);
        m_2DView->setOverlapMethod(Q2DViewer::Blend);
        m_2DView->setOverlayInput(m_lesionMaskVolume);
        m_2DView->render();
    }
}

double QRectumSegmentationExtension::calculateMaskVolume()
{
    if (!m_lesionMaskVolume) 
        return 0.0;

    int cont;
    EditorTool* edTool = static_cast<EditorTool*> (m_2DView->getToolProxy()->getTool("EditorTool"));
    if(edTool!=0)
    {
        EditorToolData* edToolData = static_cast<EditorToolData*> (m_2DView->getToolProxy()->getTool("EditorTool")->getToolData());
        if(edToolData!=0)
        {
            cont = edToolData->getVolumeVoxels();
        }
        else
        {
            DEBUG_LOG("No existeix la editor tool");
            return 0.0;
        }
    }
    else
    {
        DEBUG_LOG("No existeix la editor tool");
        return 0.0;
    }

    double spacing[3];
    m_lesionMaskVolume->getSpacing(spacing);
    double volume = 1.0;

    for(unsigned int i=0;i<Volume::VDimension;i++)
    {
        volume *= spacing[i];
    }

    m_volume = volume*(double)cont;

    return m_volume;
}

void QRectumSegmentationExtension::saveActivedMaskVolume()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
    if (!fileName.isEmpty())
    {
        if(QFileInfo(fileName).suffix() != "mhd")
        {
            fileName += ".mhd";
        }
        //Forcem que la màscara que gaurdem el dins sigui 255 i el fora 0
        vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
        imageThreshold->SetInputData(m_lesionMaskVolume->getVtkData());
        imageThreshold->ThresholdBetween(m_insideValue , m_insideValue); // només els que valen m_insideValue
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

void QRectumSegmentationExtension::saveSegmentation3DVolume()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Volume file"), m_savingMaskDirectory, tr("MetaImage Files (*.mhd)"));
    if (!fileName.isEmpty())
    {
        if(QFileInfo(fileName).suffix() != "mhd")
        {
            fileName += ".mhd";
        }
        itk::Image<unsigned char, 3>::Pointer outputImage = itk::Image<unsigned char, 3>::New();
        outputImage->SetRegions(m_mainVolume->getItkData()->GetBufferedRegion());
        outputImage->SetSpacing(m_mainVolume->getItkData()->GetSpacing());
        outputImage->SetOrigin(m_mainVolume->getItkData()->GetOrigin());
        outputImage->Allocate();

        typedef itk::ImageRegionIterator<Volume::ItkImageType> ItkIterator;
        ItkIterator iter(m_mainVolume->getItkData(), m_mainVolume->getItkData()->GetBufferedRegion());
        ItkIterator itLesion(m_lesionMaskVolume->getItkData(), m_lesionMaskVolume->getItkData()->GetBufferedRegion());
        typedef itk::ImageRegionIterator< itk::Image<unsigned char, 3> > OutIterator;
        OutIterator itOut(outputImage, outputImage->GetBufferedRegion());

        int seedSl=(int) (m_seedPosition[2]-m_mainVolume->getOrigin()[2])/m_mainVolume->getSpacing()[2];

        int i,j,k;
        for(k=0;k<m_mainVolume->getDimensions()[2];k++)
        {
            for(j=0;j<m_mainVolume->getDimensions()[1];j++)
            {
                for(i=0;i<m_mainVolume->getDimensions()[0];i++)
                {
                    if(k<seedSl)
                    {
                        if(itLesion.Get()!=m_insideValue)
                        {
                            itOut.Set(0);
                        }else{
                            itOut.Set(255);
                        }
                    }else{
                        if(itLesion.Get()!=m_insideValue)
                        {
                            //Ho reescalem de 1 fins a 254
                            itOut.Set((((iter.Get()-m_minValue)*253)/m_maxValue)+1);
                        }else{
                            itOut.Set(255);
                        }
                    }
                    ++itOut;
                    ++iter;
                    ++itLesion;
                }
            }
        }

        typedef itk::ImageFileWriter< itk::Image<unsigned char, 3> > ExternalWriterType;

        ExternalWriterType::Pointer mapWriter = ExternalWriterType::New();
        mapWriter->SetInput(outputImage);
        mapWriter->SetFileName(qPrintable(fileName));
        mapWriter->Update();

        m_savingMaskDirectory = QFileInfo(fileName).absolutePath();
    }
}

void QRectumSegmentationExtension::toolChanged(int but)
{
    if(but == m_toolsButtonGroup->id(m_regionToolButton))
    {
        m_toolManager->disableAllToolsTemporarily();
    }
    else
    {
        m_toolManager->undoDisableAllToolsTemporarily();
        m_regionToolButton->setChecked(false);        
    }
}

void QRectumSegmentationExtension::readSettings()
{
    Settings settings;

    settings.restoreGeometry(RectumSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    settings.restoreGeometry(RectumSegmentationSettings::VerticalSplitterGeometry, m_verticalSplitter);
    m_savingMaskDirectory = settings.getValue(RectumSegmentationSettings::SavedMaskPath).toString();
}

void QRectumSegmentationExtension::writeSettings()
{
    Settings settings;

    settings.saveGeometry(RectumSegmentationSettings::HorizontalSplitterGeometry, m_horizontalSplitter);
    settings.saveGeometry(RectumSegmentationSettings::VerticalSplitterGeometry, m_verticalSplitter);
    settings.setValue(RectumSegmentationSettings::SavedMaskPath, m_savingMaskDirectory);
}

}
