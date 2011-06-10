#include "orderimagesfillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"
#include "mathtools.h"
#include "dicomtagreader.h"

namespace udg {

OrderImagesFillerStep::OrderImagesFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "ImageFillerStep";
    m_priority = HighPriority;
}

OrderImagesFillerStep::~OrderImagesFillerStep()
{
    QMap<unsigned long, Image*> *instanceNumberSet;
    QMap<double, QMap<unsigned long, Image*>*> *imagePositionSet;
    QMap<QString, QMap<double, QMap< unsigned long, Image*>*>*> *normalVectorImageSet;
    QMap<int, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *volumesInSeries;

    foreach (Series *key, OrderImagesInternalInfo.keys())
    {
        volumesInSeries = OrderImagesInternalInfo.take(key);
        foreach (int volumeNumber, volumesInSeries->keys())
        {
            normalVectorImageSet = volumesInSeries->take(volumeNumber);
            foreach (QString normalVectorKey, normalVectorImageSet->keys())
            {
                imagePositionSet = normalVectorImageSet->take(normalVectorKey);
                foreach (double distanceKey, imagePositionSet->keys())
                {
                    instanceNumberSet = imagePositionSet->take(distanceKey);
                    delete instanceNumberSet;
                }
                delete imagePositionSet;
            }
            delete normalVectorImageSet;
        }
        delete volumesInSeries;
    }

    foreach (Series *series, m_acquisitionNumberEvaluation.keys())
    {
        QHash<int, QPair<QString, bool>*> volumeHash = m_acquisitionNumberEvaluation.take(series);
        foreach (int volumeNumber, volumeHash.keys())
        {
            QPair<QString, bool> *pair = volumeHash.take(volumeNumber);
            delete pair;
        }
    }
}

bool OrderImagesFillerStep::fillIndividually()
{
    QMap<int, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *volumesInSeries;

    if (OrderImagesInternalInfo.contains(m_input->getCurrentSeries()))
    {
        volumesInSeries = OrderImagesInternalInfo.value(m_input->getCurrentSeries());
        if (volumesInSeries->contains(m_input->getCurrentVolumeNumber()))
        {
            m_orderedImageSet = volumesInSeries->value(m_input->getCurrentVolumeNumber());
        }
        else
        {
            DEBUG_LOG(QString("Llista nul·la pel volum %1 de la serie %2. En creem una de nova.").arg(m_input->getCurrentVolumeNumber()).arg(
                      m_input->getCurrentSeries()->getInstanceUID()));

            m_orderedImageSet = new QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>();
            volumesInSeries->insert(m_input->getCurrentVolumeNumber(), m_orderedImageSet);
        }
    }
    else
    {
        DEBUG_LOG(QString("Llista nul·la en creem una de nova per la serie %1.").arg(m_input->getCurrentSeries()->getInstanceUID()));
        volumesInSeries = new QMap<int, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>();
        m_orderedImageSet = new QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>();
        volumesInSeries->insert(m_input->getCurrentVolumeNumber(), m_orderedImageSet);
        OrderImagesInternalInfo.insert(m_input->getCurrentSeries(), volumesInSeries);
    }

    foreach (Image * image, m_input->getCurrentImages())
    {
        processImage(image);
    }

    // Avaluació dels AcquisitionNumbers
    if (m_acquisitionNumberEvaluation.contains(m_input->getCurrentSeries()))
    {
        if (m_acquisitionNumberEvaluation.value(m_input->getCurrentSeries()).contains(m_input->getCurrentVolumeNumber()))
        {
            // Comparem els Acquisition Numbers
            QPair<QString, bool> *pair = m_acquisitionNumberEvaluation[m_input->getCurrentSeries()][m_input->getCurrentVolumeNumber()];
            if (!pair->second)
            {
                if (pair->first != m_input->getDICOMFile()->getValueAttributeAsQString(DICOMAcquisitionNumber))
                {
                    pair->second = true;
                }
            }
        }
        else
        {
            QPair<QString, bool> *acquisitionPair = new QPair<QString, bool>(m_input->getDICOMFile()->getValueAttributeAsQString(DICOMAcquisitionNumber), false);
            m_acquisitionNumberEvaluation[m_input->getCurrentSeries()].insert(m_input->getCurrentVolumeNumber(), acquisitionPair);
        }
    }
    else
    {
        QHash<int, QPair<QString, bool>*> volumeHash;
        QPair<QString, bool> *acquisitionPair = new QPair<QString, bool>(m_input->getDICOMFile()->getValueAttributeAsQString(DICOMAcquisitionNumber), false);
        volumeHash.insert(m_input->getCurrentVolumeNumber(), acquisitionPair);

        m_acquisitionNumberEvaluation.insert(m_input->getCurrentSeries(), volumeHash);
    }

    m_input->addLabelToSeries("OrderImagesFillerStep", m_input->getCurrentSeries());

    return true;
}

void OrderImagesFillerStep::postProcessing()
{
    foreach (Series *key, OrderImagesInternalInfo.keys())
    {
        setOrderedImagesIntoSeries(key);
    }
}

void OrderImagesFillerStep::processImage(Image *image)
{
    // Obtenim el vector normal del pla, que ens determina també a quin "stack" pertany la imatge
    QVector3D planeNormalVector3D = image->getImageOrientationPatient().getNormalVector();
    // El passem a string que ens serà més fàcil de comparar,perquè així és com es guarda a l'estructura d'ordenació
    QString planeNormalString = QString("%1\\%2\\%3").arg(planeNormalVector3D.x(), 0, 'f', 5).arg(planeNormalVector3D.y(), 0, 'f', 5)
                                   .arg(planeNormalVector3D.z(), 0, 'f', 5);
    // Passem el vector 3D a un array de C perquè el necessitem pel MathTools::angleInDegrees()
    double planeNormalArray[3] = { planeNormalVector3D.x(), planeNormalVector3D.y(), planeNormalVector3D.z() };

    QMap<double, QMap<unsigned long, Image*>*> *imagePositionSet;
    QMap<unsigned long, Image*> *instanceNumberSet;

    double distance = this->distance(image);

    // Primer busquem quina és la key (normal del pla) més semblant de totes les que hi ha
    // Cada key és la normal de cada pla guardat com a string.
    // En cas que tinguem diferents normals, indicaria que tenim per exemple, diferents stacks en el mateix volum
    QStringList planeNormals = m_orderedImageSet->uniqueKeys();

    // Aquest bucle serveix per trobar si la normal de la nova imatge
    // coincideix amb alguna normal de les imatges ja processada
    QString keyPlaneNormal;
    foreach (QString normal, planeNormals)
    {
        if (normal == planeNormalString)
        {
            // La normal d'aquest pla ja existeix (cas més típic)
            keyPlaneNormal = normal;
            break;
        }
        // Les normals són diferents, comprovar si ho són completament o no
        else
        {
            if (normal.isEmpty())
            {
                keyPlaneNormal = planeNormalString;
                break;
            }
            else
            {
                // Tot i que siguin diferents, pot ser que siguin gairebé iguals
                // llavors cal comprovar que de fet són prou diferents
                // ja que d'avegades només hi ha petites imprecisions simplement
                double normalVector[3];
                QStringList normalSplitted = normal.split("\\");
                normalVector[0] = normalSplitted.at(0).toDouble();
                normalVector[1] = normalSplitted.at(1).toDouble();
                normalVector[2] = normalSplitted.at(2).toDouble();

                double angle = MathTools::angleInDegrees(normalVector, planeNormalArray);
                if (angle < 1.0)
                {
                    // Si l'angle entre les normals
                    // està dins d'un threshold,
                    // les podem considerar iguals
                    // TODO definir millor aquest threshold
                    keyPlaneNormal = normal;
                    break;
                }
            }
        }
    }
    // Ara cal inserir la imatge a la llista ordenada
    // Si no hem posat cap valor, vol dir que la normal és nova i no existia fins el moment
    if (keyPlaneNormal.isEmpty())
    {
        // Assignem la clau
        keyPlaneNormal = planeNormalString;
        // Ara cal inserir la nova clau
        instanceNumberSet = new QMap<unsigned long, Image*>();
        instanceNumberSet->insert(QString("%1%2%3").arg(image->getInstanceNumber()).arg("0").arg(image->getFrameNumber()).toULong(), image);

        imagePositionSet = new QMap<double, QMap<unsigned long, Image*>*>();
        imagePositionSet->insert(distance, instanceNumberSet);
        m_orderedImageSet->insert(keyPlaneNormal, imagePositionSet);
    }
    // La normal ja existia [m_orderedImageSet->contains(keyPlaneNormal) == true], per tant només cal actualitzar l'estructura
    else
    {
        imagePositionSet = m_orderedImageSet->value(keyPlaneNormal);
        if (imagePositionSet->contains(distance))
        {
            // Hi ha series on les imatges comparteixen el mateix instance number.
            // Per evitar el problema es fa un insertMulti.
            imagePositionSet->value(distance)->insertMulti(QString("%1%2%3").arg(image->getInstanceNumber()).arg("0")
                                                              .arg(image->getFrameNumber()).toULong(), image);
        }
        else
        {
            instanceNumberSet = new QMap<unsigned long, Image*>();
            instanceNumberSet->insert(QString("%1%2%3").arg(image->getInstanceNumber()).arg("0").arg(image->getFrameNumber()).toULong(), image);
            imagePositionSet->insert(distance, instanceNumberSet);
        }
    }
}

void OrderImagesFillerStep::setOrderedImagesIntoSeries(Series *series)
{
    QList<Image*> imageSet;
    QMap<unsigned long, Image*> *instanceNumberSet;
    QMap<double, QMap<unsigned long, Image*>*> *imagePositionSet;
    QMap<double, QMap<double, QMap<unsigned long, Image*>*>*> lastOrderedImageSet;
    QMap<int, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *volumesInSeries;

    Image *currentImage;
    int orderNumberInVolume;

    volumesInSeries = OrderImagesInternalInfo.take(series);

    foreach (int currentVolumeNumber, volumesInSeries->keys())
    {
        // Multiple acquisition number
        if (m_acquisitionNumberEvaluation[series][currentVolumeNumber]->second)
        {
            DEBUG_LOG(QString("No totes les imatges tenen el mateix AcquisitionNumber. Ordenem el volume %1 de la serie %2 per Instance Number").arg(
                      currentVolumeNumber).arg(series->getInstanceUID()));
            INFO_LOG(QString("No totes les imatges tenen el mateix AcquisitionNumber. Ordenem el volume %1 de la serie %2 per Instance Number").arg(
                     currentVolumeNumber).arg(series->getInstanceUID()));
            m_orderedImageSet = volumesInSeries->take(currentVolumeNumber);
            QMap<unsigned long, Image*> sortedImagesByInstanceNumber;

            foreach (QString key, m_orderedImageSet->keys())
            {
                imagePositionSet = m_orderedImageSet->take(key);
                foreach (double key2, imagePositionSet->keys())
                {
                    instanceNumberSet = imagePositionSet->take(key2);
                    foreach (unsigned long key3, instanceNumberSet->keys())
                    {
                        currentImage = instanceNumberSet->take(key3);
                        sortedImagesByInstanceNumber.insertMulti(key3, currentImage);
                    }
                }
            }

            orderNumberInVolume = 0;

            foreach (Image *image, sortedImagesByInstanceNumber.values())
            {
                image->setOrderNumberInVolume(orderNumberInVolume);
                image->setVolumeNumberInSeries(currentVolumeNumber);
                orderNumberInVolume++;

                imageSet += image;
            }
        }
        else
        {
            m_orderedImageSet = volumesInSeries->take(currentVolumeNumber);
            // Cal ordernar les agrupacions d'imatges
            if (m_orderedImageSet->count() > 1)
            {
                foreach (QString key, m_orderedImageSet->keys())
                {
                    imagePositionSet = m_orderedImageSet->take(key);
                    Image *image = (*(*imagePositionSet->begin())->begin());

                    lastOrderedImageSet.insertMulti(this->distance(image), imagePositionSet);
                }
            }
            else
            {
                lastOrderedImageSet.insert(0, (*m_orderedImageSet->begin()));
                foreach (QString key, m_orderedImageSet->keys())
                {
                    m_orderedImageSet->take(key);
                }
            }

            orderNumberInVolume = 0;

            // Passar l'estructura a la series
            foreach (double key, lastOrderedImageSet.keys())
            {
                imagePositionSet = lastOrderedImageSet.take(key);
                foreach (double key2, imagePositionSet->keys())
                {
                    instanceNumberSet = imagePositionSet->take(key2);
                    foreach (unsigned long key3, instanceNumberSet->keys())
                    {
                        currentImage = instanceNumberSet->take(key3);
                        currentImage->setOrderNumberInVolume(orderNumberInVolume);
                        currentImage->setVolumeNumberInSeries(currentVolumeNumber);
                        orderNumberInVolume++;

                        imageSet += currentImage;
                    }
                }
            }
        }
    }
    series->setImages(imageSet);
}

double OrderImagesFillerStep::distance(Image *image)
{
    // Càlcul de la distància (basat en l'algorisme de Jolinda Smith)
    double distance = .0;
    // Origen del pla
    const double *imagePosition = image->getImagePositionPatient();
    // Normal del pla sobre la qual projectarem l'origen
    QVector3D normalVector = image->getImageOrientationPatient().getNormalVector();
    distance = normalVector.x() * imagePosition[0] + normalVector.y() * imagePosition[1] + normalVector.z() * imagePosition[2];

    return distance;
}

}
