#include "autotest.h"
#include "relativegeometrylayout.h"

using namespace udg;

class test_RelativeGeometryLayout : public QObject {
    Q_OBJECT

private slots:
    void addItem_ShouldAddItemWithTheGivenGeometry();
    void addWidget_ShouldAddWidgetWithTheGivenGeometry();

    void geometry_item_ShouldReturnExpectedGeometry();
    void geometry_widget_ShouldReturnExpectedGeometry();
    void geometryAt_ShouldReturnExpectedGeometry();

    void setGeometry_item_ShouldSetGeometry();
    void setGeometry_widget_ShouldSetGeometry();
    void setGeometryAt_ShouldSetGeometry();

    void count_ShouldReturnExpectedNumber();

    void itemAt_ShouldReturnExpectedItem();
    void takeAt_ShouldRemoveAndReturnExpectedItem();

    void setGeometry_ShouldSetTheGeometryOfAllWidgets();

    void sizeHint_ShouldReturnNullSize();

};

void test_RelativeGeometryLayout::addItem_ShouldAddItemWithTheGivenGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QLayoutItem *item = new QWidgetItem(&widget);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addItem(item, geometry);

    QCOMPARE(layout.itemAt(0), item);
    QCOMPARE(layout.geometryAt(0), geometry);
}

void test_RelativeGeometryLayout::addWidget_ShouldAddWidgetWithTheGivenGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);

    QCOMPARE(layout.indexOf(&widget), 0);
    QCOMPARE(layout.geometryAt(0), geometry);
}

void test_RelativeGeometryLayout::geometry_item_ShouldReturnExpectedGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QLayoutItem *item = new QWidgetItem(&widget);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addItem(item, geometry);

    QCOMPARE(layout.geometry(item), geometry);

    QWidget widget2;
    QWidgetItem item2(&widget2);

    QCOMPARE(layout.geometry(&item2), QRectF());
}

void test_RelativeGeometryLayout::geometry_widget_ShouldReturnExpectedGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);

    QCOMPARE(layout.geometry(&widget), geometry);

    QWidget widget2;

    QCOMPARE(layout.geometry(&widget2), QRectF());
}

void test_RelativeGeometryLayout::geometryAt_ShouldReturnExpectedGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);

    QCOMPARE(layout.geometryAt(0), geometry);

    QCOMPARE(layout.geometryAt(1), QRectF());
}

void test_RelativeGeometryLayout::setGeometry_item_ShouldSetGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QLayoutItem *item = new QWidgetItem(&widget);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addItem(item, geometry);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.setGeometry(item, geometry2);

    QCOMPARE(layout.geometry(item), geometry2);

    QWidget widget2;
    QWidgetItem item2(&widget2);
    layout.setGeometry(&item2, geometry);
}

void test_RelativeGeometryLayout::setGeometry_widget_ShouldSetGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.setGeometry(&widget, geometry2);

    QCOMPARE(layout.geometry(&widget), geometry2);

    QWidget widget2;
    layout.setGeometry(&widget2, geometry);
}

void test_RelativeGeometryLayout::setGeometryAt_ShouldSetGeometry()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.setGeometryAt(0, geometry2);

    QCOMPARE(layout.geometryAt(0), geometry2);

    layout.setGeometryAt(1, geometry);
}

void test_RelativeGeometryLayout::count_ShouldReturnExpectedNumber()
{
    RelativeGeometryLayout layout;

    QCOMPARE(layout.count(), 0);

    QWidget widget;
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addWidget(&widget, geometry);

    QCOMPARE(layout.count(), 1);

    QWidget widget2;
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.addWidget(&widget2, geometry2);

    QCOMPARE(layout.count(), 2);

    layout.takeAt(1);

    QCOMPARE(layout.count(), 1);
}

void test_RelativeGeometryLayout::itemAt_ShouldReturnExpectedItem()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QLayoutItem *item = new QWidgetItem(&widget);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addItem(item, geometry);
    QWidget widget2;
    QLayoutItem *item2 = new QWidgetItem(&widget2);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.addItem(item2, geometry2);

    QCOMPARE(layout.itemAt(0), item);
    QCOMPARE(layout.itemAt(1), item2);
    QCOMPARE(layout.itemAt(2), static_cast<QLayoutItem*>(0));
}

void test_RelativeGeometryLayout::takeAt_ShouldRemoveAndReturnExpectedItem()
{
    RelativeGeometryLayout layout;
    QWidget widget;
    QWidgetItem item(&widget);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout.addItem(&item, geometry);
    QWidget widget2;
    QWidgetItem item2(&widget2);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout.addItem(&item2, geometry2);

    QCOMPARE(layout.takeAt(0), &item);
    QCOMPARE(layout.takeAt(0), &item2);
    QCOMPARE(layout.takeAt(0), static_cast<QLayoutItem*>(0));
}

void test_RelativeGeometryLayout::setGeometry_ShouldSetTheGeometryOfAllWidgets()
{
    QWidget parent;
    RelativeGeometryLayout *layout = new RelativeGeometryLayout(&parent);
    QWidget *widget = new QWidget(&parent);
    QRectF geometry(0.6, 0.0, 0.2, 0.8);
    layout->addWidget(widget, geometry);
    QWidget *widget2 = new QWidget(&parent);
    QRectF geometry2(0.4, 0.3, 0.5, 0.5);
    layout->addWidget(widget2, geometry2);
    layout->setGeometry(QRect(0, 0, 10, 10));

    QCOMPARE(widget->geometry(), QRect(6, 0, 2, 8));
    QCOMPARE(widget2->geometry(), QRect(4, 3, 5, 5));
}

void test_RelativeGeometryLayout::sizeHint_ShouldReturnNullSize()
{
    RelativeGeometryLayout layout;

    QCOMPARE(layout.sizeHint(), QSize());
}

DECLARE_TEST(test_RelativeGeometryLayout)

#include "test_relativegeometrylayout.moc"
