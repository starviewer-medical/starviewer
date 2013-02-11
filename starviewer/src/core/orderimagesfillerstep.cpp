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
    QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*> *orderedImageSet;
    QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*> *normalVectorImageSet;
    QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*> *volumesInSeries;

    foreach (Series *key, m_orderImagesInternalInfo.keys())
    {
        volumesInSeries = m_orderImagesInternalInfo.take(key);
        foreach (int volumeNumber, volumesInSeries->keys())
        {
            normalVectorImageSet = volumesInSeries->take(volumeNumber);
            foreach (double normalVectorKey, normalVectorImageSet->keys())
            {
                orderedImageSet = normalVectorImageSet->take(normalVectorKey);
                foreach (QString key, orderedImageSet->keys())
                {
                    imagePositionSet = orderedImageSet->take(key);
                    foreach (double distanceKey, imagePositionSet->keys())
                    {
                        instanceNumberSet = imagePositionSet->take(distanceKey);
                        delete instanceNumberSet;
                    }
                    delete imagePositionSet;
                }
                delete orderedImageSet;
            }
            delete normalVectorImageSet;
        }
        delete volumesInSeries;
    }

    foreach (Series *series, m_phasesPerPositionEvaluation.keys())
    {
        QHash<int, PhasesPerPositionHashType*> *volumeHash = m_phasesPerPositionEvaluation.take(series);
        foreach (int volumeNumber, volumeHash->keys())
        {
            PhasesPerPositionHashType *phasesPerPositionHash = volumeHash->take(volumeNumber);
            delete phasesPerPositionHash;
        }
        delete volumeHash;
    }

    QHash<Series*, QHash<int, bool>* > m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash;
    foreach (Series *series, m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash.keys())
    {
        QHash<int, bool> *volumeHash = m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash.take(series);
        delete volumeHash;
    }
}

bool OrderImagesFillerStep::fillIndividually()
{
    QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*> *volumesInSeries;

    if (m_orderImagesInternalInfo.contains(m_input->getCurrentSeries()))
    {
        volumesInSeries = m_orderImagesInternalInfo.value(m_input->getCurrentSeries());
        if (volumesInSeries->contains(m_input->getCurrentVolumeNumber()))
        {
            m_orderedNormalsSet = volumesInSeries->value(m_input->getCurrentVolumeNumber());
        }
        else
        {
            DEBUG_LOG(QString("Llista nul·la pel volum %1 de la serie %2. En creem una de nova.").arg(m_input->getCurrentVolumeNumber()).arg(
                      m_input->getCurrentSeries()->getInstanceUID()));
            m_orderedNormalsSet = new QMap<double,QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>();
            volumesInSeries->insert(m_input->getCurrentVolumeNumber(), m_orderedNormalsSet);
        }
    }
    else
    {
        DEBUG_LOG(QString("Llista nul·la en creem una de nova per la serie %1.").arg(m_input->getCurrentSeries()->getInstanceUID()));
        volumesInSeries = new QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*>();
        m_orderedNormalsSet = new QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>();

        volumesInSeries->insert(m_input->getCurrentVolumeNumber(), m_orderedNormalsSet);
        m_orderImagesInternalInfo.insert(m_input->getCurrentSeries(), volumesInSeries);
    }

    foreach (Image * image, m_input->getCurrentImages())
    {
        processImage(image);
        // Avaluació del nombre de fases per posició
        processPhasesPerPositionEvaluation(image);
    }

    m_input->addLabelToSeries("OrderImagesFillerStep", m_input->getCurrentSeries());

    return true;
}

void OrderImagesFillerStep::postProcessing()
{
    // Per cada subvolum de cada sèrie, comprovem si aquestes tenen el mateix nombre de fases per posició
    foreach (Series *currentSeries, m_phasesPerPositionEvaluation.keys())
    {
        // <VolumeNumber, SameNumberOfPhasesPerPosition?>
        QHash<int, bool> *volumeNumberPhaseCountHash = new QHash<int, bool>();
        foreach (int currentVolumeNumber, m_phasesPerPositionEvaluation.value(currentSeries)->keys())
        {
            PhasesPerPositionHashType *phasesPerPositionHash = m_phasesPerPositionEvaluation.value(currentSeries)->value(currentVolumeNumber);
            // Si passem la llista de valors a un QSet i aquest té mida 1, vol dir que totes les posicions tenen el mateix nombre de fases
            // (Un QSet no admet duplicats). S'ha de tenir en compte que si només hi ha una posició amb diferents fases no cal fer res ja que serà correcte
            QList<int> phasesList = phasesPerPositionHash->values();
            int listSize = phasesList.count();
            bool sameNumberOfPhases = true;
            if (listSize > 1 && phasesList.toSet().count() > 1)
            {
                sameNumberOfPhases = false;
            }
            volumeNumberPhaseCountHash->insert(currentVolumeNumber, sameNumberOfPhases);
        }
        m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash.insert(currentSeries, volumeNumberPhaseCountHash);
    }
    
    foreach (Series *key, m_orderImagesInternalInfo.keys())
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

    QMap<double, QMap<unsigned long, Image*>*> *imagePositionSet;
    QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*> *orderedImageSet;
    QMap<unsigned long, Image*> *instanceNumberSet;

    double distance = Image::distance(image);

    // Primer busquem quina és la key (normal del pla) més semblant de totes les que hi ha
    // Cada key és la normal de cada pla guardat com a string.
    // En cas que tinguem diferents normals, indicaria que tenim per exemple, diferents stacks en el mateix volum

    QStringList planeNormals;
    foreach (double key, m_orderedNormalsSet->uniqueKeys())
    {
        planeNormals << m_orderedNormalsSet->value(key)->uniqueKeys();
    }

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
                // ja que a vegades només hi ha petites imprecisions simplement
                QStringList normalSplitted = normal.split("\\");
                QVector3D normalVector(normalSplitted.at(0).toDouble(), normalSplitted.at(1).toDouble(), normalSplitted.at(2).toDouble());

                double angle = MathTools::angleInDegrees(normalVector, planeNormalVector3D);

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
        
        orderedImageSet = new QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>();
        orderedImageSet->insert(keyPlaneNormal, imagePositionSet);

        double angle = 0;

        if(m_orderedNormalsSet->isEmpty())
        {
            m_firstPlaneVector3D = planeNormalVector3D;
        }
        else
        {
            if(m_orderedNormalsSet->size() == 1) // Busquem la normal per saber la direcció per on s'han d'ordenar
            {
                m_direction = QVector3D::crossProduct(m_firstPlaneVector3D, planeNormalVector3D);
                m_direction = QVector3D::crossProduct(m_direction, m_firstPlaneVector3D);
            }
            
            angle = MathTools::angleInRadians(m_firstPlaneVector3D, planeNormalVector3D);

            if (QVector3D::dotProduct(planeNormalVector3D, m_direction) <= 0) // Direcció d'ordenació
            {
                angle = 2 * MathTools::PiNumber - angle;
            }
        }
        m_orderedNormalsSet->insertMulti(angle, orderedImageSet);

    }
    // La normal ja existia [m_orderedImageSet->contains(keyPlaneNormal) == true], per tant només cal actualitzar l'estructura
    else
    {
        foreach (double key, m_orderedNormalsSet->keys())
        {
            if(m_orderedNormalsSet->value(key)->contains(keyPlaneNormal))
            {
                orderedImageSet = m_orderedNormalsSet->value(key);
            }
        }

        imagePositionSet = orderedImageSet->value(keyPlaneNormal);
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

void OrderImagesFillerStep::processPhasesPerPositionEvaluation(Image *image)
{
    Series *currentSeries = m_input->getCurrentSeries();
    
    // Comprovem si tenim hash per la sèrie actual i l'assignem en cas que sigui necessari
    QHash<int, PhasesPerPositionHashType*> *volumeNumberPhasesPerPositionHash = 0;
    if (m_phasesPerPositionEvaluation.contains(currentSeries))
    {
        volumeNumberPhasesPerPositionHash = m_phasesPerPositionEvaluation.value(currentSeries);
    }
    else
    {
        volumeNumberPhasesPerPositionHash = new QHash<int, PhasesPerPositionHashType*>();
        m_phasesPerPositionEvaluation.insert(currentSeries, volumeNumberPhasesPerPositionHash);
    }

    // Ara comprovem si el segon hash té l'actual número de volum i li assignem els valors en cas que sigui necessari
    int currentVolumeNumber = m_input->getCurrentVolumeNumber();
    PhasesPerPositionHashType *phasesPerPositionHash = 0;
    if (volumeNumberPhasesPerPositionHash->contains(currentVolumeNumber))
    {
        phasesPerPositionHash = volumeNumberPhasesPerPositionHash->value(currentVolumeNumber);
    }
    else
    {
        phasesPerPositionHash = new PhasesPerPositionHashType;
        volumeNumberPhasesPerPositionHash->insert(currentVolumeNumber, phasesPerPositionHash);
    }

    // Ara ja tenim el hash de les fases per posició corresponent a l'actual sèrie i número de volum
    // Procedim a inserir la informació corresponent
    const double *imagePositionPatient = image->getImagePositionPatient();

    if (!(imagePositionPatient[0] == 0. && imagePositionPatient[1] == 0. && imagePositionPatient[2] == 0.))
    {
        QString imagePositionPatientString = QString("%1\\%2\\%3").arg(imagePositionPatient[0])
                                                                    .arg(imagePositionPatient[1])
                                                                    .arg(imagePositionPatient[2]);

        if (phasesPerPositionHash->contains(imagePositionPatientString))
        {
            // Ja el tenim, augmentem el nombre de fases per aquella posició
            phasesPerPositionHash->insert(imagePositionPatientString, phasesPerPositionHash->value(imagePositionPatientString) + 1);
        }
        else
        {
            // Creem la nova entrada, inicialment seria la primera fase
            phasesPerPositionHash->insert(imagePositionPatientString, 1);
        }
        // TODO L'if anterior en principi es podria resumir en aquesta sola línia
        // m_phasesPerPositionHash.insert(imagePositionPatientString, m_phasesPerPositionHash.value(imagePositionPatientString, 0) + 1);
    }
}

void OrderImagesFillerStep::setOrderedImagesIntoSeries(Series *series)
{
    QList<Image*> imageSet;
    QMap<unsigned long, Image*> *instanceNumberSet;
    QMap<double, QMap<unsigned long, Image*>*> *imagePositionSet;
    QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*> *orderedImageSet;
    QMap<double, QMap<double, QMap<double, QMap<unsigned long, Image*>*>*>*> lastOrderedImageSetDistanceAngle;
    QMap<double, QMap<double, QMap<double, QMap<unsigned long, Image*>*>*>*> lastOrderedImageSetDistanceStack;
    QMap<double, QMap<double, QMap<unsigned long, Image*>*>*> *lastOrderedImageSetDistance;
    QMap<int, QMap<double, QMap<QString, QMap<double, QMap<unsigned long, Image*>*>*>*>*> *volumesInSeries;

    Image *currentImage;
    int orderNumberInVolume;

    volumesInSeries = m_orderImagesInternalInfo.take(series);

    foreach (int currentVolumeNumber, volumesInSeries->keys())
    {
        bool orderByInstanceNumber = false;
        // Diferent número d'imatges per fase
        if (!m_sameNumberOfPhasesPerPositionPerVolumeInSeriesHash.value(series)->value(currentVolumeNumber))
        {
            orderByInstanceNumber = true;
            DEBUG_LOG(QString("No totes les imatges tenen el mateix nombre de fases. Ordenem el volume %1 de la serie %2 per Instance Number").arg(
                      currentVolumeNumber).arg(series->getInstanceUID()));
            INFO_LOG(QString("No totes les imatges tenen el mateix nombre de fases. Ordenem el volume %1 de la serie %2 per Instance Number").arg(
                     currentVolumeNumber).arg(series->getInstanceUID()));
        }

        if (orderByInstanceNumber)
        {
            m_orderedNormalsSet = volumesInSeries->take(currentVolumeNumber);
            QMap<unsigned long, Image*> sortedImagesByInstanceNumber;

            foreach (double key, m_orderedNormalsSet->keys())
            {
                orderedImageSet = m_orderedNormalsSet->take(key);
                foreach (QString key, orderedImageSet->keys())
                {
                    imagePositionSet = orderedImageSet->take(key);
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
            m_orderedNormalsSet = volumesInSeries->take(currentVolumeNumber);

            // Cal ordernar les agrupacions d'imatges
            foreach (double angle, m_orderedNormalsSet->keys())
            {
                orderedImageSet = m_orderedNormalsSet->take(angle);
                lastOrderedImageSetDistance = new QMap<double, QMap<double, QMap<unsigned long, Image*>*>*>();
                bool isRotational = true;

                foreach (QString normal, orderedImageSet->keys())
                {
                    imagePositionSet = orderedImageSet->take(normal);
                    
                    Image *firstImage = (*(*imagePositionSet->begin())->begin());
                    double distance = Image::distance(firstImage);
                    lastOrderedImageSetDistance->insertMulti(distance, imagePositionSet);

                    if (imagePositionSet->size() > 1)
                    {
                        int pos = (imagePositionSet->size())-1;
                        double key = imagePositionSet->keys().at(pos);
                        Image *lastImage = (*(imagePositionSet->value(key))->begin());
                        double distanceToFirstImage = fabs(Image::distance(lastImage)-distance);
                        if (distanceToFirstImage > 1.0) 
                        {
                            isRotational = false;
                        }
                    }
                }

                if (isRotational)//Rotacionals
                {
                    lastOrderedImageSetDistanceAngle.insertMulti(angle, lastOrderedImageSetDistance);
                }
                else //Stacks
                {
                    Image *image = (*(*imagePositionSet->begin())->begin());
                    lastOrderedImageSetDistanceStack.insertMulti(Image::distance(image), lastOrderedImageSetDistance);
                }
            }
            
            QList<QMap<double, QMap<double, QMap<unsigned long, Image*>*>*>*> orderedSet = lastOrderedImageSetDistanceStack.values();
            orderedSet.append(lastOrderedImageSetDistanceAngle.values());
            
            orderNumberInVolume = 0;

            for (int position = 0; position < orderedSet.size(); position++)
            {    
                lastOrderedImageSetDistance = orderedSet.value(position);
                foreach (double key2, lastOrderedImageSetDistance->keys())
                {
                    imagePositionSet = lastOrderedImageSetDistance->take(key2);
                    foreach (double key3, imagePositionSet->keys())
                    {
                        instanceNumberSet = imagePositionSet->take(key3);
                        foreach (unsigned long key4, instanceNumberSet->keys())
                        {
                            currentImage = instanceNumberSet->take(key4);
                            currentImage->setOrderNumberInVolume(orderNumberInVolume);
                            currentImage->setVolumeNumberInSeries(currentVolumeNumber);
                            orderNumberInVolume++;

                            imageSet += currentImage;
                        }
                    }
                }
            }
        }
    }
    series->setImages(imageSet);
}

}
