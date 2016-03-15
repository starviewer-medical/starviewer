#include "autotest.h"
#include "hangingprotocolimagesetrestrictionexpression.h"

#include "image.h"
#include "series.h"

using namespace udg;

class test_HangingProtocolImageSetRestrictionExpression : public QObject {
    Q_OBJECT

private slots:
    void test_Series_ShouldReturnExpectedValue_data();
    void test_Series_ShouldReturnExpectedValue();

    void test_Image_ShouldReturnExpectedValue_data();
    void test_Image_ShouldReturnExpectedValue();

};

Q_DECLARE_METATYPE(HangingProtocolImageSetRestrictionExpression)

void test_HangingProtocolImageSetRestrictionExpression::test_Series_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<HangingProtocolImageSetRestrictionExpression>("restrictionExpression");
    QTest::addColumn<Series*>("series");
    QTest::addColumn<bool>("expectedValue");

    QMap<int, HangingProtocolImageSetRestriction> restrictions;
    restrictions[1] = HangingProtocolImageSetRestriction(1, "ViewPosition", "LATERAL", 0);
    restrictions[2] = HangingProtocolImageSetRestriction(2, "BodyPartExamined", "HEAD", 0);
    restrictions[3] = HangingProtocolImageSetRestriction(3, "ProtocolName", "Head CT", 0);
    restrictions[4] = HangingProtocolImageSetRestriction(4, "SeriesDecription", "Description", 0);

    Series *series = new Series(this);
    series->setBodyPartExamined("CHEST");
    series->setDescription("Description");
    series->setProtocolName("Whatever");
    series->setViewPosition("LATERAL");

    QTest::newRow("empty expression") << HangingProtocolImageSetRestrictionExpression() << series << true;
    QTest::newRow("garbage expression") << HangingProtocolImageSetRestrictionExpression("0 9a's0df9as0", restrictions) << series << true;

    QTest::newRow("one restriction, true") << HangingProtocolImageSetRestrictionExpression("1", restrictions) << series << true;
    QTest::newRow("one restriction, false") << HangingProtocolImageSetRestrictionExpression("2", restrictions) << series << false;

    QTest::newRow("not false") << HangingProtocolImageSetRestrictionExpression("not 2", restrictions) << series << true;
    QTest::newRow("not true") << HangingProtocolImageSetRestrictionExpression("not 1", restrictions) << series << false;

    QTest::newRow("false and false") << HangingProtocolImageSetRestrictionExpression("2 and 3", restrictions) << series << false;
    QTest::newRow("false and true") << HangingProtocolImageSetRestrictionExpression("2 and 1", restrictions) << series << false;
    QTest::newRow("true and false") << HangingProtocolImageSetRestrictionExpression("1 and 3", restrictions) << series << false;
    QTest::newRow("true and true") << HangingProtocolImageSetRestrictionExpression("1 and 4", restrictions) << series << true;

    QTest::newRow("false or false") << HangingProtocolImageSetRestrictionExpression("3 or 2", restrictions) << series << false;
    QTest::newRow("false or true") << HangingProtocolImageSetRestrictionExpression("2 or 4", restrictions) << series << true;
    QTest::newRow("true or false") << HangingProtocolImageSetRestrictionExpression("4 or 3", restrictions) << series << true;
    QTest::newRow("true or true") << HangingProtocolImageSetRestrictionExpression("1 or 4", restrictions) << series << true;

    QTest::newRow("parentheses (1)") << HangingProtocolImageSetRestrictionExpression("3 and (4 or 1)", restrictions) << series << false;
    QTest::newRow("parentheses (2)") << HangingProtocolImageSetRestrictionExpression("(3 and 4) or 1", restrictions) << series << true;

    QTest::newRow("and/or precedence") << HangingProtocolImageSetRestrictionExpression("3 and 4 or 1", restrictions) << series << true;

    QTest::newRow("complex expression") << HangingProtocolImageSetRestrictionExpression("not (1 and (2 or not 3))", restrictions) << series << false;
}

void test_HangingProtocolImageSetRestrictionExpression::test_Series_ShouldReturnExpectedValue()
{
    QFETCH(HangingProtocolImageSetRestrictionExpression, restrictionExpression);
    QFETCH(Series*, series);
    QFETCH(bool, expectedValue);

    QCOMPARE(restrictionExpression.test(series), expectedValue);
}

void test_HangingProtocolImageSetRestrictionExpression::test_Image_ShouldReturnExpectedValue_data()
{
    QTest::addColumn<HangingProtocolImageSetRestrictionExpression>("restrictionExpression");
    QTest::addColumn<Image*>("image");
    QTest::addColumn<bool>("expectedValue");

    QMap<int, HangingProtocolImageSetRestriction> restrictions;
    restrictions[1] = HangingProtocolImageSetRestriction(1, "ViewPosition", "LATERAL", 0);
    restrictions[2] = HangingProtocolImageSetRestriction(2, "ImageLaterality", "L", 0);
    restrictions[3] = HangingProtocolImageSetRestriction(3, "CodeMeaning", "lateral", 0);
    restrictions[4] = HangingProtocolImageSetRestriction(4, "ImageType", "LOCALIZER", 0);

    Image *image = new Image(this);
    image->setImageLaterality('R');
    image->setImageType("LOCALIZER");
    image->setViewCodeMeaning("cranio-caudal");
    image->setViewPosition("LATERAL");

    QTest::newRow("empty expression") << HangingProtocolImageSetRestrictionExpression() << image << true;
    QTest::newRow("garbage expression") << HangingProtocolImageSetRestrictionExpression("0 9a's0df9as0", restrictions) << image << true;

    QTest::newRow("one restriction, true") << HangingProtocolImageSetRestrictionExpression("1", restrictions) << image << true;
    QTest::newRow("one restriction, false") << HangingProtocolImageSetRestrictionExpression("2", restrictions) << image << false;

    QTest::newRow("not false") << HangingProtocolImageSetRestrictionExpression("not 2", restrictions) << image << true;
    QTest::newRow("not true") << HangingProtocolImageSetRestrictionExpression("not 1", restrictions) << image << false;

    QTest::newRow("false and false") << HangingProtocolImageSetRestrictionExpression("2 and 3", restrictions) << image << false;
    QTest::newRow("false and true") << HangingProtocolImageSetRestrictionExpression("2 and 1", restrictions) << image << false;
    QTest::newRow("true and false") << HangingProtocolImageSetRestrictionExpression("1 and 3", restrictions) << image << false;
    QTest::newRow("true and true") << HangingProtocolImageSetRestrictionExpression("1 and 4", restrictions) << image << true;

    QTest::newRow("false or false") << HangingProtocolImageSetRestrictionExpression("3 or 2", restrictions) << image << false;
    QTest::newRow("false or true") << HangingProtocolImageSetRestrictionExpression("2 or 4", restrictions) << image << true;
    QTest::newRow("true or false") << HangingProtocolImageSetRestrictionExpression("4 or 3", restrictions) << image << true;
    QTest::newRow("true or true") << HangingProtocolImageSetRestrictionExpression("1 or 4", restrictions) << image << true;

    QTest::newRow("parentheses (1)") << HangingProtocolImageSetRestrictionExpression("3 and (4 or 1)", restrictions) << image << false;
    QTest::newRow("parentheses (2)") << HangingProtocolImageSetRestrictionExpression("(3 and 4) or 1", restrictions) << image << true;

    QTest::newRow("and/or precedence") << HangingProtocolImageSetRestrictionExpression("3 and 4 or 1", restrictions) << image << true;

    QTest::newRow("complex expression") << HangingProtocolImageSetRestrictionExpression("not (1 and (2 or not 3))", restrictions) << image << false;
}

void test_HangingProtocolImageSetRestrictionExpression::test_Image_ShouldReturnExpectedValue()
{
    QFETCH(HangingProtocolImageSetRestrictionExpression, restrictionExpression);
    QFETCH(Image*, image);
    QFETCH(bool, expectedValue);

    QCOMPARE(restrictionExpression.test(image), expectedValue);
}

DECLARE_TEST(test_HangingProtocolImageSetRestrictionExpression)

#include "test_hangingprotocolimagesetrestrictionexpression.moc"
