#include "autotest.h"
#include "hangingprotocol.h"
#include "hangingprotocolimageset.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocoltesthelper.h"
#include "hangingprotocolmask.h"
#include <QSharedPointer>

using namespace udg;
using namespace testing;

class test_HangingProtocol : public QObject {
Q_OBJECT

private slots:

void copy_ShouldReturnExactHangingProtocolAndDifferentObjects_data();
void copy_ShouldReturnExactHangingProtocolAndDifferentObjects();
void compareTo_WorksWithDifferentTypesOfHangingProtocols_data();
void compareTo_WorksWithDifferentTypesOfHangingProtocols();
};

Q_DECLARE_METATYPE(QSharedPointer<HangingProtocol>)

void test_HangingProtocol::copy_ShouldReturnExactHangingProtocolAndDifferentObjects_data()
{
    QSharedPointer<HangingProtocol> emptyHangingProtocol(HangingProtocolTestHelper::createEmptyHangingProtocol());
    QSharedPointer<HangingProtocol> emptyHangingProtocolCopied(new HangingProtocol(emptyHangingProtocol.data()));

    QSharedPointer<HangingProtocol> basicHangingProtocol(HangingProtocolTestHelper::createHangingProtocolWithoutImageSetAndDisplaySet());
    QSharedPointer<HangingProtocol> basicHangingProtocolCopied(new HangingProtocol(basicHangingProtocol.data()));

    QSharedPointer<HangingProtocol> advancedHangingProtocol(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
    QSharedPointer<HangingProtocol> advancedHangingProtocolCopied(new HangingProtocol(advancedHangingProtocol.data()));

    QTest::addColumn<QSharedPointer<HangingProtocol> >("hangingProtocol1");
    QTest::addColumn<QSharedPointer<HangingProtocol> >("hangingProtocol2");
    QTest::addColumn<bool>("sameObject");
    QTest::addColumn<bool>("sameAttributesValue");

    QTest::newRow("copy of empty hanging protocol") << emptyHangingProtocol << emptyHangingProtocolCopied << false << true;
    QTest::newRow("copy of basic hanging protocol") << basicHangingProtocol << basicHangingProtocolCopied << false << true;
    QTest::newRow("copy of complete hanging protocol") << advancedHangingProtocol << advancedHangingProtocolCopied << false << true;
}

void test_HangingProtocol::copy_ShouldReturnExactHangingProtocolAndDifferentObjects()
{
    QFETCH(QSharedPointer<HangingProtocol>, hangingProtocol1);
    QFETCH(QSharedPointer<HangingProtocol>, hangingProtocol2);
    QFETCH(bool, sameObject);
    QFETCH(bool, sameAttributesValue);

    QCOMPARE(hangingProtocol1.data()->compareTo(hangingProtocol2.data()), sameAttributesValue);
    QCOMPARE(hangingProtocol1.data() == hangingProtocol2.data(), sameObject); //Compara adreces
    QCOMPARE(hangingProtocol1.data()->getHangingProtocolMask() == hangingProtocol2.data()->getHangingProtocolMask(), sameObject); //Compara adreces

    foreach (HangingProtocolImageSet *imageSet, hangingProtocol1.data()->getImageSets()) //Compara adreces
    {
        HangingProtocolImageSet *imageSetCopied = hangingProtocol2.data()->getImageSet(imageSet->getIdentifier());
        QCOMPARE(imageSet == imageSetCopied, sameObject);
    }

    foreach (HangingProtocolDisplaySet *displaySet, hangingProtocol1.data()->getDisplaySets()) //Compara adreces
    {
        HangingProtocolDisplaySet *displaySetCopied = hangingProtocol2.data()->getDisplaySet(displaySet->getIdentifier());
        QCOMPARE(displaySet == displaySetCopied, sameObject);
    }
}

void test_HangingProtocol::compareTo_WorksWithDifferentTypesOfHangingProtocols_data()
{
    QSharedPointer<HangingProtocol> emptyHangingProtocol_1(HangingProtocolTestHelper::createEmptyHangingProtocol());
    QSharedPointer<HangingProtocol> emptyHangingProtocol_2(new HangingProtocol(emptyHangingProtocol_1.data()));
    QSharedPointer<HangingProtocol> emptyHangingProtocol_3(new HangingProtocol(emptyHangingProtocol_1.data()));
    emptyHangingProtocol_3.data()->setPriority(10);

    QSharedPointer<HangingProtocol> basicHangingProtocol_1(HangingProtocolTestHelper::createHangingProtocolWithoutImageSetAndDisplaySet());
    QSharedPointer<HangingProtocol> basicHangingProtocol_2(new HangingProtocol(basicHangingProtocol_1.data()));
    QSharedPointer<HangingProtocol> basicHangingProtocol_3(new HangingProtocol(basicHangingProtocol_1.data()));
    basicHangingProtocol_3.data()->setIdentifier(5);

    QSharedPointer<HangingProtocol> advancedHangingProtocol_1(HangingProtocolTestHelper::createHangingProtocolWithOneImageSetAndOneDisplaySet());
    QSharedPointer<HangingProtocol> advancedHangingProtocol_2(new HangingProtocol(advancedHangingProtocol_1.data()));
    QSharedPointer<HangingProtocol> advancedHangingProtocol_3(new HangingProtocol(advancedHangingProtocol_1.data()));
    QSharedPointer<HangingProtocol> advancedHangingProtocol_4(new HangingProtocol(advancedHangingProtocol_1.data()));
    advancedHangingProtocol_3.data()->getImageSet(1)->setImageNumberInPatientModality(10);
    advancedHangingProtocol_4.data()->getDisplaySet(1)->setSlice(10);

    QTest::addColumn<QSharedPointer<HangingProtocol> >("hangingProtocol1");
    QTest::addColumn<QSharedPointer<HangingProtocol> >("hangingProtocol2");
    QTest::addColumn<bool>("expectedResult");

    QTest::newRow("copied empty hanging protocols are equals") << emptyHangingProtocol_1 << emptyHangingProtocol_2 << true;
    QTest::newRow("hanging protocols with different priority are differents") << emptyHangingProtocol_1 << emptyHangingProtocol_3 << false;
    QTest::newRow("copied basic hanging protocols are equals") << basicHangingProtocol_1 << basicHangingProtocol_2 << true;
    QTest::newRow("hanging protocols with different id are differents") << basicHangingProtocol_1 << basicHangingProtocol_3 << false;
    QTest::newRow("copied completed hanging protocols are equals") << advancedHangingProtocol_1 << advancedHangingProtocol_2 << true;
    QTest::newRow("hanging protocols with different image sets are differents") << advancedHangingProtocol_1 << advancedHangingProtocol_3 << false;
    QTest::newRow("hanging protocols with different display sets are differents") << advancedHangingProtocol_1 << advancedHangingProtocol_4 << false;
}

void test_HangingProtocol::compareTo_WorksWithDifferentTypesOfHangingProtocols()
{    
    QFETCH(QSharedPointer<HangingProtocol>, hangingProtocol1);
    QFETCH(QSharedPointer<HangingProtocol>, hangingProtocol2);
    QFETCH(bool, expectedResult);

    QCOMPARE(hangingProtocol1.data()->compareTo(hangingProtocol2.data()), expectedResult);
}


DECLARE_TEST(test_HangingProtocol)

#include "test_hangingprotocol.moc"