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

#ifndef UDGDICOMTAG_H
#define UDGDICOMTAG_H

#include <QString>

namespace udg {

/**
    Classe per representar un tag DICOM.
    Un tag està format per enters, el group i l'element. Ex: (0020,0022)
  */
class DICOMTag {

public:
    DICOMTag();
    DICOMTag(unsigned int group, unsigned int element);
    DICOMTag(DICOMTag *tag);

    ~DICOMTag();

    /// Assignar/Obtenir el valor del group
    void setGroup(unsigned int group);
    unsigned int getGroup() const;

    /// Assignar/Obtenir el valor de element
    void setElement(unsigned int element);
    unsigned int getElement() const;

    /// Assignar/Obtenir el nom del tag.
    void setName(const QString &name);
    QString getName() const;

    /// Retorna el tag en forma de text.
    QString getKeyAsQString() const;

    /// Returns true if group and element are equal.
    bool operator==(const DICOMTag &tag) const;
    /// Returns true if group or element are different.
    bool operator!=(const DICOMTag &tag) const;
    /// Returns true if this' group is smaller than tag's group or if groups are equal and this' element is smaller than tag's element.
    bool operator<(const DICOMTag &tag) const;

private:
    unsigned int m_group;
    unsigned int m_element;
    QString m_name;
};

}

#endif // DICOMTAG_H
