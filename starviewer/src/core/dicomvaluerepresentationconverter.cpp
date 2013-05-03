#include "dicomvaluerepresentationconverter.h"

#include <QStringList>
#include <QLocale>
#include <QVector2D>

namespace udg {

/// Definició de les constants
const QString DICOMValueRepresentationConverter::ValuesSeparator("\\");

DICOMValueRepresentationConverter::DICOMValueRepresentationConverter()
{
}

DICOMValueRepresentationConverter::~DICOMValueRepresentationConverter()
{
}

QVector<double> DICOMValueRepresentationConverter::decimalStringToDoubleVector(const QString &decimalString, bool *ok)
{
    // TODO Mirar si dcmtk o gdcm incorporen alguna classe que faci la comprovació estricta dels valors segons el DICOM
    // A dcmtk s'ha trobat la classe DcmDecimalString, però no es veu clar que pugui servir per aquest propòsit
    // De moment acceptarem com a bona la conversió dels valors decimals que s'ens doni amb QLocale::C
    
    // Per la definició de Decimal String (DS) veure PS 3.5 - Table 6.2-1 DICOM VALUE REPRESENTATIONS

    QVector<double> result;
    QStringList valuesList = decimalString.split(ValuesSeparator);
    
    // Creem un localOk per controlar la validació internament
    bool localOk;
    
    if (decimalString.isEmpty())
    {
        // Si l'string és buit, retornarem cert i el vector retornat serà buit
        localOk = true;
    }
    else
    {
        QLocale cLocale(QLocale::C);
        double convertedValue;
        foreach (const QString &value, valuesList)
        {
            // Acceptarem com a valors decimals bons els que ens retorni QLocale::toDouble() amb QLocale::C
            convertedValue = cLocale.toDouble(value, &localOk);
            if (localOk == false)
            {
                break;
            }
            else
            {
                result << convertedValue;
            }
        }
    }

    // Si ens han passat un punter no nul d'ok, li assignem l'ok local que hem calculat
    if (ok != 0)
    {
        *ok = localOk;
    }
    
    // Si la validació ha fallat, retornem un vector buit, sinó el vector amb els valors convertits
    if (localOk == false)
    {
        return QVector<double>();
    }
    else
    {
        return result;
    }
}

QVector2D DICOMValueRepresentationConverter::decimalStringTo2DDoubleVector(const QString &decimalString, bool *ok)
{
    QVector<double> vectorValue = DICOMValueRepresentationConverter::decimalStringToDoubleVector(decimalString, ok);
    
    bool localOk = true;
    QVector2D vector2DValue(0, 0);
    
    // If a not-null ok pointer is given, we set computed local with ok's value
    if (ok != 0)
    {
        localOk = *ok;
    }
    
    // If first conversion is not ok, we return a default-constructed value, just for safety.
    if (localOk)
    {
        // Check if it's a real 2D vector
        if (vectorValue.size() == 2)
        {
            vector2DValue.setX(vectorValue.at(0));
            vector2DValue.setY(vectorValue.at(1));
            localOk = true;
        }
        else
        {
            localOk = false;
        }
    }

    if (ok != 0)
    {
        *ok = localOk;
    }
        
    return vector2DValue;
}

} // End namespace udg
