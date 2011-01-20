/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDICOMTAG_H
#define UDGDICOMTAG_H

#include <QString>

namespace udg {

/**
   Classe per representar un tag DICOM.
   Un tag està format per enters, el group i l'element. Ex: (0020,0022)

   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
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

    /// Comparar si són iguals evaluant el group i element
    bool operator==(const DICOMTag &tag);

    /// Comparar si són diferents evaluant el group i element
    bool operator!=(const DICOMTag &tag);

private:
    unsigned int m_group;
    unsigned int m_element;
    QString m_name;
};

}

#endif // DICOMTAG_H
