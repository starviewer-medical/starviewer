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

#include "dicomformattedvaluesconverter.h"

#include "dicomvaluerepresentationconverter.h"
#include "logging.h"
#include "mathtools.h"
#include "pixelspacing2d.h"
#include "voilut.h"

#include <QColor>
#include <QRegularExpression>

namespace {

/// Returns the base used to represent the data values contained in the given data string. The base may only be 16 or 10.
int getBase(const QString &data)
{
    if (data.contains(QRegularExpression("[a-f]", QRegularExpression::CaseInsensitiveOption)))
    {
        return 16;
    }
    else
    {
        return 10;
    }
}

/// Parses the given data string and returns a list of the integers represented in it.
QList<int> getValues(const QString &lutData)
{
    QStringList lutStringValues = lutData.split("\\");
    int base = getBase(lutData);
    QList<int> lutValues;

    foreach (const QString &stringValue, lutStringValues)
    {
        bool ok;
        int value = stringValue.toInt(&ok, base);
        lutValues.append(value);

        if (!ok)
        {
            DEBUG_LOG(QString("Error while parsing %1 as an int in base %2. Result: %3").arg(stringValue).arg(base).arg(value));
            WARN_LOG(QString("Error while parsing %1 as an int in base %2. Result: %3").arg(stringValue).arg(base).arg(value));
        }
    }

    return lutValues;
}

/// Returns the maximum value in the given list.
int getMaximum(const QList<int> &values)
{
    int maximum = 0;

    foreach (int value, values)
    {
        maximum = qMax(value, maximum);
    }

    return maximum;
}

}

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
        newWindowLevel.setCenter(windowCenterArray.at(i));

        if (i < explanationsList.count())
        {
            newWindowLevel.setName(explanationsList.at(i));
        }

        windowLevelList << newWindowLevel;
    }
    
    return windowLevelList;
}

VoiLut DICOMFormattedValuesConverter::parseVoiLut(const QString &lutDescriptor, const QString &lutExplanation, const QString &lutData)
{
    TransferFunction lut;
    lut.setName(lutExplanation);

    QList<int> lutValues = getValues(lutData);
    int dataMaximum = getMaximum(lutValues);
    // The maximum value of the LUT should be a power of two minus one
    double lutMaximum = MathTools::roundUpToPowerOf2(dataMaximum) - 1;
    int firstValueMapped = lutDescriptor.split("\\").at(1).toInt();
    int inputValue = firstValueMapped;

    foreach (int value, lutValues)
    {
        double outputValue = value / lutMaximum;
        lut.setColor(inputValue, outputValue, outputValue, outputValue);
        inputValue++;
    }

    lut = lut.simplify();

    // Explicitly set first and last points
    lut.setColor(0.0, lut.getColor(firstValueMapped));
    lut.setColor(lutMaximum, lut.getColor(inputValue - 1));
    lut.setOpacity(0.0, 1.0);

    return lut;
}

PixelSpacing2D DICOMFormattedValuesConverter::parsePixelSpacing(const QString &pixelSpacingString)
{
    QStringList values = pixelSpacingString.split(DICOMValueRepresentationConverter::ValuesSeparator);

    if (values.size() == 2)
    {
        // Pixel spacing is rowSpacing\columnSpacing -> ySpacing\xSpacing
        double rowSpacing = values.at(0).toDouble();
        double columnSpacing = values.at(1).toDouble();
        return PixelSpacing2D(columnSpacing, rowSpacing);
    }
    else
    {
        return PixelSpacing2D();
    }
}

}
