#include "autotest.h"
#include "imageoverlay.h"

using namespace udg;

class test_ImageOverlay : public QObject {
Q_OBJECT

private slots:
    void ImageOverlay_InitializesClassAsExpected();
};

void test_ImageOverlay::ImageOverlay_InitializesClassAsExpected()
{    
    ImageOverlay overlay;
    unsigned int uintZero = 0;
    QCOMPARE(overlay.getRows(), uintZero);
    QCOMPARE(overlay.getColumns(), uintZero);
    QCOMPARE(overlay.getXOrigin(), 1);
    QCOMPARE(overlay.getYOrigin(), 1);
    unsigned char *nullUCharPointer = 0;
    QCOMPARE(overlay.getData(), nullUCharPointer);
}

DECLARE_TEST(test_ImageOverlay)

#include "test_imageoverlay.moc"
