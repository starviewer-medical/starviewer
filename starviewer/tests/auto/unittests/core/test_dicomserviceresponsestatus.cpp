#include "autotest.h"
#include "dicomserviceresponsestatus.h"
#include "mathtools.h"
#include "dicomdictionary.h"

using namespace udg;

class test_DICOMServiceResponseStatus : public QObject {
Q_OBJECT

private slots:
    void getServiceStatus_ShouldReturnExpectedValues_data();
    void getServiceStatus_ShouldReturnExpectedValues();

    void getRelatedFields_ShouldReturnExpectedValues_data();
    void getRelatedFields_ShouldReturnExpectedValues();
};

Q_DECLARE_METATYPE(DICOMServiceResponseStatus::ServiceStatusType)
Q_DECLARE_METATYPE(QList<DICOMTag>)

void test_DICOMServiceResponseStatus::getServiceStatus_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<unsigned int>("statusCode");
    QTest::addColumn<DICOMServiceResponseStatus::ServiceStatusType>("expectedServiceStatus");

    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResources = 0xa700 - Failure") << DICOMServiceResponseStatus::RefusedOutOfResources 
        << DICOMServiceResponseStatus::FailureStatus;

    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToCalculateNumberOfMatches = 0xa701 - Failure") 
        << DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToCalculateNumberOfMatches << DICOMServiceResponseStatus::FailureStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToPerformSubOperations = 0xa702 - Failure") 
        << DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToPerformSubOperations << DICOMServiceResponseStatus::FailureStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::RefusedMoveDestinationUnknown = 0xa801 - Failure") << DICOMServiceResponseStatus::RefusedMoveDestinationUnknown
        << DICOMServiceResponseStatus::FailureStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::IdentifierDoesNotMatchSOPClass = 0xa900 - Failure") << DICOMServiceResponseStatus::IdentifierDoesNotMatchSOPClass
        << DICOMServiceResponseStatus::FailureStatus;

    QTest::newRow("DICOMServiceResponseStatus::ErrorDataSetDoesNotMatchSOPClass = 0xa900 - Failure") << DICOMServiceResponseStatus::ErrorDataSetDoesNotMatchSOPClass
        << DICOMServiceResponseStatus::FailureStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::ErrorCannotUnderstand = 0xc000 - Failure") << DICOMServiceResponseStatus::ErrorCannotUnderstand
        << DICOMServiceResponseStatus::FailureStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::UnableToProcess = 0xc000 - Failure") << DICOMServiceResponseStatus::UnableToProcess
        << DICOMServiceResponseStatus::FailureStatus;

    QTest::newRow("DICOMServiceResponseStatus::TerminatedDueToCancelRequest = 0xfe00 - Cancel") << DICOMServiceResponseStatus::TerminatedDueToCancelRequest
        << DICOMServiceResponseStatus::CancelStatus;
    
    QTest::newRow("DICOMServiceResponseStatus::WarningSubOperationsCompleteOneOrMoreFailures = 0xb000 - Warning") 
        << DICOMServiceResponseStatus::WarningSubOperationsCompleteOneOrMoreFailures << DICOMServiceResponseStatus::WarningStatus;

    QTest::newRow("DICOMServiceResponseStatus::WarningCoercionOfDataElements = 0xb000 - Warning") << DICOMServiceResponseStatus::WarningCoercionOfDataElements
        << DICOMServiceResponseStatus::WarningStatus;

    QTest::newRow("DICOMServiceResponseStatus::WarningDatasetDoesNotMatchSOPClass = 0xb007 - Warning") << DICOMServiceResponseStatus::WarningDatasetDoesNotMatchSOPClass
        << DICOMServiceResponseStatus::WarningStatus;

    QTest::newRow("DICOMServiceResponseStatus::WarningElementsDiscarded = 0xb006 - Warning") << DICOMServiceResponseStatus::WarningElementsDiscarded
        << DICOMServiceResponseStatus::WarningStatus;

    QTest::newRow("DICOMServiceResponseStatus::Pending = 0xff00 - Pending") << DICOMServiceResponseStatus::Pending
        << DICOMServiceResponseStatus::PendingStatus;

    QTest::newRow("DICOMServiceResponseStatus::PendingWarningUnsupportedOptionalKeys = 0xff01 - Pending") << DICOMServiceResponseStatus::PendingWarningUnsupportedOptionalKeys
        << DICOMServiceResponseStatus::PendingStatus;

    QTest::newRow("DICOMServiceResponseStatus::Success = 0x0000 - Success") << DICOMServiceResponseStatus::Success
        << DICOMServiceResponseStatus::SuccessStatus;

    unsigned int randomUnknownCode = (unsigned int)MathTools::randomInt(1, 0xa6ff);
    QTest::newRow("Random code: 1:0xA6FF- Unknown") << randomUnknownCode << DICOMServiceResponseStatus::UnknownStatus;
}

void test_DICOMServiceResponseStatus::getServiceStatus_ShouldReturnExpectedValues()
{
    QFETCH(unsigned int, statusCode);
    QFETCH(DICOMServiceResponseStatus::ServiceStatusType, expectedServiceStatus);

    DICOMServiceResponseStatus response;
    response.setStatusCode(statusCode);

    QCOMPARE(response.getServiceStatus(), expectedServiceStatus);
}

void test_DICOMServiceResponseStatus::getRelatedFields_ShouldReturnExpectedValues_data()
{
    QTest::addColumn<unsigned int>("statusCode");
    QTest::addColumn<QList<DICOMTag> >("expectedRelatedFields");

    QList<DICOMTag> errorComment;
    errorComment << DICOMErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResources = 0xa700") << DICOMServiceResponseStatus::RefusedOutOfResources << errorComment;

    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToCalculateNumberOfMatches = 0xa701") 
        << DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToCalculateNumberOfMatches << errorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::RefusedMoveDestinationUnknown = 0xa801 - Failure") << DICOMServiceResponseStatus::RefusedMoveDestinationUnknown
        << errorComment;
    
    QList<DICOMTag> subOperations;
    subOperations << DICOMNumberOfCompletedSubOperations << DICOMNumberOfFailedSubOperations << DICOMNumberOfWarningSubOperations;
    
    QTest::newRow("DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToPerformSubOperations = 0xa702") 
        << DICOMServiceResponseStatus::RefusedOutOfResourcesUnableToPerformSubOperations << subOperations;
    
    QTest::newRow("DICOMServiceResponseStatus::WarningSubOperationsCompleteOneOrMoreFailures = 0xb000") 
        << DICOMServiceResponseStatus::WarningSubOperationsCompleteOneOrMoreFailures << subOperations;

    QTest::newRow("DICOMServiceResponseStatus::WarningCoercionOfDataElements = 0xb000") << DICOMServiceResponseStatus::WarningCoercionOfDataElements
        << subOperations;

    QTest::newRow("DICOMServiceResponseStatus::Success = 0x0000 - Success") << DICOMServiceResponseStatus::Success
        << subOperations;

    QList<DICOMTag> offendingAndErrorComment;
    offendingAndErrorComment << DICOMOffendingElement << DICOMErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::IdentifierDoesNotMatchSOPClass = 0xa900") << DICOMServiceResponseStatus::IdentifierDoesNotMatchSOPClass
        << offendingAndErrorComment;

    QTest::newRow("DICOMServiceResponseStatus::ErrorDataSetDoesNotMatchSOPClass = 0xa900") << DICOMServiceResponseStatus::ErrorDataSetDoesNotMatchSOPClass
        << offendingAndErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::ErrorCannotUnderstand = 0xc000") << DICOMServiceResponseStatus::ErrorCannotUnderstand
        << offendingAndErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::UnableToProcess = 0xc000") << DICOMServiceResponseStatus::UnableToProcess
        << offendingAndErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::WarningDatasetDoesNotMatchSOPClass = 0xb007") << DICOMServiceResponseStatus::WarningDatasetDoesNotMatchSOPClass
        << offendingAndErrorComment;
    
    QTest::newRow("DICOMServiceResponseStatus::WarningElementsDiscarded = 0xb006") << DICOMServiceResponseStatus::WarningElementsDiscarded
        << offendingAndErrorComment;

    QList<DICOMTag> cancelOrPending;
    cancelOrPending << DICOMNumberOfRemainingSubOperations << DICOMNumberOfCompletedSubOperations << DICOMNumberOfFailedSubOperations << DICOMNumberOfWarningSubOperations;
    
    QTest::newRow("DICOMServiceResponseStatus::TerminatedDueToCancelRequest = 0xfe00 - Cancel") << DICOMServiceResponseStatus::TerminatedDueToCancelRequest
        << cancelOrPending;
    
    QTest::newRow("DICOMServiceResponseStatus::Pending = 0xff00 - Pending") << DICOMServiceResponseStatus::Pending
        << cancelOrPending;

    QTest::newRow("DICOMServiceResponseStatus::PendingWarningUnsupportedOptionalKeys = 0xff01 - Pending") << DICOMServiceResponseStatus::PendingWarningUnsupportedOptionalKeys
        << QList<DICOMTag>();

    unsigned int randomUnknownCode = (unsigned int)MathTools::randomInt(1, 0xa6ff);
    QTest::newRow("Random code: 1:0xA6FF- Unknown") << randomUnknownCode << QList<DICOMTag>();
}

void test_DICOMServiceResponseStatus::getRelatedFields_ShouldReturnExpectedValues()
{
    QFETCH(unsigned int, statusCode);
    QFETCH(QList<DICOMTag>, expectedRelatedFields);

    DICOMServiceResponseStatus response;
    response.setStatusCode(statusCode);

    QCOMPARE(response.getRelatedFields(), expectedRelatedFields);
}

DECLARE_TEST(test_DICOMServiceResponseStatus)

#include "test_dicomserviceresponsestatus.moc"
