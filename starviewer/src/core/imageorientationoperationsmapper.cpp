#include "imageorientationoperationsmapper.h"

#include <QStringList>

namespace udg {

ImageOrientationOperationsMapper::ImageOrientationOperationsMapper()
 : m_clockwiseTurns(0), m_horizontalFlip(false), m_hasToUpdateOperations(false)
{
    initializeOrientationTable();
}

ImageOrientationOperationsMapper::~ImageOrientationOperationsMapper()
{
}

void ImageOrientationOperationsMapper::setInitialOrientation(const PatientOrientation &initialOrientation)
{
    m_initialOrientation = initialOrientation;
    m_hasToUpdateOperations = true;
}

void ImageOrientationOperationsMapper::setDesiredOrientation(const PatientOrientation &desiredOrientation)
{
    m_desiredOrientation = desiredOrientation;
    m_hasToUpdateOperations = true;
}

int ImageOrientationOperationsMapper::getNumberOfClockwiseTurnsToApply()
{
    if (m_hasToUpdateOperations)
    {
        updateOperations();
    }
    return m_clockwiseTurns;
}

bool ImageOrientationOperationsMapper::requiresHorizontalFlip()
{
    if (m_hasToUpdateOperations)
    {
        updateOperations();
    }
    return m_horizontalFlip;
}

void ImageOrientationOperationsMapper::initializeOrientationTable()
{
    // Expressem les possibles operacions amb constants
    const QString Rotate90Degrees("1,0");
    const QString Rotate180Degrees("2,0");
    const QString Rotate270Degrees("3,0");
    const QString Flip("0,1");
    const QString Rotate90DegreesAndFlip("1,1");
    const QString Rotate180DegreesAndFlip("2,1");
    const QString Rotate270DegreesAndFlip("3,1");

    // Mapeig de les operacions per la vista sagital
    // Definim les etiquetes possibles d'orientació
    const QString AnteriorFeet(PatientOrientation::AnteriorLabel + "\\" + PatientOrientation::FeetLabel);
    const QString AnteriorHead(PatientOrientation::AnteriorLabel + "\\" + PatientOrientation::HeadLabel);
    const QString PosteriorFeet(PatientOrientation::PosteriorLabel + "\\" + PatientOrientation::FeetLabel);
    const QString PosteriorHead(PatientOrientation::PosteriorLabel + "\\" + PatientOrientation::HeadLabel);
    const QString FeetAnterior(PatientOrientation::FeetLabel + "\\" + PatientOrientation::AnteriorLabel);
    const QString FeetPosterior(PatientOrientation::FeetLabel + "\\" + PatientOrientation::PosteriorLabel);
    const QString HeadAnterior(PatientOrientation::HeadLabel + "\\" + PatientOrientation::AnteriorLabel);
    const QString HeadPosterior(PatientOrientation::HeadLabel + "\\" + PatientOrientation::PosteriorLabel);

    // Per passar de AF -> AH calen 2 rotacions i 1 flip horitzontal
    m_orientationMappingTable.insert(AnteriorFeet + "-" + AnteriorHead, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorFeet, Flip);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorHead, Rotate180Degrees);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadAnterior, Rotate90Degrees);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetAnterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetPosterior, Rotate270Degrees);
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadPosterior, Rotate270DegreesAndFlip);

    m_orientationMappingTable.insert(AnteriorHead + "-" + AnteriorFeet, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorFeet, Rotate180Degrees);
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorHead, Flip);
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetAnterior, Rotate90Degrees);
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadAnterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetPosterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadPosterior, Rotate270Degrees);

    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorFeet, Flip);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorHead, Rotate180Degrees);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + PosteriorHead, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetAnterior, Rotate270Degrees);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadAnterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetPosterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadPosterior, Rotate90Degrees);

    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorFeet, Rotate180Degrees);
    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorHead, Flip);
    m_orientationMappingTable.insert(PosteriorHead + "-" + PosteriorFeet, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetAnterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadAnterior, Rotate270Degrees);
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetPosterior, Rotate90Degrees);
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadPosterior, Rotate90DegreesAndFlip);

    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorFeet, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorHead, Rotate270Degrees);
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorFeet, Rotate90Degrees);
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorHead, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadAnterior, Flip);
    m_orientationMappingTable.insert(FeetAnterior + "-" + FeetPosterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadPosterior, Rotate180Degrees);

    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorFeet, Rotate90Degrees);
    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorHead, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorFeet, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorHead, Rotate270Degrees);
    m_orientationMappingTable.insert(FeetPosterior + "-" + FeetAnterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadAnterior, Rotate180Degrees);
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadPosterior, Flip);

    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorFeet, Rotate270Degrees);
    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorHead, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorFeet, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorHead, Rotate90Degrees);
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetAnterior, Flip);
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetPosterior, Rotate180Degrees);
    m_orientationMappingTable.insert(HeadAnterior + "-" + HeadPosterior, Rotate180DegreesAndFlip);

    m_orientationMappingTable.insert(HeadPosterior + "-" + AnteriorFeet, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(HeadPosterior + "-" + AnteriorHead, Rotate90Degrees);
    m_orientationMappingTable.insert(HeadPosterior + "-" + PosteriorFeet, Rotate270Degrees);
    m_orientationMappingTable.insert(HeadPosterior + "-" + PosteriorHead, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(HeadPosterior + "-" + FeetAnterior, Rotate180Degrees);
    m_orientationMappingTable.insert(HeadPosterior + "-" + HeadAnterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(HeadPosterior + "-" + FeetPosterior, Flip);

    // Mappeig de les operacions per la vista axial
    // Definim les etiquetes possibles d'orientació
    const QString AnteriorRight(PatientOrientation::AnteriorLabel + "\\" + PatientOrientation::RightLabel);
    const QString AnteriorLeft(PatientOrientation::AnteriorLabel + "\\" + PatientOrientation::LeftLabel);
    const QString PosteriorRight(PatientOrientation::PosteriorLabel + "\\" + PatientOrientation::RightLabel);
    const QString PosteriorLeft(PatientOrientation::PosteriorLabel + "\\" + PatientOrientation::LeftLabel);
    const QString RightAnterior(PatientOrientation::RightLabel + "\\" + PatientOrientation::AnteriorLabel);
    const QString RightPosterior(PatientOrientation::RightLabel + "\\" + PatientOrientation::PosteriorLabel);
    const QString LeftAnterior(PatientOrientation::LeftLabel + "\\" + PatientOrientation::AnteriorLabel);
    const QString LeftPosterior(PatientOrientation::LeftLabel + "\\" + PatientOrientation::PosteriorLabel);

    m_orientationMappingTable.insert(AnteriorRight + "-" + AnteriorLeft, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorRight, Flip);
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorLeft, Rotate180Degrees);
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightAnterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftAnterior, Rotate90Degrees);
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightPosterior, Rotate270Degrees);
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftPosterior, Rotate270DegreesAndFlip);

    m_orientationMappingTable.insert(AnteriorLeft + "-" + AnteriorRight, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorRight, Rotate180Degrees);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorLeft, Flip);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightAnterior, Rotate90Degrees);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftAnterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightPosterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftPosterior, Rotate270Degrees);

    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorRight, Flip);
    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorLeft, Rotate180Degrees);
    m_orientationMappingTable.insert(PosteriorRight + "-" + PosteriorLeft, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightAnterior, Rotate270Degrees);
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftAnterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightPosterior, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftPosterior, Rotate90Degrees);

    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorRight, Rotate180Degrees);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorLeft, Flip);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + PosteriorRight, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightAnterior, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftAnterior, Rotate270Degrees);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightPosterior, Rotate90Degrees);
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftPosterior, Rotate90DegreesAndFlip);

    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorRight, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorLeft, Rotate270Degrees);
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorRight, Rotate90Degrees);
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorLeft, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftAnterior, Flip);
    m_orientationMappingTable.insert(RightAnterior + "-" + RightPosterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftPosterior, Rotate180Degrees);

    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorRight, Rotate90Degrees);
    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorLeft, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorRight, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorLeft, Rotate270Degrees);
    m_orientationMappingTable.insert(RightPosterior + "-" + RightAnterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftAnterior, Rotate180Degrees);
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftPosterior, Flip);

    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorRight, Rotate270Degrees);
    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorLeft, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorRight, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorLeft, Rotate90Degrees);
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightAnterior, Flip);
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightPosterior, Rotate180Degrees);
    m_orientationMappingTable.insert(LeftAnterior + "-" + LeftPosterior, Rotate180DegreesAndFlip);

    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorRight, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorLeft, Rotate90Degrees);
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorRight, Rotate270Degrees);
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorLeft, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightAnterior, Rotate180Degrees);
    m_orientationMappingTable.insert(LeftPosterior + "-" + LeftAnterior, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightPosterior, Flip);

    // Mapeig de les operacions per la vista coronal
    // Definim les etiquetes possibles d'orientació
    const QString RightFeet(PatientOrientation::RightLabel + "\\" + PatientOrientation::FeetLabel);
    const QString RightHead(PatientOrientation::RightLabel + "\\" + PatientOrientation::HeadLabel);
    const QString LeftFeet(PatientOrientation::LeftLabel + "\\" + PatientOrientation::FeetLabel);
    const QString LeftHead(PatientOrientation::LeftLabel + "\\" + PatientOrientation::HeadLabel);
    const QString FeetRight(PatientOrientation::FeetLabel + "\\" + PatientOrientation::RightLabel);
    const QString FeetLeft(PatientOrientation::FeetLabel + "\\" + PatientOrientation::LeftLabel);
    const QString HeadRight(PatientOrientation::HeadLabel + "\\" + PatientOrientation::RightLabel);
    const QString HeadLeft(PatientOrientation::HeadLabel + "\\" + PatientOrientation::LeftLabel);
    
    m_orientationMappingTable.insert(RightFeet + "-" + RightHead, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(RightFeet + "-" + LeftFeet, Flip);
    m_orientationMappingTable.insert(RightFeet + "-" + LeftHead, Rotate180Degrees);
    m_orientationMappingTable.insert(RightFeet + "-" + HeadRight, Rotate90Degrees);
    m_orientationMappingTable.insert(RightFeet + "-" + FeetRight, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(RightFeet + "-" + FeetLeft, Rotate270Degrees);
    m_orientationMappingTable.insert(RightFeet + "-" + HeadLeft, Rotate270DegreesAndFlip);

    m_orientationMappingTable.insert(RightHead + "-" + RightFeet, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(RightHead + "-" + LeftFeet, Rotate180Degrees);
    m_orientationMappingTable.insert(RightHead + "-" + LeftHead, Flip);
    m_orientationMappingTable.insert(RightHead + "-" + FeetRight, Rotate90Degrees);
    m_orientationMappingTable.insert(RightHead + "-" + HeadRight, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(RightHead + "-" + FeetLeft, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(RightHead + "-" + HeadLeft, Rotate270Degrees);

    m_orientationMappingTable.insert(LeftFeet + "-" + RightFeet, Flip);
    m_orientationMappingTable.insert(LeftFeet + "-" + RightHead, Rotate180Degrees);
    m_orientationMappingTable.insert(LeftFeet + "-" + LeftHead, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(LeftFeet + "-" + FeetRight, Rotate270Degrees);
    m_orientationMappingTable.insert(LeftFeet + "-" + HeadRight, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(LeftFeet + "-" + FeetLeft, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(LeftFeet + "-" + HeadLeft, Rotate90Degrees);

    m_orientationMappingTable.insert(LeftHead + "-" + RightFeet, Rotate180Degrees);
    m_orientationMappingTable.insert(LeftHead + "-" + RightHead, Flip);
    m_orientationMappingTable.insert(LeftHead + "-" + LeftFeet, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(LeftHead + "-" + FeetRight, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(LeftHead + "-" + HeadRight, Rotate270Degrees);
    m_orientationMappingTable.insert(LeftHead + "-" + FeetLeft, Rotate90Degrees);
    m_orientationMappingTable.insert(LeftHead + "-" + HeadLeft, Rotate90DegreesAndFlip);

    m_orientationMappingTable.insert(FeetRight + "-" + RightFeet, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(FeetRight + "-" + RightHead, Rotate270Degrees);
    m_orientationMappingTable.insert(FeetRight + "-" + LeftFeet, Rotate90Degrees);
    m_orientationMappingTable.insert(FeetRight + "-" + LeftHead, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(FeetRight + "-" + HeadRight, Flip);
    m_orientationMappingTable.insert(FeetRight + "-" + FeetLeft, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(FeetRight + "-" + HeadLeft, Rotate180Degrees);

    m_orientationMappingTable.insert(FeetLeft + "-" + RightFeet, Rotate90Degrees);
    m_orientationMappingTable.insert(FeetLeft + "-" + RightHead, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(FeetLeft + "-" + LeftFeet, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(FeetLeft + "-" + LeftHead, Rotate270Degrees);
    m_orientationMappingTable.insert(FeetLeft + "-" + FeetRight, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(FeetLeft + "-" + HeadRight, Rotate180Degrees);
    m_orientationMappingTable.insert(FeetLeft + "-" + HeadLeft, Flip);

    m_orientationMappingTable.insert(HeadRight + "-" + RightFeet, Rotate270Degrees);
    m_orientationMappingTable.insert(HeadRight + "-" + RightHead, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(HeadRight + "-" + LeftFeet, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(HeadRight + "-" + LeftHead, Rotate90Degrees);
    m_orientationMappingTable.insert(HeadRight + "-" + FeetRight, Flip);
    m_orientationMappingTable.insert(HeadRight + "-" + FeetLeft, Rotate180Degrees);
    m_orientationMappingTable.insert(HeadRight + "-" + HeadLeft, Rotate180DegreesAndFlip);

    m_orientationMappingTable.insert(HeadLeft + "-" + RightFeet, Rotate270DegreesAndFlip);
    m_orientationMappingTable.insert(HeadLeft + "-" + RightHead, Rotate90Degrees);
    m_orientationMappingTable.insert(HeadLeft + "-" + LeftFeet, Rotate270Degrees);
    m_orientationMappingTable.insert(HeadLeft + "-" + LeftHead, Rotate90DegreesAndFlip);
    m_orientationMappingTable.insert(HeadLeft + "-" + FeetRight, Rotate180Degrees);
    m_orientationMappingTable.insert(HeadLeft + "-" + HeadRight, Rotate180DegreesAndFlip);
    m_orientationMappingTable.insert(HeadLeft + "-" + FeetLeft, Flip);
}

void ImageOrientationOperationsMapper::updateOperations()
{
    // La orientació podria tenir més d'una lletra per row
    // com per exemple RA\AL en un tall que sigui oblicu
    // Per evitar això i no fer una llista enorme de transformacions,
    // agafarem només la primera lletra del row i de la columna ja que així ja és suficient
    QString initialRowLabel = m_initialOrientation.getRowDirectionLabel().left(1);
    QString initialColumnLabel = m_initialOrientation.getColumnDirectionLabel().left(1);

    QString desiredRowLabel = m_desiredOrientation.getRowDirectionLabel().left(1);
    QString desiredColumnLabel = m_desiredOrientation.getColumnDirectionLabel().left(1);
    
    QString mapIndex = initialRowLabel + "\\" + initialColumnLabel + "-" + desiredRowLabel + "\\" + desiredColumnLabel;
    QString operations = m_orientationMappingTable.value(mapIndex);

    if (!operations.isEmpty())
    {
        QStringList listOfOperations = operations.split(",");
        // Assignem el nombre de rotacions
        m_clockwiseTurns = listOfOperations[0].toInt();
        // Assignem el flip horitzontal si n'hi ha
        if (listOfOperations[1].toInt())
        {
            m_horizontalFlip = true;
        }
        else
        {
            m_horizontalFlip = false;
        }
    }
    else
    {
        // Si no es troben combinacions, resetejem les operacions perquè aquesta sigui neutra si s'aplica
        m_clockwiseTurns = 0;
        m_horizontalFlip = false;
    }
    m_hasToUpdateOperations = false;
}

} // End namespace udg
