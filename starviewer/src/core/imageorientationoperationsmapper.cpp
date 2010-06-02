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

void ImageOrientationOperationsMapper::setInitialOrientation( const QString &topLabel, const QString &leftLabel )
{
    m_initialOrientation = formatOrientationLabel(topLabel+"\\"+leftLabel);
    m_hasToUpdateOperations = true;
}
    
void ImageOrientationOperationsMapper::setDesiredOrientation( const QString &topLabel, const QString &leftLabel )
{
    m_desiredOrientation = formatOrientationLabel(topLabel+"\\"+leftLabel);
    m_hasToUpdateOperations = true;
}

int ImageOrientationOperationsMapper::getNumberOfClockwiseTurnsToApply()
{
    if( m_hasToUpdateOperations )
        updateOperations();
    return m_clockwiseTurns;
}

bool ImageOrientationOperationsMapper::requiresHorizontalFlip()
{
    if( m_hasToUpdateOperations )
        updateOperations();
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
    const QString AnteriorFeet("A\\F");
    const QString AnteriorHead("A\\H");
    const QString PosteriorFeet("P\\F");
    const QString PosteriorHead("P\\H");
    const QString FeetAnterior("F\\A");
    const QString FeetPosterior("F\\P");
    const QString HeadAnterior("H\\A");
    const QString HeadPosterior("H\\P");

    m_orientationMappingTable.insert(AnteriorFeet + "-" + AnteriorHead, Rotate180DegreesAndFlip); // Per passar de AF -> AH calen 2 rotacions i 1 flip horitzontal
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
    const QString AnteriorRight("A\\R");
    const QString AnteriorLeft("A\\L");
    const QString PosteriorRight("P\\R");
    const QString PosteriorLeft("P\\L");
    const QString RightAnterior("R\\A");
    const QString RightPosterior("R\\P");
    const QString LeftAnterior("L\\A");
    const QString LeftPosterior("L\\P");

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

    // TODO Faltarien els mapejos sobre la vista coronal!
}

void ImageOrientationOperationsMapper::updateOperations()
{
    // La orientació podria tenir més d'una lletra per row
    // com per exemple RA\AL en un tall que sigui oblicu
    // Per evitar això i no fer una llista enorme de transformacions,
    // agafarem només la primera lletra del row i de la columna ja que així ja és suficient
    QStringList rowColumn = m_initialOrientation.split("\\");
    QString mapIndex = rowColumn.at(0).left(1) + "\\" + rowColumn.at(1).left(1) + "-" + m_desiredOrientation;
    QString operations = m_orientationMappingTable.value( mapIndex );

    if( !operations.isEmpty() )
    {
        QStringList listOfOperations = operations.split(",");
        // Assignem el nombre de rotacions
        m_clockwiseTurns = listOfOperations[0].toInt();
        // Assignem el flip horitzontal si n'hi ha
        if( listOfOperations[1].toInt() )
            m_horizontalFlip = true;
        else
            m_horizontalFlip = false;
    }
    else
    {
        // Si no es troben combinacions, resetejem les operacions perquè aquesta sigui neutra si s'aplica
        m_clockwiseTurns = 0;
        m_horizontalFlip = false;
    }
    m_hasToUpdateOperations = false;    
}

QString ImageOrientationOperationsMapper::formatOrientationLabel( const QString &label )
{
    QString labelCopy = label;
    
    // Substituim les etiquetes Superior i Inferior pels seus equivalents
    labelCopy.replace("S","H");
    labelCopy.replace("I","F");
    
    return labelCopy;
}

} // end namespace udg
