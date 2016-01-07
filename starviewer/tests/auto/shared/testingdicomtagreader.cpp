#include "testingdicomtagreader.h"

#include "dicomsequenceattribute.h"


namespace udg {

uint qHash(const DICOMTag &tag)
{
    return qHash(qMakePair(tag.getGroup(), tag.getElement()));
}

}
namespace testing {

TestingDICOMTagReader::TestingDICOMTagReader()
{
}

TestingDICOMTagReader::~TestingDICOMTagReader()
{
}

bool TestingDICOMTagReader::tagExists(const DICOMTag &tag) const
{
    return m_source.contains(tag);
}

QString TestingDICOMTagReader::getValueAttributeAsQString(const DICOMTag &tag) const
{
    return m_source.value(tag, QString()).toString();
}

DICOMSequenceAttribute *TestingDICOMTagReader::getSequenceAttribute(const DICOMTag &sequenceTag, DICOMTagReader::ReturnValueOfTags returnValueOfTags) const
{
    return m_sequences[sequenceTag];
}

void TestingDICOMTagReader::addTag(const DICOMTag &tag, const QVariant &value)
{
    m_source[tag] = value;
}

void TestingDICOMTagReader::addSequence(DICOMSequenceAttribute *sequence)
{
    m_sequences[sequence->getTag()] = sequence;
}

}
