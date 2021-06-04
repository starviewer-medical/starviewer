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

#ifndef UDGGENERICVOLUMEDISPLAYUNITHANDLER_H
#define UDGGENERICVOLUMEDISPLAYUNITHANDLER_H

#include <QObject>

class vtkImageSlice;
class vtkImageStack;

namespace udg {

class TransferFunctionModel;
class Volume;
class VolumeDisplayUnit;

/**
    Base class to handle single or multiple VolumeDisplayUnits.
    We can set the input(s) via setInput() (single) or setInputs() (multiple).
    The virtual method getMaximumNumberOfInputs() tells how many inputs it can handle.
    This method should be reimplemented by any subclass that has a limit on the number of inputs it can handle.
    It will also determine how many inputs are accepted by setInputs(). By default, there is no limit on the quantity of inputs.
    There will be also a display unit to be considered the main one, which always will be on the first position of the list.
    If there are any specific criteria to put a certain display unit as the main one, different than the order that it was set on setInputs(), 
    this should be implemented in the virtual method updateMainDisplayUnitIndex().
 */
class GenericVolumeDisplayUnitHandler : public QObject {

    Q_OBJECT

public:
    GenericVolumeDisplayUnitHandler(QObject *parent = nullptr);
    virtual ~GenericVolumeDisplayUnitHandler();

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

    /// Returns the prop that should be added to the renderer.
    virtual vtkImageSlice* getImageProp() const;

    /// Returns the transfer function model containing the available transfer functions for the volume display units of this handler.
    TransferFunctionModel* getTransferFunctionModel() const;

protected:
    /// Sets up the default transfer functions for the required inputs.
    /// Should be reimplemented by the required subclasses that need to apply specific transfer functions on their inputs
    virtual void setupDefaultTransferFunctions(){};

    /// To be reimplemented in case a concrete display unit that meets some conditions should be put explicitly on first place
    virtual void updateMainDisplayUnitIndex(){};

private:
    /// Clears all the existing display units
    void removeDisplayUnits();

    /// Adds a new display unit with the given input
    void addDisplayUnit(Volume *input);

    /// Adds the given imageSlice to the stack.
    /// If the given imageSlice is itself a stack, all its images will be processed recursively and added to the stack.
    void addImageSliceToStack(vtkImageSlice *imageSlice);

    /// Removes the given imageSlice from the stack.
    /// If the given imageSlice is itself a stack, all its images will be processed recursively and removed from the stack.
    void removeImageSliceFromStack(vtkImageSlice *imageSlice);

    /// Sets up the display units once created
    void setupDisplayUnits();

    /// Updates the layer number of all display units to match their position in the list.
    void updateLayerNumbers();

    /// Sets up the default opacities of the actors
    void setupDefaultOpacities();

    /// Initializes the transfer functions of the display units
    void initializeTransferFunctions();

private slots:
    /// Rebuilds the image stack from scratch.
    void rebuildImageStack();

protected:
    /// The list of the created display units
    QList<VolumeDisplayUnit*> m_displayUnits;

    /// Used to merge several volume display units.
    vtkImageStack *m_imageStack;

    /// Holds the transfer functions available to this handler.
    TransferFunctionModel *m_transferFunctionModel;
};

} // End namespace udg

#endif
