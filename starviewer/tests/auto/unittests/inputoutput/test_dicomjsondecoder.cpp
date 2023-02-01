/*@
    "name": "test_DicomJsonDecoder",
    "requirements": ["archive.wado.query"]
 */

#include "autotest.h"
#include "dicomjsondecoder.h"

#include "image.h"
#include "patient.h"
#include "series.h"
#include "study.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

using namespace udg;

namespace {

void comparePatientsAndStudies(const Patient *patient, const Patient *expectedPatient)
{
    QCOMPARE(patient == nullptr, expectedPatient == nullptr);

    if (patient)
    {
        QCOMPARE(patient->getFullName(), expectedPatient->getFullName());
        QCOMPARE(patient->getID(), expectedPatient->getID());
        QCOMPARE(patient->getBirthDate(), expectedPatient->getBirthDate());

        const QList<Study*> studies = patient->getStudies();
        const QList<Study*> expectedStudies = expectedPatient->getStudies();

        QCOMPARE(studies.size(), expectedStudies.size());

        for (int i = 0; i < studies.size(); i++)
        {
            const Study *study = studies[i];
            const Study *expectedStudy = expectedStudies[i];

            QCOMPARE(study->getPatientAge(), expectedStudy->getPatientAge());
            QCOMPARE(study->getInstanceUID(), expectedStudy->getInstanceUID());
            QCOMPARE(study->getID(), expectedStudy->getID());
            QCOMPARE(study->getDate(), expectedStudy->getDate());
            QCOMPARE(study->getTime(), expectedStudy->getTime());
            QCOMPARE(study->getDescription(), expectedStudy->getDescription());
            QCOMPARE(study->getModalities(), expectedStudy->getModalities());
            QCOMPARE(study->getAccessionNumber(), expectedStudy->getAccessionNumber());
            QCOMPARE(study->getReferringPhysiciansName(), expectedStudy->getReferringPhysiciansName());
            QCOMPARE(study->getInstitutionName(), expectedStudy->getInstitutionName());
            QCOMPARE(study->getDICOMSource(), expectedStudy->getDICOMSource());
        }
    }
}

void compareSeries(const Series *series, const Series *expectedSeries)
{
    QCOMPARE(series == nullptr, expectedSeries == nullptr);

    if (series)
    {
        QCOMPARE(series->getInstanceUID(), expectedSeries->getInstanceUID());
        QCOMPARE(series->getSeriesNumber(), expectedSeries->getSeriesNumber());
        QCOMPARE(series->getDescription(), expectedSeries->getDescription());
        QCOMPARE(series->getProtocolName(), expectedSeries->getProtocolName());
        QCOMPARE(series->getDate(), expectedSeries->getDate());
        QCOMPARE(series->getTime(), expectedSeries->getTime());
        QCOMPARE(series->getModality(), expectedSeries->getModality());
        QCOMPARE(series->getPerformedProcedureStepStartDate(), expectedSeries->getPerformedProcedureStepStartDate());
        QCOMPARE(series->getPerformedProcedureStepStartTime(), expectedSeries->getPerformedProcedureStepStartTime());
        QCOMPARE(series->getRequestedProcedureID(), expectedSeries->getRequestedProcedureID());
        QCOMPARE(series->getScheduledProcedureStepID(), expectedSeries->getScheduledProcedureStepID());
        QCOMPARE(series->getDICOMSource(), expectedSeries->getDICOMSource());
    }
}

void compareInstances(const Image *instance, const Image *expectedInstance)
{
    QCOMPARE(instance == nullptr, expectedInstance == nullptr);

    if (instance)
    {
        QCOMPARE(instance->getSOPInstanceUID(), expectedInstance->getSOPInstanceUID());
        QCOMPARE(instance->getInstanceNumber(), expectedInstance->getInstanceNumber());
        QCOMPARE(instance->getDICOMSource(), expectedInstance->getDICOMSource());
    }
}

}

class test_DicomJsonDecoder : public QObject
{
    Q_OBJECT

private slots:
    void getStudies_ReturnsExpectedValueAndErrors_data();
    void getStudies_ReturnsExpectedValueAndErrors();

    void getSeries_1_ReturnsExpectedValueAndErrors_data();
    void getSeries_1_ReturnsExpectedValueAndErrors();

    void getInstances_ReturnsExpectedValueAndErrors_data();
    void getInstances_ReturnsExpectedValueAndErrors();

    void getRootArray_ReturnsExpectedValueAndErrors_data();
    void getRootArray_ReturnsExpectedValueAndErrors();

    void getStudy_ReturnsExpectedValueAndErrors_data();
    void getStudy_ReturnsExpectedValueAndErrors();

    void getSeries_2_ReturnsExpectedValueAndErrors_data();
    void getSeries_2_ReturnsExpectedValueAndErrors();

    void getInstance_ReturnsExpectedValueAndErrors_data();
    void getInstance_ReturnsExpectedValueAndErrors();

    void getTagValueAsString_ReturnsExpectedValueAndErrors_data();
    void getTagValueAsString_ReturnsExpectedValueAndErrors();

    void getTagValueAsStringList_ReturnsExpectedValue_data();
    void getTagValueAsStringList_ReturnsExpectedValue();

    void getExpectedType_ReturnsExpectedValueAndErrors_data();
    void getExpectedType_ReturnsExpectedValueAndErrors();

    void getValueArray_ReturnsExpectedValueAndErrors_data();
    void getValueArray_ReturnsExpectedValueAndErrors();

    void getArrayAsStringList_ReturnsExpectedValueAndErrors_data();
    void getArrayAsStringList_ReturnsExpectedValueAndErrors();
};

Q_DECLARE_METATYPE(Patient*);
Q_DECLARE_METATYPE(Series*);
Q_DECLARE_METATYPE(Image*)
Q_DECLARE_METATYPE(PacsDevice);
Q_DECLARE_METATYPE(DicomJsonDecoder::JsonDataType)

void test_DicomJsonDecoder::getStudies_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QByteArray>("jsonText");
    QTest::addColumn<QList<Patient*>>("expectedResult");
    QTest::addColumn<bool>("expectedToHaveErrors");

    QTest::newRow("bad json") << QByteArray("·= aç02 3'0") << QList<Patient*>{} << true;

    {
        QByteArray jsonText(R"([
                                {
                                    "00100010": {
                                        "vr": "PN",
                                        "Value": [
                                            {
                                                "Alphabetic": "Son^Goku",
                                                "Ideographic": "孫^悟空"
                                            }
                                        ]
                                    },
                                    "00100020": {
                                        "vr": "LO",
                                        "Value": ["GOKU"]
                                    },
                                    "0020000D": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00081030": {
                                        "vr": "LO",
                                        "Value": ["aaa"]
                                    }
                                },
                                {
                                    "00100010": {
                                        "vr": "PN",
                                        "Value": [
                                            {
                                                "Alphabetic": "Son^Goku",
                                                "Ideographic": "孫^悟空"
                                            }
                                        ]
                                    },
                                    "00100020": {
                                        "vr": "LO",
                                        "Value": ["GOKU"]
                                    },
                                    "0020000D": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.2"]
                                    },
                                    "00081030": {
                                        "vr": "LO",
                                        "Value": ["bbb"]
                                    }
                                }
                               ])");
        Patient *patient1 = new Patient();
        patient1->setFullName("Son^Goku=孫^悟空");
        patient1->setID("GOKU");
        Study *study1 = new Study();
        study1->setInstanceUID("1.2.3.4.1");
        study1->setDescription("aaa");
        patient1->addStudy(study1);
        Patient *patient2 = new Patient();
        patient2->setFullName("Son^Goku=孫^悟空");
        patient2->setID("GOKU");
        Study *study2 = new Study();
        study2->setInstanceUID("1.2.3.4.2");
        study2->setDescription("bbb");
        patient2->addStudy(study2);
        QTest::newRow("everything ok") << jsonText << QList<Patient*>{patient1, patient2} << false;
    }

    {
        QByteArray jsonText(R"([
                                {
                                    "00100010": {
                                        "vr": "PN",
                                        "Value": [
                                            {
                                                "Alphabetic": "Son^Goku",
                                                "Ideographic": "孫^悟空"
                                            }
                                        ]
                                    },
                                    "00100020": {
                                        "vr": "LO",
                                        "Value": ["GOKU"]
                                    },
                                    "0020000D": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00081030": {
                                        "vr": "LO",
                                        "Value": ["aaa"]
                                    }
                                },
                                "...",
                                {
                                    "00100010": {
                                        "vr": "PN",
                                        "Value": [
                                            {
                                                "Alphabetic": "Son^Goku",
                                                "Ideographic": "孫^悟空"
                                            }
                                        ]
                                    },
                                    "00100020": {
                                        "vr": "LO",
                                        "Value": ["GOKU"]
                                    },
                                    "0020000D": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.3"]
                                    },
                                    "00081030": {
                                        "vr": "LO",
                                        "Value": ["ccc"]
                                    }
                                }
                               ])");
        Patient *patient1 = new Patient();
        patient1->setFullName("Son^Goku=孫^悟空");
        patient1->setID("GOKU");
        Study *study1 = new Study();
        study1->setInstanceUID("1.2.3.4.1");
        study1->setDescription("aaa");
        patient1->addStudy(study1);
        Patient *patient3 = new Patient();
        patient3->setFullName("Son^Goku=孫^悟空");
        patient3->setID("GOKU");
        Study *study3 = new Study();
        study3->setInstanceUID("1.2.3.4.3");
        study3->setDescription("ccc");
        patient3->addStudy(study3);
        QTest::newRow("one bad study") << jsonText << QList<Patient*>{patient1, patient3} << true;
    }
}

void test_DicomJsonDecoder::getStudies_ReturnsExpectedValueAndErrors()
{
    QFETCH(QByteArray, jsonText);
    QFETCH(QList<Patient*>, expectedResult);
    QFETCH(bool, expectedToHaveErrors);

    DicomJsonDecoder decoder;
    QList<Patient*> patients = decoder.getStudies(jsonText);

    QCOMPARE(patients.size(), expectedResult.size());

    for (int i = 0; i < patients.size(); i++)
    {
        comparePatientsAndStudies(patients[i], expectedResult[i]);
        delete patients[i];
        delete expectedResult[i];
    }

    QCOMPARE(!decoder.getErrors().isEmpty(), expectedToHaveErrors);
}

void test_DicomJsonDecoder::getSeries_1_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QByteArray>("jsonText");
    QTest::addColumn<QList<Series*>>("expectedResult");
    QTest::addColumn<bool>("expectedToHaveErrors");

    QTest::newRow("bad json") << QByteArray("·= aç02 3'0") << QList<Series*>{} << true;

    {
        QByteArray jsonText(R"([
                                {
                                    "0020000E": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00200011": {
                                        "vr": "IS",
                                        "Value": [1]
                                    },
                                    "0008103E": {
                                        "vr": "LO",
                                        "Value": ["asdf"]
                                    },
                                    "00080060": {
                                        "vr": "CS",
                                        "Value": ["CT"]
                                    }
                                },
                                {
                                    "0020000E": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.2"]
                                    },
                                    "00200011": {
                                        "vr": "IS",
                                        "Value": [200]
                                    },
                                    "0008103E": {
                                        "vr": "LO"
                                    },
                                    "00080060": {
                                        "vr": "CS",
                                        "Value": ["CT"]
                                    }
                                }
                               ])");
        Series *series1 = new Series();
        series1->setInstanceUID("1.2.3.4.1");
        series1->setSeriesNumber("1");
        series1->setDescription("asdf");
        series1->setModality("CT");
        Series *series2 = new Series();
        series2->setInstanceUID("1.2.3.4.2");
        series2->setSeriesNumber("200");
        series2->setModality("CT");
        QTest::newRow("everything ok") << jsonText << QList<Series*>{series1, series2} << false;
    }

    {
        QByteArray jsonText(R"([
                                {
                                    "0020000E": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00200011": {
                                        "vr": "IS",
                                        "Value": [1]
                                    },
                                    "0008103E": {
                                        "vr": "LO",
                                        "Value": ["asdf"]
                                    },
                                    "00080060": {
                                        "vr": "CS",
                                        "Value": ["CT"]
                                    }
                                },
                                null,
                                {
                                    "0020000E": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.3"]
                                    },
                                    "00200011": {
                                        "vr": "IS",
                                        "Value": [300]
                                    },
                                    "0008103E": {
                                        "vr": "LO"
                                    },
                                    "00080060": {
                                        "vr": "CS",
                                        "Value": ["CT"]
                                    }
                                }
                               ])");
        Series *series1 = new Series();
        series1->setInstanceUID("1.2.3.4.1");
        series1->setSeriesNumber("1");
        series1->setDescription("asdf");
        series1->setModality("CT");
        Series *series3 = new Series();
        series3->setInstanceUID("1.2.3.4.3");
        series3->setSeriesNumber("300");
        series3->setModality("CT");
        QTest::newRow("one bad series") << jsonText << QList<Series*>{series1, series3} << true;
    }
}

void test_DicomJsonDecoder::getSeries_1_ReturnsExpectedValueAndErrors()
{
    QFETCH(QByteArray, jsonText);
    QFETCH(QList<Series*>, expectedResult);
    QFETCH(bool, expectedToHaveErrors);

    DicomJsonDecoder decoder;
    QList<Series*> series = decoder.getSeries(jsonText);

    QCOMPARE(series.size(), expectedResult.size());

    for (int i = 0; i < series.size(); i++)
    {
        compareSeries(series[i], expectedResult[i]);
        delete series[i];
        delete expectedResult[i];
    }

    QCOMPARE(!decoder.getErrors().isEmpty(), expectedToHaveErrors);
}

void test_DicomJsonDecoder::getInstances_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QByteArray>("jsonText");
    QTest::addColumn<QList<Image*>>("expectedResult");
    QTest::addColumn<bool>("expectedToHaveErrors");

    QTest::newRow("bad json") << QByteArray("·= aç02 3'0") << QList<Image*>{} << true;

    {
        QByteArray jsonText(R"([
                                {
                                    "00080018": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00200013": {
                                        "vr": "IS",
                                        "Value": [1]
                                    }
                                },
                                {
                                    "00080018": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.2"]
                                    },
                                    "00200013": {
                                        "vr": "IS",
                                        "Value": [2]
                                    }
                                }
                               ])");
        Image *image1 = new Image();
        image1->setSOPInstanceUID("1.2.3.4.1");
        image1->setInstanceNumber("1");
        Image *image2 = new Image();
        image2->setSOPInstanceUID("1.2.3.4.2");
        image2->setInstanceNumber("2");
        QTest::newRow("everything ok") << jsonText << QList<Image*>{image1, image2} << false;
    }

    {
        QByteArray jsonText(R"([
                                {
                                    "00080018": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.1"]
                                    },
                                    "00200013": {
                                        "vr": "IS",
                                        "Value": [1]
                                    }
                                },
                                "asdf",
                                {
                                    "00080018": {
                                        "vr": "UI",
                                        "Value": ["1.2.3.4.3"]
                                    },
                                    "00200013": {
                                        "vr": "IS",
                                        "Value": [3]
                                    }
                                }
                               ])");
        Image *image1 = new Image();
        image1->setSOPInstanceUID("1.2.3.4.1");
        image1->setInstanceNumber("1");
        Image *image3 = new Image();
        image3->setSOPInstanceUID("1.2.3.4.3");
        image3->setInstanceNumber("3");
        QTest::newRow("one bad instance") << jsonText << QList<Image*>{image1, image3} << true;
    }
}

void test_DicomJsonDecoder::getInstances_ReturnsExpectedValueAndErrors()
{
    QFETCH(QByteArray, jsonText);
    QFETCH(QList<Image*>, expectedResult);
    QFETCH(bool, expectedToHaveErrors);

    DicomJsonDecoder decoder;
    QList<Image*> instances = decoder.getInstances(jsonText);

    QCOMPARE(instances.size(), expectedResult.size());

    for (int i = 0; i < instances.size(); i++)
    {
        compareInstances(instances[i], expectedResult[i]);
        delete instances[i];
        delete expectedResult[i];
    }

    QCOMPARE(!decoder.getErrors().isEmpty(), expectedToHaveErrors);
}

void test_DicomJsonDecoder::getRootArray_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QByteArray>("jsonText");
    QTest::addColumn<QJsonArray>("expectedResult");
    QTest::addColumn<bool>("expectedToHaveErrors");

    QTest::newRow("empty string") << QByteArray("") << QJsonArray{} << true;
    QTest::newRow("random text") << QByteArray(",mf093'c0a8sed0'q w9") << QJsonArray{} << true;
    QTest::newRow("object") << QByteArray("{}") << QJsonArray{} << true;
    QTest::newRow("empty array") << QByteArray("[]") << QJsonArray{} << false;
    QTest::newRow("non-empty array") << QByteArray("[1]") << QJsonArray{1} << false;
}

void test_DicomJsonDecoder::getRootArray_ReturnsExpectedValueAndErrors()
{
    QFETCH(QByteArray, jsonText);
    QFETCH(QJsonArray, expectedResult);
    QFETCH(bool, expectedToHaveErrors);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getRootArray(jsonText), expectedResult);
    QCOMPARE(!decoder.getErrors().isEmpty(), expectedToHaveErrors);
}

void test_DicomJsonDecoder::getStudy_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<PacsDevice>("pacs");
    QTest::addColumn<bool>("overrideInstitutionName");
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<Patient*>("expectedResult");
    QTest::addColumn<QStringList>("expectedErrors");

    PacsDevice pacs;
    pacs.setID("1");
    pacs.setInstitution("Starviewer");
    DICOMSource dicomSource;
    dicomSource.addRetrievePACS(pacs);
    QString errorContext("test getStudy");

    QTest::newRow("not an object") << pacs << true << QJsonValue(22) << errorContext << static_cast<Patient*>(nullptr)
                                   << QStringList{errorContext + ": not an object."};

    {
        Patient *patient = new Patient();
        patient->setID("JS111");
        QJsonValue jsonValue(QJsonObject{
                                 {"00100020", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"JS111"}}}},
                                 {"0010", {}}
                             });
        QTest::newRow("invalid key format, study not added") << pacs << false << jsonValue << errorContext << patient
                                                             << QStringList{errorContext + ": invalid key format: \"0010\".",
                                                                            errorContext + ": study has not been added to the patient."};
    }

    {
        Patient *patient = new Patient();
        patient->setFullName("Smith^John");
        patient->setID("JS111");
        patient->setBirthDate("19030909");
        Study *study = new Study();
        study->setInstanceUID("1.2.3.4");
        study->setID("0383");
        study->setDate("20171001");
        study->setTime("171400");
        study->setDICOMSource(dicomSource);
        study->addModality("CT");
        study->setAccessionNumber("307305754");
        study->setInstitutionName("Hospital");
        patient->addStudy(study);
        QJsonValue jsonValue(QJsonObject{
                                 {"00100010", QJsonObject{{"vr", "PN"}, {"Value", QJsonArray{QJsonObject{{"Alphabetic", "Smith^John"}}}}}},
                                 {"00100020", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"JS111"}}}},
                                 {"00100030", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"19030909"}}}},
                                 {"0020000D", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"00200010", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"0383"}}}},
                                 {"00080020", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"20171001"}}}},
                                 {"00080030", QJsonObject{{"vr", "TM"}, {"Value", QJsonArray{"171400"}}}},
                                 {"00080061", QJsonObject{{"vr", "CS"}, {"Value", QJsonArray{"CT"}}}},
                                 {"00080050", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"307305754"}}}},
                                 {"00080080", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"Hospital"}}}}
                             });
        QTest::newRow("normal case, no override institution") << pacs << false << jsonValue << errorContext << patient << QStringList{};
    }

    {
        Patient *patient = new Patient();
        patient->setFullName("Smith^John");
        patient->setID("JS111");
        patient->setBirthDate("19030909");
        Study *study = new Study();
        study->setPatientAge("114Y");
        study->setInstanceUID("1.2.3.4");
        study->setID("0383");
        study->setDate("20171001");
        study->setTime("171400");
        study->setDescription("Whatever");
        study->addModality("CT");
        study->addModality("PT");
        study->setAccessionNumber("307305754");
        study->setReferringPhysiciansName("Pere^Pi");
        study->setInstitutionName("Starviewer");
        study->setDICOMSource(dicomSource);
        patient->addStudy(study);
        QJsonValue jsonValue(QJsonObject{
                                 {"00100010", QJsonObject{{"vr", "PN"}, {"Value", QJsonArray{QJsonObject{{"Alphabetic", "Smith^John"}}}}}},
                                 {"00100020", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"JS111"}}}},
                                 {"00100030", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"19030909"}}}},
                                 {"00101010", QJsonObject{{"vr", "AS"}, {"Value", QJsonArray{"114Y"}}}},
                                 {"0020000D", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"00200010", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"0383"}}}},
                                 {"00080020", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"20171001"}}}},
                                 {"00080030", QJsonObject{{"vr", "TM"}, {"Value", QJsonArray{"171400"}}}},
                                 {"00081030", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"Whatever"}}}},
                                 {"00080061", QJsonObject{{"vr", "CS"}, {"Value", QJsonArray{"CT", "PT"}}}},
                                 {"00080050", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"307305754"}}}},
                                 {"00080090", QJsonObject{{"vr", "PN"}, {"Value", QJsonArray{QJsonObject{{"Alphabetic", "Pere^Pi"}}}}}},
                                 {"00080080", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"Hospital"}}}}
                             });
        QTest::newRow("normal case, override institution") << pacs << true << jsonValue << errorContext << patient << QStringList{};
    }
}

void test_DicomJsonDecoder::getStudy_ReturnsExpectedValueAndErrors()
{
    QFETCH(PacsDevice, pacs);
    QFETCH(bool, overrideInstitutionName);
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(Patient*, expectedResult);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;
    decoder.setPacsDevice(pacs, overrideInstitutionName);
    Patient *patient = decoder.getStudy(jsonValue, errorContext);

    comparePatientsAndStudies(patient, expectedResult);
    QCOMPARE(decoder.getErrors(), expectedErrors);

    delete patient;
    delete expectedResult;
}

void test_DicomJsonDecoder::getSeries_2_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<PacsDevice>("pacs");
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<Series*>("expectedResult");
    QTest::addColumn<QStringList>("expectedErrors");

    PacsDevice pacs;
    pacs.setID("1");
    DICOMSource dicomSource;
    dicomSource.addRetrievePACS(pacs);
    QString errorContext("test getSeries (2)");

    QTest::newRow("not an object") << pacs << QJsonValue(22) << errorContext << static_cast<Series*>(nullptr) << QStringList{errorContext + ": not an object."};

    {
        Series *series = new Series();
        series->setInstanceUID("1.2.3.4");
        series->setDICOMSource(dicomSource);
        QJsonValue jsonValue(QJsonObject{
                                 {"0020000E", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"0008103e", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"bla bla bla"}}}}  // error because key is lowercase
                             });
        QTest::newRow("invalid key format") << pacs << jsonValue << errorContext << series << QStringList{errorContext + ": invalid key format: \"0008103e\"."};
    }

    {
        Series *series = new Series();
        series->setInstanceUID("1.2.3.4");
        series->setSeriesNumber("1");
        series->setDescription("bla bla bla");
        series->setProtocolName("IP");
        series->setDate("20171001");
        series->setTime("171400");
        series->setModality("CT");
        series->setPerformedProcedureStepStartDate("20211129");
        series->setPerformedProcedureStepStartTime("131200");
        series->setRequestedProcedureID("fail");
        series->setScheduledProcedureStepID("end");
        series->setDICOMSource(dicomSource);
        QJsonValue jsonValue(QJsonObject{
                                 {"0020000E", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"00200011", QJsonObject{{"vr", "IS"}, {"Value", QJsonArray{1}}}},
                                 {"0008103E", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"bla bla bla"}}}},
                                 {"00181030", QJsonObject{{"vr", "LO"}, {"Value", QJsonArray{"IP"}}}},
                                 {"00080021", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"20171001"}}}},
                                 {"00080031", QJsonObject{{"vr", "TM"}, {"Value", QJsonArray{"171400"}}}},
                                 {"00080060", QJsonObject{{"vr", "CS"}, {"Value", QJsonArray{"CT"}}}},
                                 {"00400244", QJsonObject{{"vr", "DA"}, {"Value", QJsonArray{"20211129"}}}},
                                 {"00400245", QJsonObject{{"vr", "TM"}, {"Value", QJsonArray{"131200"}}}},
                                 {"00401001", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"fail"}}}},
                                 {"00400009", QJsonObject{{"vr", "SH"}, {"Value", QJsonArray{"end"}}}}
                             });
        QTest::newRow("normal case") << pacs << jsonValue << errorContext << series << QStringList{};
    }
}

void test_DicomJsonDecoder::getSeries_2_ReturnsExpectedValueAndErrors()
{
    QFETCH(PacsDevice, pacs);
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(Series*, expectedResult);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;
    decoder.setPacsDevice(pacs);
    Series *series = decoder.getSeries(jsonValue, errorContext);

    compareSeries(series, expectedResult);
    QCOMPARE(decoder.getErrors(), expectedErrors);

    delete series;
    delete expectedResult;
}

void test_DicomJsonDecoder::getInstance_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<PacsDevice>("pacs");
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<Image*>("expectedResult");
    QTest::addColumn<QStringList>("expectedErrors");

    PacsDevice pacs;
    pacs.setID("1");
    DICOMSource dicomSource;
    dicomSource.addRetrievePACS(pacs);
    QString errorContext("test getInstance");

    QTest::newRow("not an object") << pacs << QJsonValue(22) << errorContext << static_cast<Image*>(nullptr) << QStringList{errorContext + ": not an object."};

    {
        Image *instance = new Image();
        instance->setSOPInstanceUID("1.2.3.4");
        instance->setDICOMSource(dicomSource);
        QJsonValue jsonValue(QJsonObject{
                                 {"00080018", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"asdf", {}}
                             });
        QTest::newRow("invalid key format") << pacs << jsonValue << errorContext << instance << QStringList{errorContext + ": invalid key format: \"asdf\"."};
    }

    {
        Image *instance = new Image();
        instance->setSOPInstanceUID("1.2.3.4");
        instance->setInstanceNumber("1");
        instance->setDICOMSource(dicomSource);
        QJsonValue jsonValue(QJsonObject{
                                 {"00080018", QJsonObject{{"vr", "UI"}, {"Value", QJsonArray{"1.2.3.4"}}}},
                                 {"00200013", QJsonObject{{"vr", "IS"}, {"Value", QJsonArray{"1"}}}}
                             });
        QTest::newRow("normal case") << pacs << jsonValue << errorContext << instance << QStringList{};
    }
}

void test_DicomJsonDecoder::getInstance_ReturnsExpectedValueAndErrors()
{
    QFETCH(PacsDevice, pacs);
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(Image*, expectedResult);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;
    decoder.setPacsDevice(pacs);
    Image *instance = decoder.getInstance(jsonValue, errorContext);

    compareInstances(instance, expectedResult);
    QCOMPARE(decoder.getErrors(), expectedErrors);

    delete instance;
    delete expectedResult;
}

void test_DicomJsonDecoder::getTagValueAsString_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<QString>("expectedValue");
    QTest::addColumn<QStringList>("expectedErrors");

    QString errorContext("test getTagValueAsString");

    QTest::newRow("undefined expected type") << QJsonValue(QJsonObject{{"vr", "NJ"}}) << errorContext << QString()
                                             << QStringList{errorContext + ", key vr: unsupported VR: NJ."};
    QTest::newRow("empty value array") << QJsonValue(QJsonObject{{"vr", "UC"}}) << errorContext << QString() << QStringList{};
    QTest::newRow("more than one value in array") << QJsonValue(QJsonObject{{"vr", "UC"}, {"Value", QJsonArray{"a", "b", "c"}}}) << errorContext << QString("a")
                                                  << QStringList(errorContext + ": Value array has more than one element.");
    QTest::newRow("normal case") << QJsonValue(QJsonObject{{"vr", "UC"}, {"Value", QJsonArray{"a"}}}) << errorContext << QString("a") << QStringList{};
}

void test_DicomJsonDecoder::getTagValueAsString_ReturnsExpectedValueAndErrors()
{
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(QString, expectedValue);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getTagValueAsString(jsonValue, errorContext), expectedValue);
    QCOMPARE(decoder.getErrors(), expectedErrors);
}

void test_DicomJsonDecoder::getTagValueAsStringList_ReturnsExpectedValue_data()
{
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QStringList>("expectedValue");

    QTest::newRow("undefined expected type") << QJsonValue(QJsonObject{{"vr", "NJ"}}) << QStringList{};
    QTest::newRow("empty value array") << QJsonValue(QJsonObject{{"vr", "UC"}}) << QStringList{};
    QTest::newRow("normal case") << QJsonValue(QJsonObject{{"vr", "UC"}, {"Value", QJsonArray{"a", "b", "c"}}}) << QStringList{"a", "b", "c"};
}

void test_DicomJsonDecoder::getTagValueAsStringList_ReturnsExpectedValue()
{
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QStringList, expectedValue);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getTagValueAsStringList(jsonValue, {}), expectedValue);
}

void test_DicomJsonDecoder::getExpectedType_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<DicomJsonDecoder::JsonDataType>("expectedType");
    QTest::addColumn<QStringList>("expectedErrors");

    QString errorContext("test getExpectedType");

    QTest::newRow("not an object") << QJsonValue("asdf") << errorContext << DicomJsonDecoder::JsonDataType::Undefined
                                   << QStringList{errorContext + ": value is not an object."};
    QTest::newRow("no vr") << QJsonValue(QJsonObject{}) << errorContext << DicomJsonDecoder::JsonDataType::Undefined
                           << QStringList{errorContext + ": value does not specify VR."};
    QTest::newRow("vr not a string") << QJsonValue(QJsonObject{{"vr", 0}}) << errorContext << DicomJsonDecoder::JsonDataType::Undefined
                                     << QStringList{errorContext + ", key vr: value is not a string."};
    QTest::newRow("unrecognized VR") << QJsonValue(QJsonObject{{"vr", "L·L"}}) << errorContext << DicomJsonDecoder::JsonDataType::Undefined
                                     << QStringList{errorContext + ", key vr: unsupported VR: L·L."};
    QTest::newRow("recognized VR") << QJsonValue(QJsonObject{{"vr", "AT"}}) << errorContext << DicomJsonDecoder::JsonDataType::String << QStringList{};
}

void test_DicomJsonDecoder::getExpectedType_ReturnsExpectedValueAndErrors()
{
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(DicomJsonDecoder::JsonDataType, expectedType);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getExpectedType(jsonValue, errorContext), expectedType);
    QCOMPARE(decoder.getErrors(), expectedErrors);
}

void test_DicomJsonDecoder::getValueArray_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QJsonValue>("jsonValue");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<QJsonArray>("expectedResult");
    QTest::addColumn<QStringList>("expectedErrors");

    QString errorContext("test getValueArray");

    QTest::newRow("not an object") << QJsonValue(true) << errorContext << QJsonArray{} << QStringList{errorContext + ": value is not an object."};
    QTest::newRow("no Value") << QJsonValue(QJsonObject{}) << errorContext << QJsonArray{} << QStringList{};
    QTest::newRow("Value not an array") << QJsonValue(QJsonObject{{"Value", "ç"}}) << errorContext << QJsonArray{}
                                        << QStringList{errorContext + ", key Value: value is not an array."};
    QTest::newRow("empty array") << QJsonValue(QJsonObject{{"Value", QJsonArray{}}}) << errorContext << QJsonArray{}
                                 << QStringList{errorContext + ", key Value: array is empty."};
    QTest::newRow("normal return") << QJsonValue(QJsonObject{{"Value", QJsonArray{"asdf", "QWERTY"}}}) << errorContext << QJsonArray{"asdf", "QWERTY"}
                                   << QStringList{};
}

void test_DicomJsonDecoder::getValueArray_ReturnsExpectedValueAndErrors()
{
    QFETCH(QJsonValue, jsonValue);
    QFETCH(QString, errorContext);
    QFETCH(QJsonArray, expectedResult);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getValueArray(jsonValue, errorContext), expectedResult);
    QCOMPARE(decoder.getErrors(), expectedErrors);
}

void test_DicomJsonDecoder::getArrayAsStringList_ReturnsExpectedValueAndErrors_data()
{
    QTest::addColumn<QJsonArray>("jsonArray");
    QTest::addColumn<DicomJsonDecoder::JsonDataType>("jsonDataType");
    QTest::addColumn<QString>("errorContext");
    QTest::addColumn<QStringList>("expectedResult");
    QTest::addColumn<QStringList>("expectedErrors");

    QString errorContext("test getArrayAsStringList");

    QTest::newRow("empty array") << QJsonArray{} << DicomJsonDecoder::JsonDataType::Double << errorContext << QStringList{} << QStringList{};
    QTest::newRow("some nulls") << QJsonArray{0, 5, 12, -2, QJsonValue(), QJsonValue()} << DicomJsonDecoder::JsonDataType::Integer << errorContext
                                << QStringList{"0", "5", "12", "-2", "null", "null"} << QStringList{};
    QTest::newRow("strings") << QJsonArray{"foo", "bar"} << DicomJsonDecoder::JsonDataType::String << errorContext << QStringList{"foo", "bar"}
                             << QStringList{};
    QTest::newRow("strings (errors)") << QJsonArray{1234, "--"} << DicomJsonDecoder::JsonDataType::String << errorContext << QStringList{{}, "--"}
                                      << QStringList{errorContext + ", index 0: not a string."};
    QTest::newRow("integers") << QJsonArray{1312} << DicomJsonDecoder::JsonDataType::Integer << errorContext << QStringList{"1312"} << QStringList{};
    QTest::newRow("integers (errors)") << QJsonArray{1714, 10, -100, QJsonObject{}, 83,  QJsonValue()} << DicomJsonDecoder::JsonDataType::Integer
                                       << errorContext << QStringList{"1714", "10", "-100", {}, "83", "null"}
                                       << QStringList{errorContext + ", index 3: not a number."};
    QTest::newRow("doubles") << QJsonArray{3.1416, 10e8, 5} << DicomJsonDecoder::JsonDataType::Double << errorContext << QStringList{"3.1416", "1e+09", "5"}
                             << QStringList{};
    QTest::newRow("doubles (errors)") << QJsonArray{QJsonObject{}, -0.5} << DicomJsonDecoder::JsonDataType::Double << errorContext << QStringList{{}, "-0.5"}
                                      << QStringList{errorContext + ", index 0: not a number."};
    QTest::newRow("integers or strings") << QJsonArray{"123", -12} << DicomJsonDecoder::JsonDataType::IntegerOrString << errorContext
                                         << QStringList{"123", "-12"} << QStringList{};
    QTest::newRow("integers or strings (errors)") << QJsonArray{"64", 1, 99, QJsonArray{}, -459467, "59", "18"}
                                                  << DicomJsonDecoder::JsonDataType::IntegerOrString << errorContext
                                                  << QStringList{"64", "1", "99", {}, "-459467", "59", "18"}
                                                  << QStringList{errorContext + ", index 3: not a number nor string."};
    QTest::newRow("doubles or strings") << QJsonArray{-2.7, "+8.32E-10", +8.32E-10, "44"} << DicomJsonDecoder::JsonDataType::DoubleOrString << errorContext
                                        << QStringList{"-2.7", "+8.32E-10", "8.32e-10", "44"} << QStringList{};
    QTest::newRow("doubles or strings (errors)") << QJsonArray{7, "264", 99.99, -3.5395, "0.0", 83649, "444", false}
                                                 << DicomJsonDecoder::JsonDataType::DoubleOrString << errorContext
                                                 << QStringList{"7", "264", "99.99", "-3.5395", "0.0", "83649", "444", {}}
                                                 << QStringList{errorContext + ", index 7: not a number nor string."};
    QTest::newRow("person name (all groups)") << QJsonArray{QJsonObject{{"Alphabetic", "Son^Goku"}, {"Ideographic", "孫^悟空"}, {"Phonetic", "そん^ごくう"}}}
                                              << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext << QStringList{"Son^Goku=孫^悟空=そん^ごくう"}
                                              << QStringList{};
    QTest::newRow("person name (alphabetic and phonetic)") << QJsonArray{QJsonObject{{"Alphabetic", "Son^Goku"}, {"Phonetic", "そん^ごくう"}}}
                                                           << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext
                                                           << QStringList{"Son^Goku==そん^ごくう"} << QStringList{};
    QTest::newRow("person name (alphabetic and ideographic)") << QJsonArray{QJsonObject{{"Alphabetic", "Son^Goku"}, {"Ideographic", "孫^悟空"}}}
                                                              << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext
                                                              << QStringList{"Son^Goku=孫^悟空"} << QStringList{};
    QTest::newRow("person name (ideographic and phonetic)") << QJsonArray{QJsonObject{{"Ideographic", "孫^悟空"}, {"Phonetic", "そん^ごくう"}}}
                                                            << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext
                                                            << QStringList{"=孫^悟空=そん^ごくう"} << QStringList{};
    QTest::newRow("person name (alphabetic)") << QJsonArray{QJsonObject{{"Alphabetic", "Son^Goku"}}} << DicomJsonDecoder::JsonDataType::PersonNameObject
                                              << errorContext << QStringList{"Son^Goku"} << QStringList{};
    QTest::newRow("person name (ideographic)") << QJsonArray{QJsonObject{{"Ideographic", "孫^悟空"}}} << DicomJsonDecoder::JsonDataType::PersonNameObject
                                               << errorContext << QStringList{"=孫^悟空"} << QStringList{};
    QTest::newRow("person name (phonetic)") << QJsonArray{QJsonObject{{"Phonetic", "そん^ごくう"}}} << DicomJsonDecoder::JsonDataType::PersonNameObject
                                            << errorContext << QStringList{"==そん^ごくう"} << QStringList{};
    QTest::newRow("person name (empty)") << QJsonArray{QJsonObject{}} << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext << QStringList{{}}
                                         << QStringList{};
    QTest::newRow("person name (ignored keys)") << QJsonArray{QJsonObject{{"Alphabetic", "Son^Goku"}, {"Ideographic", "孫^悟空"}, {"x", 1}}}
                                                << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext << QStringList{"Son^Goku=孫^悟空"}
                                                << QStringList{};
    QTest::newRow("person name (errors)") << QJsonArray{QJsonObject{{"Ideographic", 1.5e9}, {"Phonetic", "そん^ごくう"}}, 22.2}
                                          << DicomJsonDecoder::JsonDataType::PersonNameObject << errorContext << QStringList{"==そん^ごくう", {}}
                                          << QStringList{errorContext + ", index 0, key Ideographic: not a string.",
                                                         errorContext + ", index 1: not an object."};
    QTest::newRow("undefined") << QJsonArray{"asdf"} << DicomJsonDecoder::JsonDataType::Undefined << errorContext << QStringList{{}}
                               << QStringList{errorContext + ", index 0: unsupported type."};
    // In this array case two items are added to ensure that the initializer list constructor is called.
    // With one item clang for example wrongly calls the move constructor (https://bugs.llvm.org/show_bug.cgi?id=23812).
    QTest::newRow("array") << QJsonArray{QJsonArray{}, QJsonArray{}} << DicomJsonDecoder::JsonDataType::Array << errorContext << QStringList{{}, {}}
                           << QStringList{errorContext + ", index 0: unsupported type.", errorContext + ", index 1: unsupported type."};
    QTest::newRow("base64") << QJsonArray{"U29uIEdva3U="} << DicomJsonDecoder::JsonDataType::Base64String << errorContext << QStringList{{}}
                            << QStringList{errorContext + ", index 0: unsupported type."};
}

void test_DicomJsonDecoder::getArrayAsStringList_ReturnsExpectedValueAndErrors()
{
    QFETCH(QJsonArray, jsonArray);
    QFETCH(DicomJsonDecoder::JsonDataType, jsonDataType);
    QFETCH(QString, errorContext);
    QFETCH(QStringList, expectedResult);
    QFETCH(QStringList, expectedErrors);

    DicomJsonDecoder decoder;

    QCOMPARE(decoder.getArrayAsStringList(jsonArray, jsonDataType, errorContext), expectedResult);
    QCOMPARE(decoder.getErrors(), expectedErrors);
}

DECLARE_TEST(test_DicomJsonDecoder)

#include "test_dicomjsondecoder.moc"
