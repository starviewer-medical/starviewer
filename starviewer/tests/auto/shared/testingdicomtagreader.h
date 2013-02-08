#ifndef TESTINGDICOMTAGREADER_H
#define TESTINGDICOMTAGREADER_H

#include "dicomtagreader.h"

#include "dicomtag.h"

#include <QHash>
#include <QVariant>

using namespace udg;

namespace {

uint qHash(const DICOMTag &tag)
{
    return qHash(qMakePair(tag.getGroup(), tag.getElement()));
}

}

namespace testing {

/**
    Versió del DICOMTagReader pels tests. Fa servir un QHash per guardar els tags en lloc d'accedir a un fitxer.
 */
class TestingDICOMTagReader : public udg::DICOMTagReader {

public:

    TestingDICOMTagReader();
    virtual ~TestingDICOMTagReader();

    virtual bool tagExists(const DICOMTag &tag) const;
    virtual QString getValueAttributeAsQString(const DICOMTag &tag) const;

    /// Afegeix la parella de tag i valor al hash.
    void addTag(const DICOMTag &tag, const QVariant &value = QVariant());

private:

    QHash<DICOMTag, QVariant> m_source;

};

}

#endif // TESTINGDICOMTAGREADER_H
