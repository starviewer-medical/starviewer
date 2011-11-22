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
    QCOMPARE(fromVariant.getDiffuseLighting(), renderingStyle.getDiffuseLighting());
    QCOMPARE(fromVariant.getSpecularLighting(), renderingStyle.getSpecularLighting());
    QCOMPARE(fromVariant.getSpecularPower(), renderingStyle.getSpecularPower());
    QCOMPARE(fromVariant.getTransferFunction(), renderingStyle.getTransferFunction());
    QCOMPARE(fromVariant.getContour(), renderingStyle.getContour());
    QCOMPARE(fromVariant.getContourThreshold(), renderingStyle.getContourThreshold());
    QCOMPARE(fromVariant.getObscurance(), renderingStyle.getObscurance());
    QCOMPARE(fromVariant.getObscuranceQuality(), renderingStyle.getObscuranceQuality());
    QCOMPARE(fromVariant.getObscuranceFactor(), renderingStyle.getObscuranceFactor());
    QCOMPARE(fromVariant.getIsoValue(), renderingStyle.getIsoValue());
}

void test_RenderingStyle::setupData()
{
    QTest::addColumn<RenderingStyle>("renderingStyle");
    QTest::addColumn<QVariant>("variant");

    const int NumberOfRows = 6;

    RenderingStyle::Method methods[NumberOfRows] = { RenderingStyle::MIP, RenderingStyle::RayCasting, RenderingStyle::IsoSurface,
                                                     RenderingStyle::Contouring, RenderingStyle::Texture2D, RenderingStyle::Texture3D };
    bool diffuseLightings[NumberOfRows] = { false, false, false, false, true, true };
    bool specularLightings[NumberOfRows] = { false, false, true, false, false, false };
    double specularPowers[NumberOfRows] = { 35.2, 109.9, 90.3, 29.8, 125.0, 838.0 };
    TransferFunction transferFunctions[NumberOfRows];
    bool contours[NumberOfRows] = { false, false, false, false, true, false };
    double contourThresholds[NumberOfRows] = { 0.61, 0.78, 0.25, 0.46, 0.25, 0.27 };
    bool obscurances[NumberOfRows] = { false, false, true, true, true, false };
    RenderingStyle::ObscuranceQuality obscuranceQualities[NumberOfRows] = { RenderingStyle::High, RenderingStyle::Low, RenderingStyle::Low,
                                                                            RenderingStyle::Medium, RenderingStyle::Medium, RenderingStyle::Low };
    double obscuranceFactors[NumberOfRows] = { 6.97, 9.04, 2.81, 6.60, 5.31, 5.96 };
    double isoValues[NumberOfRows] = { -202.3, -3058.7, -4874.5, -732.5, -3984.9, 2574.8 };

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

    transferFunctions[3].setColor(-3995.1, 0.096, 0.969, 0.176);
    transferFunctions[3].setColor(-3403.6, 0.380, 0.761, 0.794);
    transferFunctions[3].setColor(2396.9, 0.040, 0.888, 0.309);
    transferFunctions[3].setColor(2618.3, 0.820, 0.106, 0.306);
    transferFunctions[3].setColor(2436.1, 0.297, 0.829, 0.145);
    transferFunctions[3].setScalarOpacity(-2972.8, 0.769);
    transferFunctions[3].setScalarOpacity(-1387.0, 0.567);

    transferFunctions[4].setColor(882.6, 0.117, 0.328, 0.172);
    transferFunctions[4].setColor(4921.7, 0.988, 0.510, 0.196);
    transferFunctions[4].setScalarOpacity(2971.1, 0.532);
    transferFunctions[4].setScalarOpacity(-4835.2, 0.513);
    transferFunctions[4].setScalarOpacity(1610.8, 0.311);
    transferFunctions[4].setScalarOpacity(-173.6, 0.134);
    transferFunctions[4].setScalarOpacity(1288.5, 0.014);
    transferFunctions[4].setScalarOpacity(2786.0, 0.061);
    transferFunctions[4].setGradientOpacity(-4923.5, 0.344);
    transferFunctions[4].setGradientOpacity(-922.2, 0.210);
    transferFunctions[4].setGradientOpacity(2833.1, 0.913);
    transferFunctions[4].setGradientOpacity(-890.9, 0.339);

    transferFunctions[5].setGradientOpacity(2518.4, 0.119);

    for (int i = 0; i < NumberOfRows; i++)
    {
        RenderingStyle renderingStyle;
        QMap<QString, QVariant> map;
        renderingStyle.setMethod(methods[i]);
        map["method"] = methods[i];
        renderingStyle.setDiffuseLighting(diffuseLightings[i]);
        map["diffuseLighting"] = diffuseLightings[i];
        renderingStyle.setSpecularLighting(specularLightings[i]);
        map["specularLighting"] = specularLightings[i];
        renderingStyle.setSpecularPower(specularPowers[i]);
        map["specularPower"] = specularPowers[i];
        renderingStyle.setTransferFunction(transferFunctions[i]);
        map["transferFunction"] = transferFunctions[i].toVariant();
        renderingStyle.setContour(contours[i]);
        map["contour"] = contours[i];
        renderingStyle.setContourThreshold(contourThresholds[i]);
        map["contourThreshold"] = contourThresholds[i];
        renderingStyle.setObscurance(obscurances[i]);
        map["obscurance"] = obscurances[i];
        renderingStyle.setObscuranceQuality(obscuranceQualities[i]);
        map["obscuranceQuality"] = obscuranceQualities[i];
        renderingStyle.setObscuranceFactor(obscuranceFactors[i]);
        map["obscuranceFactor"] = obscuranceFactors[i];
        renderingStyle.setIsoValue(isoValues[i]);
        map["isoValue"] = isoValues[i];
        QTest::newRow(qPrintable(QString("random (%1)").arg(i))) << renderingStyle << QVariant(map);
    }
}

DECLARE_TEST(test_RenderingStyle)

#include "test_renderingstyle.moc"
