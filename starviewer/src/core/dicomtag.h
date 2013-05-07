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
