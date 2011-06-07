#include "dicomvaluerepresentationconverter.h"

#include <QStringList>
#include <QLocale>

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

} // End namespace udg
