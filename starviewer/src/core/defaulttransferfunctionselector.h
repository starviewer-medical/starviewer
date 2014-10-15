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

#ifndef DEFAULTTRANSFERFUNCTIONSELECTOR_H
#define DEFAULTTRANSFERFUNCTIONSELECTOR_H

class QString;

namespace udg {

class TransferFunctionModel;

/**
    This class has methods to choose the default transfer function from a given TransferFunctionModel for a specific role.
 */
class DefaultTransferFunctionSelector {

public:
    /// Returns the default transfer function for PET.
    int getDefaultTransferFunctionForPET(const TransferFunctionModel *model) const;

    /// Returns the default transfer function for the PET volume in PET-CT.
    int getDefaultTransferFunctionForPETCT(const TransferFunctionModel *model) const;

    /// Returns the default transfer function for NM.
    int getDefaultTransferFunctionForNM(const TransferFunctionModel *model) const;

    /// Returns the default transfer function for the NM volume in SPECT-CT.
    int getDefaultTransferFunctionForNMCT(const TransferFunctionModel *model) const;

private:
    /// Returns the index of the transfer function by name. Returns -1 if not found.
    int getTransferFunctionIndexByName(const QString &name, const TransferFunctionModel *model) const;
};

}

#endif
