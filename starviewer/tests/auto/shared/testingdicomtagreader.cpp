#include "testingdicomtagreader.h"


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

void TestingDICOMTagReader::addTag(const DICOMTag &tag, const QVariant &value)
{
    m_source[tag] = value;
}

}
