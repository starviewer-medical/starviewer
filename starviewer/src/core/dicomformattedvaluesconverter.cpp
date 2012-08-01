#include "dicomformattedValuesConverter.h"

#include "dicomvaluerepresentationconverter.h"
#include <QStringList>

namespace udg {

DICOMFormattedValuesConverter::DICOMFormattedValuesConverter()
{
}

DICOMFormattedValuesConverter::~DICOMFormattedValuesConverter()
{
}

QList<WindowLevel> DICOMFormattedValuesConverter::parseWindowLevelValues(const QString &windowWidthString, const QString &windowCenterString,
    const QString &explanationString)
{
    QList<WindowLevel> windowLevelList;

    // Convertim els valors d'string a vectors de doubles i comprovem que estiguin el format correcte
    // En cas que no sigui així, retornem una llista buida
    bool ok;
    QVector<double> windowWidthArray = DICOMValueRepresentationConverter::decimalStringToDoubleVector(windowWidthString, &ok);
    if (!ok)
    {
        return windowLevelList;
    }
    
    QVector<double> windowCenterArray = DICOMValueRepresentationConverter::decimalStringToDoubleVector(windowCenterString, &ok);
    if (!ok)
    {
        return windowLevelList;
    }

    // Per crear una llista vàlida hem de tenir el mateix nombre de window width i window level
    // En cas que no sigui així, retornem una llista buida
    if (windowWidthArray.count() != windowCenterArray.count())
    {
        return windowLevelList;
    }
    
    // Un cop comprovades les condicions, procedim a crear el llistat. La llista de descripcions pot estar buida ja que és opcional
    // Només s'afegiran les descripcions que hi hagi dins del rang determinat pels valors trobats de window level
    QStringList explanationsList = explanationString.split(DICOMValueRepresentationConverter::ValuesSeparator);

    int numberOfWindowLevels = windowWidthArray.count();
    for (int i = 0; i < numberOfWindowLevels; ++i)
    {
        WindowLevel newWindowLevel;

        newWindowLevel.setWidth(windowWidthArray.at(i));
        newWindowLevel.setLevel(windowCenterArray.at(i));

        if (i < explanationsList.count())
        {
            newWindowLevel.setName(explanationsList.at(i));
        }

        windowLevelList << newWindowLevel;
    }
    
    return windowLevelList;
}

}
