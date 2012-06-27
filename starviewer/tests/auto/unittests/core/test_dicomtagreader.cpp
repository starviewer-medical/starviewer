#include "autotest.h"

#include "dicomtagreader.h"
#include "dicomvalueattribute.h"

#include <dcdatset.h>
#include <dcdeftag.h>
#include <dcsequen.h>

using namespace udg;

class test_DICOMTagReader : public QObject {
Q_OBJECT
private slots:
    void getValueAttribute_ReturnsNull_data();
    void getValueAttribute_ReturnsNull();
    
    void getValueAttribute_ReturnsExpectedValues_data();
    void getValueAttribute_ReturnsExpectedValues();
};

Q_DECLARE_METATYPE(DcmDataset*)
Q_DECLARE_METATYPE(DICOMValueAttribute*)
Q_DECLARE_METATYPE(DICOMTag)

void test_DICOMTagReader::getValueAttribute_ReturnsNull_data()
{
    QTest::addColumn<DcmDataset*>("dataSet");
    QTest::addColumn<DICOMTag>("tag");
    
    DcmDataset *nullDataSet = 0;
    QTest::newRow("Empty dataset") << nullDataSet << DICOMPixelSpacing;
    
    DcmItem *sequenceItem = new DcmItem;
    Sint16 values[3] = {1024, 0, 10};
    OFCondition ok = sequenceItem->putAndInsertSint16Array(DCM_LUTDescriptor, values, 3);   
    DcmSequenceOfItems *sequence = new DcmSequenceOfItems(DCM_ModalityLUTSequence);
    ok = sequence->append(sequenceItem);
    DcmDataset *dataset = new DcmDataset;
    ok = dataset->insert(sequence);

    QTest::newRow("Tag is a sequence") << dataset << DICOMModalityLUTSequence;
    QTest::newRow("Tag is an attribute inside a sequence") << static_cast<DcmDataset*>(dataset->clone()) << DICOMLUTDescriptor;
    QTest::newRow("Tag does not exist in dataset") << static_cast<DcmDataset*>(dataset->clone()) << DICOMPixelSpacing;
}

void test_DICOMTagReader::getValueAttribute_ReturnsNull()
{
    QFETCH(DcmDataset*, dataSet);
    QFETCH(DICOMTag, tag);

    DICOMTagReader tagReader;
    tagReader.setDcmDataset("", dataSet);

    QVERIFY(!tagReader.getValueAttribute(tag));
}

void test_DICOMTagReader::getValueAttribute_ReturnsExpectedValues_data()
{
    QTest::addColumn<DcmDataset*>("dataSet");
    QTest::addColumn<DICOMTag>("tag");
    QTest::addColumn<DICOMValueAttribute*>("expectedValue");

    DcmDataset *dataset = new DcmDataset;
    OFCondition ok = dataset->putAndInsertString(DCM_PixelSpacing, "1.5\\1.2");
    dataset->putAndInsertUint16(DCM_Rows, 10);
    dataset->putAndInsertString(DCM_PhotometricInterpretation, "MONOCHROME1");
    dataset->putAndInsertString(DCM_PatientName, "JOHN^DOE");
    dataset->putAndInsertString(DCM_StudyDate, "19991231");

    DICOMValueAttribute *pixelSpacingAttribute = new DICOMValueAttribute;
    pixelSpacingAttribute->setTag(DICOMPixelSpacing);
    pixelSpacingAttribute->setValue(QString("1.5\\1.2"));
    QTest::newRow("Pixel Spacing (DS VR)") << dataset << DICOMPixelSpacing << pixelSpacingAttribute;

    DICOMValueAttribute *rowsAttribute = new DICOMValueAttribute;
    rowsAttribute->setTag(DICOMRows);
    rowsAttribute->setValue(10);
    QTest::newRow("Rows (US VR)") << static_cast<DcmDataset*>(dataset->clone()) << DICOMRows << rowsAttribute;

    DICOMValueAttribute *photometricInterpretationAttribute = new DICOMValueAttribute;
    photometricInterpretationAttribute->setTag(DICOMPhotometricInterpretation);
    photometricInterpretationAttribute->setValue(QString("MONOCHROME1"));
    QTest::newRow("PhotometricInterpretation (CS VR)") << static_cast<DcmDataset*>(dataset->clone()) << DICOMPhotometricInterpretation 
        << photometricInterpretationAttribute;

    DICOMValueAttribute *patientsNameAttribute = new DICOMValueAttribute;
    patientsNameAttribute->setTag(DICOMPatientsName);
    patientsNameAttribute->setValue(QString("JOHN^DOE"));
    QTest::newRow("PatientsName (PN VR)") << static_cast<DcmDataset*>(dataset->clone()) << DICOMPatientsName << patientsNameAttribute;

    DICOMValueAttribute *studyDateAttribute = new DICOMValueAttribute;
    studyDateAttribute->setTag(DICOMStudyDate);
    studyDateAttribute->setValue(QString("19991231"));
    QTest::newRow("StudyDate (DA VR)") << static_cast<DcmDataset*>(dataset->clone()) << DICOMStudyDate << studyDateAttribute;
}

void test_DICOMTagReader::getValueAttribute_ReturnsExpectedValues()
{
    QFETCH(DcmDataset*, dataSet);
    QFETCH(DICOMTag, tag);
    QFETCH(DICOMValueAttribute*, expectedValue);
    
    DICOMTagReader tagReader;
    tagReader.setDcmDataset("", dataSet);

    DICOMValueAttribute *returnValue = tagReader.getValueAttribute(tag);
    
    QVERIFY(returnValue);
    QCOMPARE(expectedValue->getValueAsByteArray(), returnValue->getValueAsByteArray());
}

DECLARE_TEST(test_DICOMTagReader)

#include "test_dicomtagreader.moc"
