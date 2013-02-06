#include "imagefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "dicomsequenceattribute.h"
#include "dicomsequenceitem.h"
#include "dicomvalueattribute.h"
#include "thumbnailcreator.h"
#include "patientorientation.h"
#include "displayshutter.h"
// Pel fabs. Necessari per Mac
#include <cmath>
#include <QFileInfo>

namespace udg {

ImageFillerStep::ImageFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

ImageFillerStep::~ImageFillerStep()
{
}

bool ImageFillerStep::fillIndividually()
{
    Q_ASSERT(m_input);

    bool ok = false;

    DICOMTagReader *dicomReader = m_input->getDICOMFile();

    if (dicomReader)
    {
        ok = true;
        QList<Image*> generatedImages = processDICOMFile(dicomReader);
        if (!generatedImages.isEmpty())
        {
            m_input->setCurrentImages(generatedImages);
            m_input->addLabelToSeries("ImageFillerStep", m_input->getCurrentSeries());
        }
    }

    return ok;
}

QList<Image*> ImageFillerStep::processDICOMFile(DICOMTagReader *dicomReader)
{
    Q_ASSERT(dicomReader);

    QList<Image*> generatedImages;
    bool ok = dicomReader->tagExists(DICOMPixelData);
    if (ok)
    {
        // Comprovem si la imatge és enhanced o no per tal de cridar el mètode específic més adient
        if (isEnhancedImageSOPClass(dicomReader->getValueAttributeAsQString(DICOMSOPClassUID)))
        {
            generatedImages = processEnhancedDICOMFile(dicomReader);
        }
        else
        {
            int numberOfFrames = 1;
            int volumeNumber = m_input->getCurrentSingleFrameVolumeNumber();
            if (dicomReader->tagExists(DICOMNumberOfFrames))
            {
                numberOfFrames = dicomReader->getValueAttributeAsQString(DICOMNumberOfFrames).toInt();
                // Si és la segona imatge multiframe que ens trobem, augmentarem el número que identifica l'actual volum
                if (m_input->getCurrentSeries()->getImages().count() > 1)
                {
                    m_input->increaseCurrentMultiframeVolumeNumber();
                }
                volumeNumber = m_input->getCurrentMultiframeVolumeNumber();
            }

            for (int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++)
            {
                Image *image = new Image();
                if (processImage(image, dicomReader))
                {
                    // Li assignem el nº de frame i el nº de volum al que pertany
                    image->setFrameNumber(frameNumber);

                    // Comprovem si les imatges són de diferents mides per assignar-lis volums diferents
                    // Això només passarà quan les imatges siguin single-frame
                    if (numberOfFrames == 1)
                    {
                        if (!m_input->getCurrentSeries()->getImages().isEmpty())
                        {
                            // Si la imatge anterior i l'actual tenen mides diferents, aniran en un volum diferent
                            Image *lastProcessedImage = m_input->getCurrentSeries()->getImages().last();
                            if (areOfDifferentSize(lastProcessedImage, image) || areOfDifferentPhotometricInterpretation(lastProcessedImage, image) 
                                || areOfDifferentPixelSpacing(lastProcessedImage, image))
                            {
                                m_input->increaseCurrentSingleFrameVolumeNumber();
                                volumeNumber = m_input->getCurrentSingleFrameVolumeNumber();
                                // Actualitzem el número actual de volum i guardem el corresponent thumbnail
                                m_input->setCurrentVolumeNumber(volumeNumber);
                                // HACK Si és la segona imatge de mida diferent, cal generar el propi thumbnail de la imatge anterior
                                if (volumeNumber == 101)
                                {
                                    QString path = QString("%1/thumbnail%2.png").arg(QFileInfo(lastProcessedImage->getPath()).absolutePath()).arg(
                                                           lastProcessedImage->getVolumeNumberInSeries());
                                    lastProcessedImage->getThumbnail().save(path, "PNG");
                                }
                                saveThumbnail(dicomReader);
                            }
                        }
                    }
                    image->setVolumeNumberInSeries(volumeNumber);

                    // Afegirem la imatge a la llista si aquesta s'ha pogut afegir a la corresponent sèrie
                    if (m_input->getCurrentSeries()->addImage(image))
                    {
                        generatedImages << image;
                    }
                }
                else
                {
                    delete image;
                }
            }
            m_input->setCurrentVolumeNumber(volumeNumber);
        }

        if (generatedImages.count() > 1)
        {
            // Com que la imatge és multiframe (tant si és enhanced com si no) creem els corresponents thumbnails i els guardem a la cache
            saveThumbnail(dicomReader);
        }
    }
    return generatedImages;
}

void ImageFillerStep::saveThumbnail(DICOMTagReader *dicomReader)
{
    Q_ASSERT(dicomReader);

    int volumeNumber = m_input->getCurrentVolumeNumber();
    QString thumbnailPath = QFileInfo(dicomReader->getFileName()).absolutePath();

    ThumbnailCreator thumbnailCreator;
    QImage thumbnail = thumbnailCreator.getThumbnail(dicomReader);
    thumbnail.save(QString("%1/thumbnail%2.png").arg(thumbnailPath).arg(volumeNumber), "PNG");

    // Si és el primer thumbnail, també creem el thumbnail ordinari que s'havia fet sempre
    if (volumeNumber == 1)
    {
        thumbnail.save(QString("%1/thumbnail.png").arg(thumbnailPath), "PNG");
    }
}

bool ImageFillerStep::fillCommonImageInformation(Image *image, DICOMTagReader *dicomReader)
{
    Q_ASSERT(image);
    Q_ASSERT(dicomReader);

    // El path on es troba la imatge a disc
    image->setPath(dicomReader->getFileName());

    // DICOMSource del que prové la imatge
    image->setDICOMSource(m_input->getDICOMSource());

    // C.12.1 SOP Common Module
    image->setSOPInstanceUID(dicomReader->getValueAttributeAsQString(DICOMSOPInstanceUID));
    image->setInstanceNumber(dicomReader->getValueAttributeAsQString(DICOMInstanceNumber));

    // C.7.6.3 Image Pixel Module
    image->setSamplesPerPixel(dicomReader->getValueAttributeAsQString(DICOMSamplesPerPixel).toInt());
    image->setPhotometricInterpretation(dicomReader->getValueAttributeAsQString(DICOMPhotometricInterpretation));
    image->setRows(dicomReader->getValueAttributeAsQString(DICOMRows).toInt());
    image->setColumns(dicomReader->getValueAttributeAsQString(DICOMColumns).toInt());
    image->setBitsAllocated(dicomReader->getValueAttributeAsQString(DICOMBitsAllocated).toInt());
    image->setBitsStored(dicomReader->getValueAttributeAsQString(DICOMBitsStored).toInt());
    image->setPixelRepresentation(dicomReader->getValueAttributeAsQString(DICOMPixelRepresentation).toInt());

    // C.7.6.1 General Image Module (present a totes les modalitats no enhanced, excepte 3D XA, 3D CF i OPT)
    // C.8.13.1 Enhanced MR Image Module
    // C.8.15.2 Enhanced CT Image Module
    // C.8.19.2 Enhanced XA/XRF Image Module
    image->setImageType(dicomReader->getValueAttributeAsQString(DICOMImageType));

    // Obtenim l'hora en que es va crear/obtenir la píxel data
    // C.7.6.1 General Image Module (present a totes les modalitats no enhanced, excepte 3D XA, 3D CF i OPT)
    // C.7.6.16 Multi-Frame Functional Groups Module
    image->setImageTime(dicomReader->getValueAttributeAsQString(DICOMContentTime));

    // En el cas d'XA/XRF el pixel spacing vindrà especificat per totes les imatges per igual (no cal fer un recorregut per-frame)
    QString sopClassUID = m_input->getDICOMFile()->getValueAttributeAsQString(DICOMSOPClassUID);
    if (sopClassUID == UIDEnhancedXAImageStorage || sopClassUID == UIDEnhancedXRFImageStorage)
    {
        //
        // XA/XRF Acquisition Module - C.8.19.3
        // És requerit si el primer valor d'ImageType == ORIGINAL
        // Pot estar present encara que no es compleixi la condició anterior.
        //

        //
        // Obtenim el Pixel Spacing (Imager Pixel Spacing (1))
        //
        if (dicomReader->tagExists(DICOMImagerPixelSpacing))
        {
            QString spacing = dicomReader->getValueAttributeAsQString(DICOMImagerPixelSpacing);
            if (!spacing.isEmpty())
            {
                QStringList list;
                list = spacing.split("\\");
                if (list.size() == 2)
                {
                    image->setPixelSpacing(list.at(0).toDouble(), list.at(1).toDouble());
                }
                else
                {
                    DEBUG_LOG("No s'ha trobat cap valor de pixel spacing definit de forma estàndar esperada.");
                }
            }
        }
    }

    // C.9 Overlays
    image->setNumberOfOverlays(getNumberOfOverlays(dicomReader));

    return true;
}

bool ImageFillerStep::processImage(Image *image, DICOMTagReader *dicomReader)
{
    Q_ASSERT(image);
    Q_ASSERT(dicomReader);

    // Comprovem si l'arxiu és una imatge, per això caldrà que existeixi el tag PixelData->TODO es podria eliminar perquè ja ho comprovem abans! Falta fer la
    // comprovació quan llegim fitxer a fitxer
    bool ok = dicomReader->tagExists(DICOMPixelData);
    if (ok)
    {
        QString value;

        // Omplim la informació comuna
        fillCommonImageInformation(image, dicomReader);

        // Calculem el pixel spacing
        computePixelSpacing(image, dicomReader);

        //
        // Calculem propietats del pla imatge
        //

        //
        // Propietats d'Image Plane Module (C.7.6.2) (Requerit per CT,MR i PET)
        //

        //
        // Obtenim Slice Thickness, tipus 2
        //
        value = dicomReader->getValueAttributeAsQString(DICOMSliceThickness);
        if (!value.isEmpty())
        {
            image->setSliceThickness(value.toDouble());
        }

        //
        // Obtenim Slice Location, tipus 3
        //
        if (dicomReader->tagExists(DICOMSliceLocation))
        {
            image->setSliceLocation(dicomReader->getValueAttributeAsQString(DICOMSliceLocation));
        }

        //
        // Obtenim Image Position (Patient), tipus 1
        //
        value = dicomReader->getValueAttributeAsQString(DICOMImagePositionPatient);
        if (!value.isEmpty())
        {
            QStringList list = value.split("\\");
            if (list.size() == 3)
            {
                double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
                image->setImagePositionPatient(position);
            }
        }

        //
        // Obtenim Image Orientation (Patient), tipus 1
        //
        if (dicomReader->tagExists(DICOMImageOrientationPatient))
        {
            value = dicomReader->getValueAttributeAsQString(DICOMImageOrientationPatient);
            ImageOrientation imageOrientation;
            imageOrientation.setDICOMFormattedImageOrientation(value);
            image->setImageOrientationPatient(imageOrientation);

            // Orientació de pacient
            // L'obtenim a partir del tag DICOM si existeix, sinó el calculem a partir d'ImageOrientation
            PatientOrientation patientOrientation;
            value = dicomReader->getValueAttributeAsQString(DICOMPatientOrientation);
            if (!value.isEmpty())
            {
                patientOrientation.setDICOMFormattedPatientOrientation(value);
            }
            else
            {
                patientOrientation.setPatientOrientationFromImageOrientation(image->getImageOrientationPatient());
            }
            image->setPatientOrientation(patientOrientation);
        }
        else
        {
            //
            // General Image Module (C.7.6.1)
            // Requerit a pràcticament totes les modalitats no-enhanced, conté el tag Patient Orientation
            //

            // Com que no tenim ImageOrientationPatient no podem generar la informació de Patient Orientation
            // Per tant, anem a buscar el valor del tag PatientOrientation, de tipus 2C
            value = dicomReader->getValueAttributeAsQString(DICOMPatientOrientation);
            
            PatientOrientation patientOrientation;
            patientOrientation.setDICOMFormattedPatientOrientation(value);
            image->setPatientOrientation(patientOrientation);
        }

        //
        // Obtenim dades del Grayscale Pipeline
        //

        // Obtenim Rescale Slope i Rescale Intercept, tipus 1/1C segons el mòdul
        value = dicomReader->getValueAttributeAsQString(DICOMRescaleSlope);
        if (value.toDouble() == 0)
        {
            image->setRescaleSlope(1.);
        }
        else
        {
            image->setRescaleSlope(value.toDouble());
        }

        image->setRescaleIntercept(dicomReader->getValueAttributeAsQString(DICOMRescaleIntercept).toDouble());

        //
        // Llegim els valors de window level, tipus 1C
        // Aquests es troben a VOI LUT Module (C.11.2)
        // El mòdul és opcional a CR, CT, MR, NM, US, US MF, SC, XA, RF, RF IM i PET
        // i és condicional a SC MF GB, SC MF GW, DX, MG i IO
        //
        QStringList windowWidthList = dicomReader->getValueAttributeAsQString(DICOMWindowWidth).split("\\");
        QStringList windowLevelList = dicomReader->getValueAttributeAsQString(DICOMWindowCenter).split("\\");
        for (int i = 0; i < windowWidthList.size(); i++)
        {
            image->addWindowLevel(windowWidthList.at(i).toDouble(), windowLevelList.at(i).toDouble());
        }

        // Llegim les respectives descripcions de ww/wl si n'hi ha (tipus 3)
        image->setWindowLevelExplanations(dicomReader->getValueAttributeAsQString(DICOMWindowCenterWidthExplanation).split("\\"));

        // Propietats útils pels hanging protocols
        value = dicomReader->getValueAttributeAsQString(DICOMImageLaterality);
        if (!value.isEmpty())
        {
            image->setImageLaterality(value.at(0));
        }
        // De moment només ho aprofitarem per mammografia, però pot ser vàlid per altres modalitats
        DICOMSequenceAttribute *viewCodeSequence = dicomReader->getSequenceAttribute(DICOMViewCodeSequence);
        if (viewCodeSequence)
        {
            QList<DICOMSequenceItem*> items = viewCodeSequence->getItems();
            // Per definició, només hauríem de tenir un ítem
            switch (items.count())
            {
                case 0:
                    DEBUG_LOG("ViewCodeSequence no té cap ítem o no existeix");
                    break;

                case 1:
                    image->setViewCodeMeaning(items.at(0)->getValueAttribute(DICOMCodeMeaning)->getValueAsQString());
                    break;

                default:
                    DEBUG_LOG("ViewCodeSequence té més d'un ítem!");
                    break;
            }
        }

        // Només pel cas que sigui DX tindrem aquest atribut a nivell d'imatge
        image->setViewPosition(dicomReader->getValueAttributeAsQString(DICOMViewPosition));

        // Display Shutter Module (C.7.6.11)
        // Omple la informació referent als Display Shutters que podem trobar en imatges de modalitat CR, XA, RF, DX, MG i IO
        fillDisplayShutterInformation(image, dicomReader);
    }
    else
    {
        DEBUG_LOG("L'arxiu [" + image->getPath() + "] no conté el tag PixelData");
    }

    return ok;
}

QList<Image*> ImageFillerStep::processEnhancedDICOMFile(DICOMTagReader *dicomReader)
{
    Q_ASSERT(dicomReader);

    QList<Image*> generatedImages;
    // Comprovem si l'arxiu és una imatge, per això caldrà que existeixi el tag PixelData->TODO es podria eliminar perquè ja ho comprovem abans! Falta fer la
    // comprovació quan llegim fitxer a fitxer
    if (dicomReader->tagExists(DICOMPixelData))
    {
        int numberOfFrames = dicomReader->getValueAttributeAsQString(DICOMNumberOfFrames).toInt();
        // Si és la segona imatge enhanced que ens trobem, augmentarem el número que identifica l'actual volum
        if (m_input->getCurrentSeries()->getImages().count() > 1)
        {
            m_input->increaseCurrentMultiframeVolumeNumber();
        }
        m_input->setCurrentVolumeNumber(m_input->getCurrentMultiframeVolumeNumber());

        for (int frameNumber = 0; frameNumber < numberOfFrames; frameNumber++)
        {
            Image *image = new Image();
            fillCommonImageInformation(image, dicomReader);
            // Li assignem el nº de frame i el nº de volum al que pertany
            image->setFrameNumber(frameNumber);
            image->setVolumeNumberInSeries(m_input->getCurrentVolumeNumber());

            // Afegirem la imatge a la llista si aquesta s'ha pogut afegir a la corresponent sèrie
            if (m_input->getCurrentSeries()->addImage(image))
            {
                generatedImages << image;
            }
        }

        // Tractem la Shared Functional Groups Sequence
        DICOMSequenceAttribute *sharedFunctionalGroupsSequence = dicomReader->getSequenceAttribute(DICOMSharedFunctionalGroupsSequence);
        if (sharedFunctionalGroupsSequence)
        {
            // Aquesta seqüència pot contenir un ítem o cap
            QList<DICOMSequenceItem*> sharedItems = sharedFunctionalGroupsSequence->getItems();
            if (!sharedItems.isEmpty())
            {
                foreach (Image *image, generatedImages)
                {
                    fillFunctionalGroupsInformation(image, sharedItems.first());
                }
            }
        }
        else
        {
            DEBUG_LOG("No hem trobat la Shared Functional Groups Sequence en un arxiu DICOM que es presuposa Enhanced");
            ERROR_LOG("No hem trobat la Shared Functional Groups Sequence en un arxiu DICOM que es presuposa Enhanced");
        }
        // Tractem la Per-Frame Functional Groups Sequence
        DICOMSequenceAttribute *perFrameFunctionalGroupsSequence = dicomReader->getSequenceAttribute(DICOMPerFrameFunctionalGroupsSequence);
        if (perFrameFunctionalGroupsSequence)
        {
            QList<DICOMSequenceItem*> perFrameItems = perFrameFunctionalGroupsSequence->getItems();
            int frameNumber = 0;
            foreach (DICOMSequenceItem *item, perFrameItems)
            {
                fillFunctionalGroupsInformation(generatedImages.at(frameNumber), item);
                frameNumber++;
            }
        }
        else
        {
            DEBUG_LOG("No hem trobat la per-frame Functional Groups Sequence en un arxiu DICOM que es presuposa Enhanced");
            ERROR_LOG("No hem trobat la per-frame Functional Groups Sequence en un arxiu DICOM que es presuposa Enhanced");
        }
    }
    else
    {
        DEBUG_LOG("L'arxiu [" + dicomReader->getFileName() + "] no conté el tag PixelData");
    }

    return generatedImages;
}

void ImageFillerStep::fillFunctionalGroupsInformation(Image *image, DICOMSequenceItem *frameItem)
{
    Q_ASSERT(image);
    Q_ASSERT(frameItem);

    // Hi ha alguns atributs que els haurem de buscar en llocs diferents segons la modalitat
    QString sopClassUID = m_input->getDICOMFile()->getValueAttributeAsQString(DICOMSOPClassUID);

    //
    // Atributs de CT i MR
    //
    if (sopClassUID == UIDEnhancedCTImageStorage || sopClassUID == UIDEnhancedMRImageStorage)
    {
        //
        // Per obtenir el Frame Type, haurem de seleccionar la seqüència adient, segons la modalitat
        //
        DICOMSequenceAttribute *imageFrameTypeSequence = 0;
        if (sopClassUID == UIDEnhancedCTImageStorage)
        {
            //
            // CT Image Frame Type (C.8.15.3.1)
            //
            imageFrameTypeSequence = frameItem->getSequenceAttribute(DICOMCTImageFrameTypeSequence);
        }
        else
        {
            // MR Image Frame Type (C.8.13.5.1)
            imageFrameTypeSequence = frameItem->getSequenceAttribute(DICOMMRImageFrameTypeSequence);
        }

        // Un cop seleccionada la seqüència adient, obtenim els valors
        if (imageFrameTypeSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> imageFrameTypeItems = imageFrameTypeSequence->getItems();
            if (!imageFrameTypeItems.empty())
            {
                DICOMSequenceItem *item = imageFrameTypeItems.at(0);
                //
                // Obtenim el Frame Type (1)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMFrameType);
                if (dicomValue)
                {
                    image->setImageType(dicomValue->getValueAsQString());
                }
                else
                {
                    DEBUG_LOG("Falta el tag FrameType que hauria d'estar present!");
                    ERROR_LOG("Falta el tag FrameType que hauria d'estar present!");
                }
            }
        }

        //
        // Pixel Measures Module - C.7.6.16.2.1
        //
        DICOMSequenceAttribute *pixelMeasuresSequence = frameItem->getSequenceAttribute(DICOMPixelMeasuresSequence);
        if (pixelMeasuresSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> pixelMeasuresItems = pixelMeasuresSequence->getItems();
            if (!pixelMeasuresItems.empty())
            {
                DICOMSequenceItem *item = pixelMeasuresItems.at(0);
                //
                // Obtenim el Pixel Spacing (1C)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMPixelSpacing);
                if (dicomValue)
                {
                    QString spacing = dicomValue->getValueAsQString();
                    if (!spacing.isEmpty())
                    {
                        QStringList list = spacing.split("\\");
                        if (list.size() == 2)
                        {
                            image->setPixelSpacing(list.at(0).toDouble(), list.at(1).toDouble());
                        }
                        else
                        {
                            DEBUG_LOG("No s'ha trobat cap valor de pixel spacing definit de forma estàndar esperada");
                        }
                    }
                }

                //
                // Obtenim l'Slice Thickness (1C)
                //
                dicomValue = item->getValueAttribute(DICOMSliceThickness);
                if (dicomValue)
                {
                    image->setSliceThickness(dicomValue->getValueAsDouble());
                }
            }
        }

        //
        // Plane Orientation Module - C.7.6.16.2.4
        //
        DICOMSequenceAttribute *planeOrientationSequence = frameItem->getSequenceAttribute(DICOMPlaneOrientationSequence);
        if (planeOrientationSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> planeOrientationItems = planeOrientationSequence->getItems();
            if (!planeOrientationItems.empty())
            {
                DICOMSequenceItem *item = planeOrientationItems.at(0);
                //
                // Obtenim Image Orientation (Patient) (1C) + assignació del "Patient Orientation"
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMImageOrientationPatient);
                if (dicomValue)
                {
                    ImageOrientation imageOrientation;
                    imageOrientation.setDICOMFormattedImageOrientation(dicomValue->getValueAsQString());
                    image->setImageOrientationPatient(imageOrientation);
                    // Li passem l'ImageOrientation obtingut per crear les etiquetes d'orientació
                    PatientOrientation patientOrientation;
                    patientOrientation.setPatientOrientationFromImageOrientation(image->getImageOrientationPatient());
                    image->setPatientOrientation(patientOrientation);
                }
            }
        }

        //
        // Plane Position Module - C.7.6.16.2.3
        //
        DICOMSequenceAttribute *planePositionSequence = frameItem->getSequenceAttribute(DICOMPlanePositionSequence);
        if (planePositionSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> planePositionItems = planePositionSequence->getItems();
            if (!planePositionItems.empty())
            {
                DICOMSequenceItem *item = planePositionItems.at(0);
                //
                // Obtenim Image Position (Patient) (1C)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMImagePositionPatient);
                if (dicomValue)
                {
                    QString imagePositionPatientString = dicomValue->getValueAsQString();
                    if (!imagePositionPatientString.isEmpty())
                    {
                        QStringList list = imagePositionPatientString.split("\\");
                        if (list.size() == 3)
                        {
                            double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
                            image->setImagePositionPatient(position);
                        }
                    }
                    else
                    {
                        DEBUG_LOG("El valor està buit quan hauria de contenir algun valor!");
                    }
                }
            }
        }

        //
        // CT Pixel Value Transformation Module - C.8.15.3.10 - Enhanced CT
        // Pixel Value Transformation Module - C.7.6.16.2.9 - Enhanced MR
        //
        // Contenen la mateixa informació. El primer és simplement l'especialització pels CT
        //
        DICOMSequenceAttribute *pixelValueTransformationSequence = frameItem->getSequenceAttribute(DICOMPixelValueTransformationSequence);
        if (pixelValueTransformationSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> pixelValueTransformationItems = pixelValueTransformationSequence->getItems();
            if (!pixelValueTransformationItems.empty())
            {
                DICOMSequenceItem *item = pixelValueTransformationItems.at(0);
                //
                // Obtenim Rescale Intercept (1)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMRescaleIntercept);
                if (dicomValue)
                {
                    image->setRescaleIntercept(dicomValue->getValueAsDouble());
                }
                else
                {
                    DEBUG_LOG("Falta el tag RescaleIntercept que hauria d'estar present!");
                    ERROR_LOG("Falta el tag RescaleIntercept que hauria d'estar present!");
                }
                //
                // Obtenim Rescale Slope (1)
                //
                dicomValue = item->getValueAttribute(DICOMRescaleSlope);
                if (dicomValue)
                {
                    image->setRescaleSlope(dicomValue->getValueAsDouble());
                }
                else
                {
                    DEBUG_LOG("Falta el tag RescaleSlope que hauria d'estar present!");
                    ERROR_LOG("Falta el tag RescaleSlope que hauria d'estar present!");
                }
            }
        }
    }
    //
    // Atributs d'XA i XRF
    //
    else if (sopClassUID == UIDEnhancedXAImageStorage || sopClassUID == UIDEnhancedXRFImageStorage)
    {
        //
        // X-Ray Object Thickness Macro - C.8.19.6.7
        //
        DICOMSequenceAttribute *objectThicknessSequence = frameItem->getSequenceAttribute(DICOMObjectThicknessSequence);
        if (objectThicknessSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> objectThicknessItems = objectThicknessSequence->getItems();
            if (!objectThicknessItems.empty())
            {
                DICOMSequenceItem *item = objectThicknessItems.at(0);
                //
                // Obtenim Calculated Anatomy Thickness (1)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMCalculatedAnatomyThickness);
                if (dicomValue)
                {
                    image->setSliceThickness(dicomValue->getValueAsDouble());
                }
                else
                {
                    DEBUG_LOG("No s'ha trobat el tag Calculated Anatomy Thickness en una seqüència que se suposa que l'ha de tenir!");
                    ERROR_LOG("No s'ha trobat el tag Calculated Anatomy Thickness en una seqüència que se suposa que l'ha de tenir!");
                }
            }
        }

        //
        // Patient Orientation in Frame Macro - C.7.6.16.2.15
        // Requerit si C-arm Positioner Tabletop Relationship està present i és igual a YES
        // Podria estar present tot i que no es compleixi l'anterior condició
        //
        DICOMSequenceAttribute *patientOrientationInFrameSequence = frameItem->getSequenceAttribute(DICOMPatientOrientationInFrameSequence);
        if (patientOrientationInFrameSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> patientOrientationInFrameItems = patientOrientationInFrameSequence->getItems();
            if (!patientOrientationInFrameItems.empty())
            {
                DICOMSequenceItem *item = patientOrientationInFrameItems.at(0);
                //
                // Obtenim Patient Orientation (1)
                //
                DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMPatientOrientation);
                if (dicomValue)
                {
                    PatientOrientation patientOrientation;
                    patientOrientation.setDICOMFormattedPatientOrientation(dicomValue->getValueAsQString());
                    image->setPatientOrientation(patientOrientation);
                }
                else
                {
                    DEBUG_LOG("No s'ha trobat el tag Patient Orientation en una seqüència que se suposa que l'ha de tenir!");
                    ERROR_LOG("No s'ha trobat el tag Patient Orientation en una seqüència que se suposa que l'ha de tenir!");
                }
            }
        }
    }

    //
    // Frame Display Shutter Macro (C.7.6.16.2.16)
    // Es pot trobar a les modalitats Enhanced: XA, XRF i US Volume
    //
    if (sopClassUID == UIDEnhancedXAImageStorage || sopClassUID == UIDEnhancedXRFImageStorage || sopClassUID == UIDEnhancedUSVolumeStorage)
    {
        DICOMSequenceAttribute *frameDisplayShutterSequence = frameItem->getSequenceAttribute(DICOMFrameDisplayShutterSequence);
        if (frameDisplayShutterSequence)
        {
            // Segons DICOM només es permet que contingui un sol ítem
            QList<DICOMSequenceItem*> frameDisplayShutterItems = frameDisplayShutterSequence->getItems();
            if (!frameDisplayShutterItems.empty())
            {
                DICOMSequenceItem *item = frameDisplayShutterItems.at(0);
                fillDisplayShutterInformation(image, item);
            }
        }
    }
    
    //
    // A continuació llegim els tags/mòduls que es troben a totes les modalitats enhanced (MR/CT/XA/XRF)
    //

    //
    // Frame VOI LUT Macro (C.7.6.16.2.10)
    //
    DICOMSequenceAttribute *frameVOILUTSequence = frameItem->getSequenceAttribute(DICOMFrameVOILUTSequence);
    if (frameVOILUTSequence)
    {
        // Segons DICOM només es permet que contingui un sol ítem
        QList<DICOMSequenceItem*> frameVOILUTItems = frameVOILUTSequence->getItems();
        if (!frameVOILUTItems.empty())
        {
            DICOMSequenceItem *item = frameVOILUTItems.at(0);
            //
            // Obtenim Window Center (1)
            //
            DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMWindowCenter);
            QStringList windowLevelList;
            if (dicomValue)
            {
                windowLevelList = dicomValue->getValueAsQString().split("\\");
            }
            else
            {
                DEBUG_LOG("No s'ha trobat el tag Window Center en un arxiu que se suposa que l'ha de tenir!");
                ERROR_LOG("No s'ha trobat el tag Window Center en un arxiu que se suposa que l'ha de tenir!");
            }

            //
            // Obtenim Window Width (1)
            //
            dicomValue = item->getValueAttribute(DICOMWindowWidth);
            QStringList windowWidthList;
            if (dicomValue)
            {
                windowWidthList = dicomValue->getValueAsQString().split("\\");
            }
            else
            {
                DEBUG_LOG("No s'ha trobat el tag Window Width en un arxiu que se suposa que l'ha de tenir!");
                ERROR_LOG("No s'ha trobat el tag Window Width en un arxiu que se suposa que l'ha de tenir!");
            }

            // Per evitar que hi hagi problemes si les llistes no tenen la mateixa llargada tenim en compte la més curta
            int maxNumberOfElements = windowWidthList.size() < windowLevelList.size()? windowWidthList.size(): windowLevelList.size();
            for (int i = 0; i < maxNumberOfElements; i++)
            {
                image->addWindowLevel(windowWidthList.at(i).toDouble(), windowLevelList.at(i).toDouble());
            }

            //
            // Obtenim Window Explanations (3)
            //
            dicomValue = item->getValueAttribute(DICOMWindowCenterWidthExplanation);
            if (dicomValue)
            {
                image->setWindowLevelExplanations(dicomValue->getValueAsQString().split("\\"));
            }
        }
    }

    //
    // Atributs que fem servir pels hanging protocols
    //

    //
    // Frame Anatomy Module (C.7.6.16.2.8)
    //
    DICOMSequenceAttribute *frameAnatomySequence = frameItem->getSequenceAttribute(DICOMFrameAnatomySequence);
    if (frameAnatomySequence)
    {
        // Segons DICOM només es permet que contingui un sol ítem
        QList<DICOMSequenceItem*> frameAnatomyItems = frameAnatomySequence->getItems();
        if (!frameAnatomyItems.empty())
        {
            DICOMSequenceItem *item = frameAnatomyItems.at(0);
            //
            // Obtenim Frame Laterality (1)
            //
            DICOMValueAttribute *dicomValue = item->getValueAttribute(DICOMFrameLaterality);
            if (dicomValue)
            {
                image->setImageLaterality(dicomValue->getValueAsQString().at(0));
            }
            else
            {
                DEBUG_LOG("No s'ha trobat el tag Frame Laterality en una seqüència que se suposa que l'ha de tenir!");
                ERROR_LOG("No s'ha trobat el tag Frame Laterality en una seqüència que se suposa que l'ha de tenir!");
            }
        }
    }
}

unsigned short ImageFillerStep::getNumberOfOverlays(DICOMTagReader *dicomReader)
{
    Q_ASSERT(dicomReader);

    DICOMTag overlayTag(DICOMOverlayRows);

    unsigned short numberOfOverlays = 0;

    bool stop = false;
    while (!stop)
    {
        if (dicomReader->tagExists(overlayTag))
        {
            ++numberOfOverlays;
            overlayTag.setGroup(overlayTag.getGroup() + 2);
        }
        else
        {
            stop = true;
        }
    }

    return numberOfOverlays;
}

void ImageFillerStep::fillDisplayShutterInformation(Image *image, DICOMSequenceItem *displayShutterItems)
{
    Q_ASSERT(image);
    Q_ASSERT(displayShutterItems);

    //
    // Obtenim Shutter Shape (1)
    //
    DICOMValueAttribute *dicomValue = displayShutterItems->getValueAttribute(DICOMShutterShape);
    QString shutterShape;
    if (dicomValue)
    {
        shutterShape = dicomValue->getValueAsQString();
    }
    else
    {
        // Si no hi ha el valor shutter shape llavors és que no tenim shutters, no cal mirar cap tag més
        return;
    }

    //
    // Obtenim el valor de presentació (3)
    //
    unsigned short presentationValue = 0;
    dicomValue = displayShutterItems->getValueAttribute(DICOMShutterPresentationValue);
    if (dicomValue)
    {
        presentationValue = dicomValue->getValueAsQString().toUShort();
    }
    
    // Segons els valors de shutterShape, omplirem la informació de cada forma del shutter
    if (shutterShape.contains("RECTANGULAR"))
    {
        int minX, maxX, minY, maxY;
        // Obtenim Shutter Left Vertical Edge, requerit quan shutter shape conté RECTANGULAR
        dicomValue = displayShutterItems->getValueAttribute(DICOMShutterLeftVerticalEdge);
        if (dicomValue)
        {
            minX = dicomValue->getValueAsInt();
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Shutter Left Vertical Edge en un arxiu que se suposa que l'ha de tenir!");
        }

        // Obtenim Shutter Right Vertical Edge, requerit quan shutter shape conté RECTANGULAR
        dicomValue = displayShutterItems->getValueAttribute(DICOMShutterRightVerticalEdge);
        if (dicomValue)
        {
            maxX = dicomValue->getValueAsInt();
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Shutter Right Vertical Edge en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Shutter Right Vertical Edge en un arxiu que se suposa que l'ha de tenir!");
        }
        
        // Obtenim Shutter Upper Horizontal Edge, requerit quan shutter shape conté RECTANGULAR
        dicomValue = displayShutterItems->getValueAttribute(DICOMShutterUpperHorizontalEdge);
        if (dicomValue)
        {
            minY = dicomValue->getValueAsInt();
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Shutter Upper Horizontal Edge en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Shutter Upper Horizontal Edge en un arxiu que se suposa que l'ha de tenir!");
        }
        
        // Obtenim Shutter Lower Horizontal Edge, requerit quan shutter shape conté RECTANGULAR
        dicomValue = displayShutterItems->getValueAttribute(DICOMShutterLowerHorizontalEdge);
        if (dicomValue)
        {
            maxY = dicomValue->getValueAsInt();
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Shutter Lower Horizontal Edge en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Shutter Lower Horizontal Edge en un arxiu que se suposa que l'ha de tenir!");
        }
        
        DisplayShutter rectangularShutter;
        rectangularShutter.setShape(DisplayShutter::RectangularShape);
        rectangularShutter.setShutterValue(presentationValue);
        rectangularShutter.setPoints(QPoint(minX, minY), QPoint(maxX, maxY));

        image->addDisplayShutter(rectangularShutter);
    }

    if (shutterShape.contains("CIRCULAR"))
    {
        // Obtenim Radius of Circular Shutter, requerit quan shutter shape conté CIRCULAR
        int radius = 0;
        dicomValue = displayShutterItems->getValueAttribute(DICOMRadiusOfCircularShutter);
        if (dicomValue)
        {
            radius = dicomValue->getValueAsInt();
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Radius of Circular Shutter en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Radius of Circular Shutter en un arxiu que se suposa que l'ha de tenir!");
        }

        // Obtenim Center of Circular Shutter, requerit quan shutter shape conté CIRCULAR
        QPoint centre;
        dicomValue = displayShutterItems->getValueAttribute(DICOMCenterOfCircularShutter);
        if (dicomValue)
        {
            QStringList centreStringList = dicomValue->getValueAsQString().split("\\");
            if (centreStringList.count() != 2)
            {
                DEBUG_LOG("L'atribut Center of Circular Shutter està en un format inesperat: " + dicomValue->getValueAsQString());
                ERROR_LOG("L'atribut Center of Circular Shutter està en un format inesperat: " + dicomValue->getValueAsQString());
            }
            else
            {
                centre.setX(centreStringList.at(0).toInt());
                centre.setY(centreStringList.at(1).toInt());
            }
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Radius of Circular Shutter en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Radius of Circular Shutter en un arxiu que se suposa que l'ha de tenir!");
        }

        DisplayShutter circularShutter;
        circularShutter.setShape(DisplayShutter::CircularShape);
        circularShutter.setShutterValue(presentationValue);
        circularShutter.setPoints(centre, radius);

        image->addDisplayShutter(circularShutter);
    }
    
    if (shutterShape.contains("POLYGONAL"))
    {
        QVector<QPoint> polygonVertices;
        
        // Obtenim Vertices of the Polygonal Shutter, requerit quan shutter shape conté POLYGONAL
        dicomValue = displayShutterItems->getValueAttribute(DICOMVerticesOfThePolygonalShutter);
        if (dicomValue)
        {
            QStringList vertices = dicomValue->getValueAsQString().split("\\");
            if (vertices.count() % 2 != 0)
            {
                DEBUG_LOG("L'atribut Vertices of the Polygonal Shutter està en un format inesperat: " + dicomValue->getValueAsQString());
                ERROR_LOG("L'atribut Vertices of the Polygonal Shutter està en un format inesperat: " + dicomValue->getValueAsQString());
            }
            else
            {
                for (int i = 0; i < vertices.count() - 1; i += 2 )
                {
                    polygonVertices << QPoint(vertices.at(i).toInt(), vertices.at(i + 1).toInt());
                }
            }
        }
        else
        {
            DEBUG_LOG("No s'ha trobat el tag Vertices of the Polygonal Shutter en un arxiu que se suposa que l'ha de tenir!");
            ERROR_LOG("No s'ha trobat el tag Vertices of the Polygonal Shutter en un arxiu que se suposa que l'ha de tenir!");
        }
        
        DisplayShutter polygonalShutter;
        polygonalShutter.setShape(DisplayShutter::PolygonalShape);
        polygonalShutter.setShutterValue(presentationValue);
        polygonalShutter.setPoints(polygonVertices);

        image->addDisplayShutter(polygonalShutter);
    }
}

void ImageFillerStep::fillDisplayShutterInformation(Image *image, DICOMTagReader *dicomReader)
{
    Q_ASSERT(image);
    Q_ASSERT(dicomReader);

    //
    // Obtenim Shutter Shape (1)
    //
    DICOMValueAttribute *dicomValue = dicomReader->getValueAttribute(DICOMShutterShape);
    QString shutterShape;
    if (dicomValue)
    {
        // Si tenim el valor de Shutter Shape, llavors és que tenim shutters
        // Posem els atributs en una seqüència i els tractem en el mètode específic
        DICOMSequenceItem *displayShutterModuleItems = new DICOMSequenceItem;
        displayShutterModuleItems->addAttribute(dicomValue);

        //
        // Obtenim el valor de presentació (3)
        //
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMShutterPresentationValue));
        // Obtenim els atributs requerits quan shutter shape conté RECTANGULAR
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMShutterLeftVerticalEdge));
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMShutterRightVerticalEdge));
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMShutterUpperHorizontalEdge));
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMShutterLowerHorizontalEdge));
        // Obtenim els atributs requerits quan shutter shape conté CIRCULAR
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMRadiusOfCircularShutter));
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMCenterOfCircularShutter));
        // Obtenim Vertices of the Polygonal Shutter, requerit quan shutter shape conté POLYGONAL
        displayShutterModuleItems->addAttribute(dicomReader->getValueAttribute(DICOMVerticesOfThePolygonalShutter));

        // Tractem els tags adequadament i omplim image
        fillDisplayShutterInformation(image, displayShutterModuleItems);
    }
    else
    {
        // No existeix el tag, per tant, no hi ha res a omplir
        return;
    }
}

void ImageFillerStep::computePixelSpacing(Image *image, DICOMTagReader *dicomReader)
{
    Q_ASSERT(image);
    Q_ASSERT(dicomReader);

    //
    // Obtenim el pixel spacing segons la modalitat que estem tractant
    //
    QString value;
    QString modality = dicomReader->getValueAttributeAsQString(DICOMModality);

    //
    // Per modalitats CT, MR i PET el pixel spacing el trobem
    // a Image Plane Module (C.7.6.2), al tag Pixel Spacing, tipus 1
    //
    if (modality == "CT" || modality == "MR" || modality == "PET")
    {
        value = dicomReader->getValueAttributeAsQString(DICOMPixelSpacing);
    }
    else if (modality == "US")
    {
        //
        // En el cas de la modalitat US, hem de fer alguns càlculs extra per tal obtenir un pixel spacing aproximat
        //
        DICOMSequenceAttribute *ultraSoundsRegionsSequence = dicomReader->getSequenceAttribute(DICOMSequenceOfUltrasoundRegions);
        // Ho hem de comprovar perquè és opcional.
        if (ultraSoundsRegionsSequence)
        {
            // Aquesta seqüència pot tenir més d'un ítem. TODO Nosaltres només tractem el primer, però ho hauríem de fer per tots,
            // ja que defineix més d'una regió i podríem estar obtenint informació equivocada
            QList<DICOMSequenceItem*> items = ultraSoundsRegionsSequence->getItems();
            if (!items.isEmpty())
            {
                int physicalUnitsX = items.at(0)->getValueAttribute(DICOMPhysicalUnitsXDirection)->getValueAsInt();
                int physicalUnitsY = items.at(0)->getValueAttribute(DICOMPhysicalUnitsYDirection)->getValueAsInt();

                // 3 significa que les unitats son cm
                if (physicalUnitsX == 3 && physicalUnitsY == 3)
                {
                    double physicalDeltaX = items.at(0)->getValueAttribute(DICOMPhysicalDeltaX)->getValueAsDouble();
                    double physicalDeltaY = items.at(0)->getValueAttribute(DICOMPhysicalDeltaY)->getValueAsDouble();

                    physicalDeltaX = std::abs(physicalDeltaX) * 10.;
                    physicalDeltaY = std::abs(physicalDeltaY) * 10.;

                    value = QString("%1").arg(physicalDeltaX);
                    value += "\\";
                    value += QString("%1").arg(physicalDeltaY);
                }
            }
        }
    }
    // Per altres modalitats li assignarem a partir d'aquest tag
    else
    {
        // Als mòduls CR Image (C.8.1.2), X-Ray Acquisition (C.8.7.2), DX Detector (C.8.11.4),
        // XA/XRF Acquisition (C.8.19.3), X-Ray 3D Angiographic Image Contributing Sources (C.8.21.2.1) i
        // X-Ray 3D Craniofacial Image Contributing Sources (C.8.21.2.2)
        // podem trobar el tag ImagerPixelSpacing, que segons el mòdul serà de tipus 1,1C ó 3
        value = dicomReader->getValueAttributeAsQString(DICOMImagerPixelSpacing);

        // TODO en els casos de X-Ray 3D Angiographic Image Contributing Sources (C.8.21.2.1) i
        // X-Ray 3D Craniofacial Image Contributing Sources (C.8.21.2.2), aquest tag es troba dins de
        // la seqüència Contributing Sources Sequence, que de moment no tractarem
    }

    QStringList list;
    if (!value.isEmpty())
    {
        list = value.split("\\");
        if (list.size() == 2)
        {
            image->setPixelSpacing(list.at(0).toDouble(), list.at(1).toDouble());
        }
        else
        {
            DEBUG_LOG("No s'ha trobat cap valor de pixel spacing definit de forma estàndar esperada. Modalitat de la imatge: [" + modality + "]");
        }
    }
}

bool ImageFillerStep::areOfDifferentSize(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    return firstImage->getColumns() != secondImage->getColumns() || firstImage->getRows() != secondImage->getRows();
}

bool ImageFillerStep::areOfDifferentPhotometricInterpretation(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    return firstImage->getPhotometricInterpretation().trimmed() != secondImage->getPhotometricInterpretation().trimmed();
}

bool ImageFillerStep::areOfDifferentPixelSpacing(Image *firstImage, Image *secondImage)
{
    Q_ASSERT(firstImage);
    Q_ASSERT(secondImage);

    const double *spacing1 = firstImage->getPixelSpacing();
    const double *spacing2 = secondImage->getPixelSpacing();

    return QString::number(spacing1[0], 'f', 3) != QString::number(spacing2[0], 'f', 3) || QString::number(spacing1[1], 'f', 3) != QString::number(spacing2[1], 'f', 3);
}

bool ImageFillerStep::isEnhancedImageSOPClass(const QString &sopClassUID)
{
    return (sopClassUID == UIDEnhancedCTImageStorage || sopClassUID == UIDEnhancedMRImageStorage || sopClassUID == UIDEnhancedXAImageStorage ||
        sopClassUID == UIDEnhancedXRFImageStorage || sopClassUID == UIDEnhancedUSVolumeStorage || sopClassUID == UIDEnhancedMRColorImageStorage ||
        sopClassUID == UIDEnhancedPETImageStorage);
}

}
