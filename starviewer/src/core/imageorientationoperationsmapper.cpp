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

    m_orientationMappingTable.insert(AnteriorFeet + "-" + AnteriorHead, "2,1"); // per passar de AF -> AH calen 0 rotacions i 1 flip horitzontal
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorFeet, "0,1");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorHead, "2,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetAnterior, "1,1");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetPosterior, "3,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadPosterior, "3,1");

    m_orientationMappingTable.insert(AnteriorHead + "-" + AnteriorFeet, "2,1"); 
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorFeet, "2,0");
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorHead, "0,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadAnterior, "1,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetPosterior, "3,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadPosterior, "3,0");

    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorFeet, "0,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorHead, "2,0");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + PosteriorHead, "2,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadAnterior, "3,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetPosterior, "1,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadPosterior, "1,0");

    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorFeet, "2,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorHead, "0,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + PosteriorFeet, "2,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetAnterior, "3,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetPosterior, "1,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadPosterior, "1,1");

    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorFeet, "1,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorHead, "3,0");
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorFeet, "1,0");
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorHead, "3,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadAnterior, "0,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + FeetPosterior, "2,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadPosterior, "2,0");

    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorFeet, "1,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorHead, "3,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorFeet, "1,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorHead, "3,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + FeetAnterior, "2,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadAnterior, "2,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadPosterior, "0,1");

    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorFeet, "3,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorHead, "1,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorFeet, "3,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorHead, "1,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetAnterior, "0,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetPosterior, "2,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + HeadPosterior, "2,1");

    m_orientationMappingTable.insert(HeadPosterior + "-" + AnteriorFeet, "1,1");
    m_orientationMappingTable.insert(HeadPosterior + "-" + AnteriorHead, "1,0");
    m_orientationMappingTable.insert(HeadPosterior + "-" + PosteriorFeet, "3,0");
    m_orientationMappingTable.insert(HeadPosterior + "-" + PosteriorHead, "3,1");
    m_orientationMappingTable.insert(HeadPosterior + "-" + FeetAnterior, "2,0");
    m_orientationMappingTable.insert(HeadPosterior + "-" + HeadAnterior, "2,1");
    m_orientationMappingTable.insert(HeadPosterior + "-" + FeetPosterior, "0,1");

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

    m_orientationMappingTable.insert(AnteriorRight + "-" + AnteriorLeft, "2,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorRight, "0,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorLeft, "2,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightAnterior, "1,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightPosterior, "3,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftPosterior, "3,1");

    m_orientationMappingTable.insert(AnteriorLeft + "-" + AnteriorRight, "2,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorRight, "2,0");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorLeft, "0,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftAnterior, "1,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightPosterior, "3,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftPosterior, "3,0");

    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorRight, "0,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorLeft, "2,0");
    m_orientationMappingTable.insert(PosteriorRight + "-" + PosteriorLeft, "2,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftAnterior, "3,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightPosterior, "1,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftPosterior, "1,0");

    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorRight, "2,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorLeft, "0,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + PosteriorRight, "2,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightAnterior, "3,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightPosterior, "1,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftPosterior, "1,1");

    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorRight, "1,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorLeft, "3,0");
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorRight, "1,0");
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorLeft, "3,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftAnterior, "0,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + RightPosterior, "2,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftPosterior, "2,0");

    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorRight, "1,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorLeft, "3,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorRight, "1,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorLeft, "3,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + RightAnterior, "2,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftAnterior, "2,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftPosterior, "0,1");

    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorRight, "3,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorLeft, "1,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorRight, "3,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorLeft, "1,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightAnterior, "0,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightPosterior, "2,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + LeftPosterior, "2,1");

    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorRight, "3,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorLeft, "1,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorRight, "3,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorLeft, "1,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightAnterior, "2,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + LeftAnterior, "2,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightPosterior, "0,1");

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
