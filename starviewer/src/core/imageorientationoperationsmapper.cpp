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
    m_orientationMappingTable.insert("A\\F-A\\H", "0,1"); // per passar de AF -> AH calen 0 rotacions i 1 flip vertical
    m_orientationMappingTable.insert("A\\F-P\\F", "2,1"); m_orientationMappingTable.insert("A\\F-P\\H", "2,0"); m_orientationMappingTable.insert("A\\F-H\\A", "1,0");
    m_orientationMappingTable.insert("A\\F-F\\A", "3,1"); m_orientationMappingTable.insert("A\\F-F\\P", "3,0"); m_orientationMappingTable.insert("A\\F-H\\P", "1,1");

    m_orientationMappingTable.insert("A\\H-A\\F", "0,1"); m_orientationMappingTable.insert("A\\H-P\\F", "2,0"); m_orientationMappingTable.insert("A\\H-P\\H", "2,1");
    m_orientationMappingTable.insert("A\\H-F\\A", "1,0"); m_orientationMappingTable.insert("A\\H-H\\A", "3,1"); m_orientationMappingTable.insert("A\\H-F\\P", "1,1");
    m_orientationMappingTable.insert("A\\H-H\\P", "3,0");

    m_orientationMappingTable.insert("P\\F-A\\F", "2,1"); m_orientationMappingTable.insert("P\\F-A\\H", "2,0"); m_orientationMappingTable.insert("P\\F-P\\H", "0,1");
    m_orientationMappingTable.insert("P\\F-F\\A", "3,0"); m_orientationMappingTable.insert("P\\F-H\\A", "1,1"); m_orientationMappingTable.insert("P\\F-F\\P", "3,1");
    m_orientationMappingTable.insert("P\\F-H\\P", "1,0");

    m_orientationMappingTable.insert("P\\H-A\\F", "2,0"); m_orientationMappingTable.insert("P\\H-A\\H", "2,1"); m_orientationMappingTable.insert("P\\H-P\\F", "0,1");
    m_orientationMappingTable.insert("P\\H-F\\A", "1,1"); m_orientationMappingTable.insert("P\\H-H\\A", "3,0"); m_orientationMappingTable.insert("P\\H-F\\P", "1,0");
    m_orientationMappingTable.insert("P\\H-H\\P", "3,1");

    m_orientationMappingTable.insert("F\\A-A\\F", "3,1"); m_orientationMappingTable.insert("F\\A-A\\H", "3,0"); m_orientationMappingTable.insert("F\\A-P\\F", "1,0");
    m_orientationMappingTable.insert("F\\A-P\\H", "1,1"); m_orientationMappingTable.insert("F\\A-H\\A", "2,1"); m_orientationMappingTable.insert("F\\A-F\\P", "0,1");
    m_orientationMappingTable.insert("F\\A-H\\P", "2,0");

    m_orientationMappingTable.insert("H\\A-A\\F", "3,0"); m_orientationMappingTable.insert("H\\A-A\\H", "3,1"); m_orientationMappingTable.insert("H\\A-P\\F", "1,1");
    m_orientationMappingTable.insert("H\\A-P\\H", "1,0"); m_orientationMappingTable.insert("H\\A-F\\A", "2,1"); m_orientationMappingTable.insert("H\\A-F\\P", "2,0");
    m_orientationMappingTable.insert("H\\A-H\\P", "0,1");

    m_orientationMappingTable.insert("F\\P-A\\F", "1,0"); m_orientationMappingTable.insert("F\\P-A\\H", "1,1"); m_orientationMappingTable.insert("F\\P-P\\F", "3,1");
    m_orientationMappingTable.insert("F\\P-P\\H", "3,0"); m_orientationMappingTable.insert("F\\P-F\\A", "0,1"); m_orientationMappingTable.insert("F\\P-H\\A", "2,0");
    m_orientationMappingTable.insert("F\\P-H\\P", "2,1");

    m_orientationMappingTable.insert("S\\P-A\\F", "3,1"); m_orientationMappingTable.insert("S\\P-A\\H", "1,0"); m_orientationMappingTable.insert("S\\P-P\\F", "3,0");
    m_orientationMappingTable.insert("S\\P-P\\S", "1,1"); m_orientationMappingTable.insert("S\\P-F\\A", "2,0"); m_orientationMappingTable.insert("S\\P-H\\A", "0,1");
    m_orientationMappingTable.insert("S\\P-F\\P", "2,1");

    // Mappeig de les operacions per la vista axial
    m_orientationMappingTable.insert("A\\R-A\\L", "0,1");	m_orientationMappingTable.insert("A\\R-P\\R", "2,1");	m_orientationMappingTable.insert("A\\R-P\\L", "2,0");
    m_orientationMappingTable.insert("A\\R-R\\A", "3,1");	m_orientationMappingTable.insert("A\\R-L\\A", "1,0");	m_orientationMappingTable.insert("A\\R-R\\P", "3,0");
    m_orientationMappingTable.insert("A\\R-L\\P", "1,1");

    m_orientationMappingTable.insert("A\\L-A\\R", "0,1");	m_orientationMappingTable.insert("A\\L-P\\R", "2,0");	m_orientationMappingTable.insert("A\\L-P\\L", "2,1");
    m_orientationMappingTable.insert("A\\L-R\\A", "1,0");	m_orientationMappingTable.insert("A\\L-L\\A", "3,1");	m_orientationMappingTable.insert("A\\L-R\\P", "1,1");
    m_orientationMappingTable.insert("A\\L-L\\P", "3,0");

    m_orientationMappingTable.insert("P\\R-A\\R", "2,1");	m_orientationMappingTable.insert("P\\R-A\\L", "2,0");	m_orientationMappingTable.insert("P\\R-P\\L", "0,1");
    m_orientationMappingTable.insert("P\\R-R\\A", "3,0");	m_orientationMappingTable.insert("P\\R-L\\A", "1,1");	m_orientationMappingTable.insert("P\\R-R\\P", "3,1");
    m_orientationMappingTable.insert("P\\R-L\\P", "1,0");

    m_orientationMappingTable.insert("P\\L-A\\R", "2,0");	m_orientationMappingTable.insert("P\\L-A\\L", "2,1");	m_orientationMappingTable.insert("P\\L-P\\R", "0,1");
    m_orientationMappingTable.insert("P\\L-R\\A", "1,1");	m_orientationMappingTable.insert("P\\L-L\\A", "3,0");	m_orientationMappingTable.insert("P\\L-R\\P", "1,0");
    m_orientationMappingTable.insert("P\\L-L\\P", "3,1");

    m_orientationMappingTable.insert("R\\A-A\\R", "3,1");	m_orientationMappingTable.insert("R\\A-A\\L", "3,0");	m_orientationMappingTable.insert("R\\A-P\\R", "1,0");
    m_orientationMappingTable.insert("R\\A-P\\L", "1,1");	m_orientationMappingTable.insert("R\\A-L\\A", "2,1");	m_orientationMappingTable.insert("R\\A-R\\P", "0,1");
    m_orientationMappingTable.insert("R\\A-L\\P", "2,0");

    m_orientationMappingTable.insert("L\\A-A\\R", "3,0");	m_orientationMappingTable.insert("L\\A-A\\L", "3,1");	m_orientationMappingTable.insert("L\\A-P\\R", "1,1");
    m_orientationMappingTable.insert("L\\A-P\\L", "1,0");	m_orientationMappingTable.insert("L\\A-R\\A", "2,1");	m_orientationMappingTable.insert("L\\A-R\\P", "2,0");
    m_orientationMappingTable.insert("L\\A-L\\P", "0,1");

    m_orientationMappingTable.insert("R\\P-A\\R", "1,0");	m_orientationMappingTable.insert("R\\P-A\\L", "1,1");	m_orientationMappingTable.insert("R\\P-P\\R", "3,1");
    m_orientationMappingTable.insert("R\\P-P\\L", "3,0");	m_orientationMappingTable.insert("R\\P-R\\A", "0,1");	m_orientationMappingTable.insert("R\\P-L\\A", "2,0");
    m_orientationMappingTable.insert("R\\P-L\\P", "2,1");

    m_orientationMappingTable.insert("L\\P-A\\R", "1,1");	m_orientationMappingTable.insert("L\\P-A\\L", "1,0");	m_orientationMappingTable.insert("L\\P-P\\R", "3,0");
    m_orientationMappingTable.insert("L\\P-P\\L", "3,1");	m_orientationMappingTable.insert("L\\P-R\\A", "2,0");	m_orientationMappingTable.insert("L\\P-L\\A", "0,1");
    m_orientationMappingTable.insert("L\\P-R\\P", "2,1");
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
