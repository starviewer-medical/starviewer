#ifndef FILTEROUTPUT_H
#define FILTEROUTPUT_H

class vtkAlgorithmOutput;
class vtkImageData;

namespace udg {

class Filter;

/**
    This class represents the output of a Filter object.
    It has an associated Filter object and has methods to get the output in different forms.
 */
class FilterOutput {

public:
    /// Creates a FilterOutput associated to the given filter.
    explicit FilterOutput(Filter *filter);

    /// Returns the associated Filter object.
    Filter* getFilter() const;

    /// Returns the output in vtkAlgorithmOutput form, useful to create a VTK pipeline connection.
    vtkAlgorithmOutput* getVtkAlgorithmOutput();

    /// Returns the output in vtkImageData form. Updates the filter before returning the data.
    vtkImageData* getVtkImageData();

private:
    /// Filter object from which to get the output.
    Filter *m_filter;

};

}

#endif
