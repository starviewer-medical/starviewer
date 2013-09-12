#ifndef UDGGENERICVOLUMEDISPLAYUNITHANDLER_H
#define UDGGENERICVOLUMEDISPLAYUNITHANDLER_H

#include <QList>

namespace udg {

class Volume;
class VolumeDisplayUnit;

/**
    Base class to handle single or multiple VolumeDisplayUnits.
    We can set the input(s) via setInput() (single) or setInputs() (multiple).
    The virtual method getMaximumNumberOfInputs() tells how many inputs it can handle.
    This method should be reimplemented by any subclass that has a limit on the number of inputs it can handle.
    It will also determine how many inputs are accepted by setInputs(). By default, there is no limit on the quantity of inputs.
    It can also handle which input should be considered the main one. By default it will be the first provided. If there's some
    different criteria to choose which must be considered the main input, this should be implemented in the virtual method updateMainDisplayUnitIndex().
 */
class GenericVolumeDisplayUnitHandler {
public:
    GenericVolumeDisplayUnitHandler();
    ~GenericVolumeDisplayUnitHandler();

    /// Sets single input
    void setInput(Volume *input);

    /// Sets multiple inputs. If the number of given inputs is greater than getMaximumNumberOfInputs(),
    /// only the first inputs till getMaximumNumberOfInputs() will be processed
    void setInputs(QList<Volume*> inputs);

    /// Returns true if there's at least one input, false otherwise
    bool hasInput() const;

    /// Returns the number of inputs
    int getNumberOfInputs() const;

    VolumeDisplayUnit* getMainVolumeDisplayUnit() const;
    VolumeDisplayUnit* getVolumeDisplayUnit(int i) const;
    QList<VolumeDisplayUnit*> getVolumeDisplayUnitList() const;

    /// Returns the maximum number of inputs supported by the handler
    virtual int getMaximumNumberOfInputs() const;

protected:
    /// Sets up the default transfer functions for the required inputs.
    /// Should be reimplemented by the required subclasses that need to apply specific transfer functions on their inputs
    virtual void setupDefaultTransferFunctions(){};

    /// To be reimplemented in case main display unit could be different than the first one
    virtual void updateMainDisplayUnitIndex(){};

private:
    /// Initializes the class in the constructor
    void initialize();

    /// Clears all the existing display units
    void removeDisplayUnits();

    /// Adds a new display unit with the given input
    void addDisplayUnit(Volume *input);

    /// Sets up the display units once created
    void setupDisplayUnits();

    /// Sets up the default opacities of the actors
    void setupDefaultOpacities();

    /// Initializes the transfer functions of the display units
    void initializeTransferFunctions();

protected:
    /// The list of the created display units
    QList<VolumeDisplayUnit*> m_displayUnits;

    /// By default, the main display unit will be the first one
    int m_mainDisplayUnitIndex;
};

} // End namespace udg

#endif
