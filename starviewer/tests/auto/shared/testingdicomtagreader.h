#ifndef TESTINGDICOMTAGREADER_H
#define TESTINGDICOMTAGREADER_H

#include "dicomtagreader.h"

#include "dicomtag.h"

#include <QHash>
#include <QVariant>

using namespace udg;

namespace udg{

class DICOMSequenceAttribute;

uint qHash(const DICOMTag &tag);

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
    virtual DICOMSequenceAttribute* getSequenceAttribute(const DICOMTag &sequenceTag, DICOMTagReader::ReturnValueOfTags returnValueOfTags = AllTags) const;

    /// Afegeix la parella de tag i valor al hash.
    void addTag(const DICOMTag &tag, const QVariant &value = QVariant());

    /// Afegeix la parella de tag i valor al hash.
    void addSequence(DICOMSequenceAttribute *sequence);

private:

    QHash<DICOMTag, QVariant> m_source;
    QHash<DICOMTag, DICOMSequenceAttribute*> m_sequences;
};

}

#endif // TESTINGDICOMTAGREADER_H
