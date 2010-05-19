#include "imageorientationoperationsmapper.h"

#include <QStringList>

namespace udg {

ImageOrientationOperationsMapper::ImageOrientationOperationsMapper()
 : m_clockwiseTurns(0), m_verticalFlip(false), m_hasToUpdateOperations(false)
{
    initializeOrientationTable();
}

ImageOrientationOperationsMapper::~ImageOrientationOperationsMapper()
{
}

void ImageOrientationOperationsMapper::setInitialOrientation( const QString &topLabel, const QString &leftLabel )
{
    m_initialOrientation = topLabel+"\\"+leftLabel;
    m_hasToUpdateOperations = true;
}
    
void ImageOrientationOperationsMapper::setDesiredOrientation( const QString &topLabel, const QString &leftLabel )
{
    m_desiredOrientation = topLabel+"\\"+leftLabel;
    m_hasToUpdateOperations = true;
}

int ImageOrientationOperationsMapper::getNumberOfClockwiseTurnsToApply()
{
    if( m_hasToUpdateOperations )
        updateOperations();
    return m_clockwiseTurns;
}

bool ImageOrientationOperationsMapper::requiresVerticalFlip()
{
    if( m_hasToUpdateOperations )
        updateOperations();
    return m_verticalFlip;
}

void ImageOrientationOperationsMapper::initializeOrientationTable()
{
    // Mapeig de les operacions per la vista sagital
    // Definim les etiquetes possibles d'orientació
    const QString AnteriorFeet("A\\F");
    const QString AnteriorHead("A\\H");
    const QString PosteriorFeet("P\\F");
    const QString PosteriorHead("P\\H");
    const QString HeadAnterior("H\\A");
    const QString HeadPosterior("H\\P");
    const QString FeetAnterior("F\\A");
    const QString FeetPosterior("F\\P");
    // Superior ("S") és equivalent a Head ("H")
    // TODO en comptes de duplicar entrades equivalents, 
    // el que es podria fer és que quan ens introdueixin l'etiqueta "S" 
    // la substituïm internament per "H". El mateix passaria per l'etiqueta "I"
    // (Inferior) equivalent a "F" (Feet)
    const QString SuperiorPosterior("S\\P");
    const QString PosteriorSuperior("P\\S");

    m_orientationMappingTable.insert(AnteriorFeet + "-" + AnteriorHead, "0,1"); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorFeet, "2,1");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + PosteriorHead, "2,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetAnterior, "3,1");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + FeetPosterior, "3,0");
    m_orientationMappingTable.insert(AnteriorFeet + "-" + HeadPosterior, "1,1");

    m_orientationMappingTable.insert(AnteriorHead + "-" + AnteriorFeet, "0,1"); 
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorFeet, "2,0");
    m_orientationMappingTable.insert(AnteriorHead + "-" + PosteriorHead, "2,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadAnterior, "3,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + FeetPosterior, "1,1");
    m_orientationMappingTable.insert(AnteriorHead + "-" + HeadPosterior, "3,0");

    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorFeet, "2,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + AnteriorHead, "2,0");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + PosteriorHead, "0,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadAnterior, "1,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + FeetPosterior, "3,1");
    m_orientationMappingTable.insert(PosteriorFeet + "-" + HeadPosterior, "1,0");

    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorFeet, "2,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + AnteriorHead, "2,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + PosteriorFeet, "0,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetAnterior, "1,1");
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + FeetPosterior, "1,0");
    m_orientationMappingTable.insert(PosteriorHead + "-" + HeadPosterior, "3,1");

    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorFeet, "3,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + AnteriorHead, "3,0");
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorFeet, "1,0");
    m_orientationMappingTable.insert(FeetAnterior + "-" + PosteriorHead, "1,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadAnterior, "2,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + FeetPosterior, "0,1");
    m_orientationMappingTable.insert(FeetAnterior + "-" + HeadPosterior, "2,0");

    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorFeet, "3,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + AnteriorHead, "3,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorFeet, "1,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + PosteriorHead, "1,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetAnterior, "2,1");
    m_orientationMappingTable.insert(HeadAnterior + "-" + FeetPosterior, "2,0");
    m_orientationMappingTable.insert(HeadAnterior + "-" + HeadPosterior, "0,1");

    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorFeet, "1,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + AnteriorHead, "1,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorFeet, "3,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + PosteriorHead, "3,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + FeetAnterior, "0,1");
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadAnterior, "2,0");
    m_orientationMappingTable.insert(FeetPosterior + "-" + HeadPosterior, "2,1");

    m_orientationMappingTable.insert(SuperiorPosterior + "-" + AnteriorFeet, "3,1");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + AnteriorHead, "1,0");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + PosteriorFeet, "3,0");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + PosteriorSuperior, "1,1");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + FeetAnterior, "2,0");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + HeadAnterior, "0,1");
    m_orientationMappingTable.insert(SuperiorPosterior + "-" + FeetPosterior, "2,1");

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

    m_orientationMappingTable.insert(AnteriorRight + "-" + AnteriorLeft, "0,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorRight, "2,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + PosteriorLeft, "2,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightAnterior, "3,1");
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + RightPosterior, "3,0");
    m_orientationMappingTable.insert(AnteriorRight + "-" + LeftPosterior, "1,1");

    m_orientationMappingTable.insert(AnteriorLeft + "-" + AnteriorRight, "0,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorRight, "2,0");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + PosteriorLeft, "2,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightAnterior, "1,0");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftAnterior, "3,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + RightPosterior, "1,1");
    m_orientationMappingTable.insert(AnteriorLeft + "-" + LeftPosterior, "3,0");

    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorRight, "2,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + AnteriorLeft, "2,0");
    m_orientationMappingTable.insert(PosteriorRight + "-" + PosteriorLeft, "0,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftAnterior, "1,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + RightPosterior, "3,1");
    m_orientationMappingTable.insert(PosteriorRight + "-" + LeftPosterior, "1,0");

    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorRight, "2,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + AnteriorLeft, "2,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + PosteriorRight, "0,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightAnterior, "1,1");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftAnterior, "3,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + RightPosterior, "1,0");
    m_orientationMappingTable.insert(PosteriorLeft + "-" + LeftPosterior, "3,1");

    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorRight, "3,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + AnteriorLeft, "3,0");
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorRight, "1,0");
    m_orientationMappingTable.insert(RightAnterior + "-" + PosteriorLeft, "1,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftAnterior, "2,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + RightPosterior, "0,1");
    m_orientationMappingTable.insert(RightAnterior + "-" + LeftPosterior, "2,0");

    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorRight, "3,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + AnteriorLeft, "3,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorRight, "1,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + PosteriorLeft, "1,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightAnterior, "2,1");
    m_orientationMappingTable.insert(LeftAnterior + "-" + RightPosterior, "2,0");
    m_orientationMappingTable.insert(LeftAnterior + "-" + LeftPosterior, "0,1");

    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorRight, "1,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + AnteriorLeft, "1,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorRight, "3,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + PosteriorLeft, "3,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + RightAnterior, "0,1");
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftAnterior, "2,0");
    m_orientationMappingTable.insert(RightPosterior + "-" + LeftPosterior, "2,1");

    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorRight, "1,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + AnteriorLeft, "1,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorRight, "3,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + PosteriorLeft, "3,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightAnterior, "2,0");
    m_orientationMappingTable.insert(LeftPosterior + "-" + LeftAnterior, "0,1");
    m_orientationMappingTable.insert(LeftPosterior + "-" + RightPosterior, "2,1");

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
        // Assignem el flip vertical si n'hi ha
        if( listOfOperations[1].toInt() )
            m_verticalFlip = true;
        else
            m_verticalFlip = false;
    }
    else
    {
        // Si no es troben combinacions, resetejem les operacions perquè aquesta sigui neutra si s'aplica
        m_clockwiseTurns = 0;
        m_verticalFlip = false;
    }
    m_hasToUpdateOperations = false;    
}

} // end namespace udg
