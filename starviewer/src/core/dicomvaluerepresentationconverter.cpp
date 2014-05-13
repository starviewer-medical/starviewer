/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "dicomvaluerepresentationconverter.h"

#include <QStringList>
#include <QLocale>
#include <QVector2D>

#include "mathtools.h"

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

QTime DICOMValueRepresentationConverter::timeToQTime(const QString &timeString)
{
    // Allowed format is HHMMSS.FFFFFF, where .FFFFFF is optional (and up to 6 digits)
    // We treat separately the microseconds from the rest
    QStringList splittedTime = timeString.trimmed().split(".");
    QString simpleTime = splittedTime.first().trimmed();
    QTime time;
    switch (simpleTime.size())
    {
        case 2:
            // Should be HH format, range 00-23
            time = QTime(simpleTime.toInt(), 0);
            break;

        case 4:
            // Should be HHMM format, range 00-23, 00-59
            time.setHMS(simpleTime.left(2).toInt(), simpleTime.right(2).toInt(), 0);
            break;

        case 6:
            {
                // Should be HHMMSS format, range 00-23, 00-59, 00-60 (leap second)
                int seconds = simpleTime.right(2).toInt();
                if (seconds == 60)
                {
                    // To prevent create an invalid QTime, because DICOM accepts leap seconds on its time format
                    seconds = 59;
                }

                simpleTime.chop(2);
                int minutes = simpleTime.right(2).toInt();
            
                simpleTime.chop(2);
                int hours = simpleTime.toInt();
            
                time.setHMS(hours, minutes, seconds);
            }
            break;

        default:
            break;
    }

    if (time.isValid())
    {
        if (splittedTime.count() == 2)
        {
            // Maximum QTime resolution is milliseconds
            int milliseconds = splittedTime.last().left(3).toInt();
            time = time.addMSecs(milliseconds);
        }
    }
    
    return time;
}

} // End namespace udg
