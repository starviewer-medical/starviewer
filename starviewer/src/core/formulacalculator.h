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

#ifndef UDGFORMULACALCULATOR_H
#define UDGFORMULACALCULATOR_H

namespace udg {

class Image;
class DICOMTagReader;

/**
    Class to combine with Formula to compute a formula from the data provided from an Image or DICOMTagReader.

    When all the required data to compute the formula is available from the data stored on Image, provided image will be used.
    When Image data is not enough, the needed data should be retrieved via DICOMTagReader.

    Inherited classes should take into account wether Image or DICOMTagReader is provided.

    TODO Add pure virtual method to tell wether the formula needs data which is not present on Image (thus, DICOMTagReader must be used)
    This method could be used on setDataSource(Image*) then load automatically the related DICOMTagReader of the image.
 */
class FormulaCalculator {
public:
    FormulaCalculator();
    virtual ~FormulaCalculator();

    /// Tells wether the formula can be computed or not with the provided data sources.
    virtual bool canCompute() = 0;

    /// Computes and returns the resulting value of running the formula with the given data
    /// If canCompute() returns false, the validity of the result is undefined.
    virtual double compute() = 0;

    /// Sets the data source of the formula
    void setDataSource(Image *image);
    void setDataSource(DICOMTagReader *tagReader);

protected:
    /// The possible data sources to compute the formula from
    Image *m_imageSource;
    DICOMTagReader *m_tagReaderSource;
};

} // End namespace udg

#endif
