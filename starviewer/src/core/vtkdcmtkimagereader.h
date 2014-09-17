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

#ifndef VTKDCMTKIMAGEREADER_H
#define VTKDCMTKIMAGEREADER_H

#include <stdexcept>

#include <vtkImageReader2.h>

#include <QList>

class DicomImage;

namespace udg {

class DICOMTagReader;

/**
    VTK image reader that uses DCMTK to read DICOM files.
 */
class VtkDcmtkImageReader : public vtkImageReader2 {

public:

    class CantReadImageException;

public:

    vtkTypeMacro(VtkDcmtkImageReader, vtkImageReader2);

    /// Create an object with Debug turned off, modified time initialized to zero, and reference counting on.
    static VtkDcmtkImageReader *New();

    /// Methods invoked by print to print information about the object including superclasses. Typically not called by the user (use Print() instead) but used
    /// in the hierarchical print process to combine the output of several classes.
    virtual void PrintSelf(std::ostream &os, vtkIndent indent);

    /// Sets the list of frame numbers in the order they must be read from a multiframe file. No need to specify for single-frame files.
    void setFrameNumbers(const QList<int> &frameNumbers);

protected:

    VtkDcmtkImageReader();

    /// Reads image information from the first file and fills data properties (extent, spacing, origin).
    virtual int RequestInformation(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);
    /// Reads image data from the file(s).
    virtual int RequestData(vtkInformation *request, vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:

    VtkDcmtkImageReader(const VtkDcmtkImageReader &);   // Not implemented
    void operator=(const VtkDcmtkImageReader &);        // Not implemented

    /// Reads image information from the given file.
    bool readInformation(const QString &filename);
    /// Fills data extent from the given DICOM tag reader.
    void readExtent(const DICOMTagReader &dicomTagReader);
    /// Fills data spacing from the given DICOM tag reader.
    void readSpacing(const DICOMTagReader &dicomTagReader);
    /// Fills X and Y data spacing from the given DICOM tag reader.
    void readPixelSpacing(const DICOMTagReader &dicomTagReader);
    /// Fills Z data spacing from the given DICOM tag reader.
    void readSpacingBetweenSlices(const DICOMTagReader &dicomTagReader);
    /// Fills data origin from the given DICOM tag reader.
    void readOrigin(const DICOMTagReader &dicomTagReader);
    /// Reads rescale values from the DICOM per-frame functional groups sequence, if present.
    void readPerFrameRescale(const DICOMTagReader &dicomTagReader);
    /// Decides the appropiate initial scalar type for the image data according to the range of values contained in one frame of the image and sets the number
    /// of scalar components. The scalar type may change to a bigger one while reading all the data. Returns false in case of error, if it can't decide the
    /// scalar type.
    bool decideInitialScalarTypeAndNumberOfComponents();
    /// Decides the appropiate initial scalar type for the image data according to given image file and sets the number of scalar components.
    /// Returns false in case of error, if it can't decide the scalar type.
    bool decideInitialScalarTypeAndNumberOfComponents(const char *filename);

    /// Loads image data from the file(s) for the given update extent.
    bool loadData(int updateExtent[6]);
    /// Loads image data from a single frame file into the given buffer.
    void loadSingleFrameFile(const char *filename, void *buffer);
    /// Loads image data from a multiframe file, for the given update extent, into the given buffer.
    void loadMultiframeFile(const char *filename, void *buffer, int updateExtent[6]);
    /// Copies the image data stored in the given dicom image into the given buffer.
    void copyDcmtkImageToBuffer(void *buffer, DicomImage &dicomImage);

private:

    /// Struct that represents a DICOM rescale, with intercept and slope values.
    struct Rescale
    {
        double intercept;
        double slope;
    };

    /// List of frame numbers in the order they must be read from a multiframe file. Not used for single-frame files.
    QList<int> m_frameNumbers;

    /// True if reading a multiframe volume.
    bool m_isMultiframe;
    /// True if reading a multiframe volume and it has rescale defined in the per-frame functional groups sequence.
    bool m_hasPerFrameRescale;
    /// Stores the rescale values defined in the per-frame functional groups sequence.
    QList<Rescale> m_perFrameRescale;
    /// True if the volume is monochrome.
    bool m_isMonochrome;
    /// Size of an image frame in bytes.
    size_t m_frameSize;
    /// Maximum voxel value found in the image data.
    double m_maximumVoxelValue;
    /// If it's true, a float scalar type will be used.
    bool m_needsFloatScalarType;

};

/// This exception is thrown when the image can't be loaded.
class VtkDcmtkImageReader::CantReadImageException : public std::runtime_error {

public:

    explicit CantReadImageException(const std::string& what);

};

} // namespace udg

#endif // VTKDCMTKIMAGEREADER_H
