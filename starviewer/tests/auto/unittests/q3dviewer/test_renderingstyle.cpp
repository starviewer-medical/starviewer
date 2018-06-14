#include "autotest.h"
#include "renderingstyle.h"
#include <QMap>
#include <QVariant>

using namespace udg;

class test_RenderingStyle : public QObject {
Q_OBJECT

private slots:
    void toVariant_ShouldReturnCorrectVariant_data();
    void toVariant_ShouldReturnCorrectVariant();

    void fromVariant_ShouldReturnCorrectRenderingStyle_data();
    void fromVariant_ShouldReturnCorrectRenderingStyle();

private:
    void setupData();
};

Q_DECLARE_METATYPE(RenderingStyle)

void test_RenderingStyle::toVariant_ShouldReturnCorrectVariant_data()
{
    setupData();
}

void test_RenderingStyle::toVariant_ShouldReturnCorrectVariant()
{
    QFETCH(RenderingStyle, renderingStyle);
    QFETCH(QVariant, variant);

    QCOMPARE(renderingStyle.toVariant(), variant);
}

void test_RenderingStyle::fromVariant_ShouldReturnCorrectRenderingStyle_data()
{
    setupData();
}

void test_RenderingStyle::fromVariant_ShouldReturnCorrectRenderingStyle()
{
    QFETCH(RenderingStyle, renderingStyle);
    QFETCH(QVariant, variant);

    RenderingStyle fromVariant = RenderingStyle::fromVariant(variant);

    QCOMPARE(fromVariant.getMethod(), renderingStyle.getMethod());
    QCOMPARE(fromVariant.getShading(), renderingStyle.getShading());
    QCOMPARE(fromVariant.getAmbientCoefficient(), renderingStyle.getAmbientCoefficient());
    QCOMPARE(fromVariant.getDiffuseCoefficient(), renderingStyle.getDiffuseCoefficient());
    QCOMPARE(fromVariant.getSpecularCoefficient(), renderingStyle.getSpecularCoefficient());
    QCOMPARE(fromVariant.getSpecularPower(), renderingStyle.getSpecularPower());
    QCOMPARE(fromVariant.getTransferFunction(), renderingStyle.getTransferFunction());
    QCOMPARE(fromVariant.getIsoValue(), renderingStyle.getIsoValue());
}

void test_RenderingStyle::setupData()
{
    QTest::addColumn<RenderingStyle>("renderingStyle");
    QTest::addColumn<QVariant>("variant");

    const int NumberOfRows = 3;

    RenderingStyle::Method methods[NumberOfRows] = { RenderingStyle::MIP, RenderingStyle::RayCasting, RenderingStyle::IsoSurface };
    bool shadings[NumberOfRows] = { false, false, true };
    double ambientCoefficients[NumberOfRows] = { 0.26, 0.74, 1.0 };
    double diffuseCoefficients[NumberOfRows] = { 0.65, 0.57, 0.88 };
    double specularCoefficients[NumberOfRows] = { 0.79, 0.77, 0.07 };
    double specularPowers[NumberOfRows] = { 35.2, 109.9, 90.3 };
    TransferFunction transferFunctions[NumberOfRows];
    double isoValues[NumberOfRows] = { -202.3, -3058.7, -4874.5 };

    transferFunctions[0].setColor(387.4, 0.633, 0.942, 0.967);
    transferFunctions[0].setScalarOpacity(-2752.2, 0.175);
    transferFunctions[0].setScalarOpacity(4056.1, 0.505);
    transferFunctions[0].setScalarOpacity(-2708.4, 0.994);
    transferFunctions[0].setScalarOpacity(-2029.1, 0.622);
    transferFunctions[0].setScalarOpacity(-3106.9, 0.879);
    transferFunctions[0].setGradientOpacity(-13470, 0.325);
    transferFunctions[0].setGradientOpacity(1978.1, 0.097);
    transferFunctions[0].setGradientOpacity(-2688.8, 0.056);
    transferFunctions[0].setGradientOpacity(-3359.8, 0.759);
    transferFunctions[0].setGradientOpacity(-4011.6, 0.168);
    transferFunctions[0].setGradientOpacity(1212.5, 0.188);

    transferFunctions[1].setColor(-3106.9, 0.772, 0.224, 0.367);
    transferFunctions[1].setColor(-2440.0, 0.755, 0.477, 0.985);
    transferFunctions[1].setScalarOpacity(2435.6, 0.194);
    transferFunctions[1].setGradientOpacity(-3565.0, 0.780);
    transferFunctions[1].setGradientOpacity(1992.3, 0.097);
    transferFunctions[1].setGradientOpacity(-4302.5, 0.787);
    transferFunctions[1].setGradientOpacity(-1821.0, 0.706);
    transferFunctions[1].setGradientOpacity(1831.7, 0.901);
    transferFunctions[1].setGradientOpacity(-3223.1, 0.949);

    transferFunctions[2].setScalarOpacity(-1917.3, 0.381);
    transferFunctions[2].setScalarOpacity(-4087.6, 0.252);
    transferFunctions[2].setScalarOpacity(-4058.2, 0.198);
    transferFunctions[2].setGradientOpacity(-2679.2, 0.759);
    transferFunctions[2].setGradientOpacity(-4645.9, 0.027);
    transferFunctions[2].setGradientOpacity(-4827.1, 0.457);
    transferFunctions[2].setGradientOpacity(151.5, 0.432);
    transferFunctions[2].setGradientOpacity(4893.8, 0.737);

    for (int i = 0; i < NumberOfRows; i++)
    {
        RenderingStyle renderingStyle;
        QMap<QString, QVariant> map;
        renderingStyle.setMethod(methods[i]);
        map["method"] = methods[i];
        renderingStyle.setShading(shadings[i]);
        map["shading"] = shadings[i];
        renderingStyle.setAmbientCoefficient(ambientCoefficients[i]);
        map["ambientCoefficient"] = ambientCoefficients[i];
        renderingStyle.setDiffuseCoefficient(diffuseCoefficients[i]);
        map["diffuseCoefficient"] = diffuseCoefficients[i];
        renderingStyle.setSpecularCoefficient(specularCoefficients[i]);
        map["specularCoefficient"] = specularCoefficients[i];
        renderingStyle.setSpecularPower(specularPowers[i]);
        map["specularPower"] = specularPowers[i];
        renderingStyle.setTransferFunction(transferFunctions[i]);
        map["transferFunction"] = transferFunctions[i].toVariant();
        renderingStyle.setIsoValue(isoValues[i]);
        map["isoValue"] = isoValues[i];
        QTest::newRow(qPrintable(QString("random (%1)").arg(i))) << renderingStyle << QVariant(map);
    }
}

DECLARE_TEST(test_RenderingStyle)

#include "test_renderingstyle.moc"
