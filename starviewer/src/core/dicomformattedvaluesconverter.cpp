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

    // Per evitar que hi hagi problemes si les llistes no tenen la mateixa llargada tenim en compte la més curta
    int maximumNumberOfWindowLevels = windowWidthArray.size() < windowCenterArray.size() ? windowWidthArray.size(): windowCenterArray.size();
    // La llista de descripcions pot estar buida ja que és opcional
    // Només s'afegiran les descripcions que hi hagi dins del rang determinat pels valors trobats de window level
    QStringList explanationsList = explanationString.split(DICOMValueRepresentationConverter::ValuesSeparator);
    
    for (int i = 0; i < maximumNumberOfWindowLevels; ++i)
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
