#include "autotest.h"
#include "qmrusortedtoolbuttonwithmenu.h"

#include <QAction>
#include <QMenu>

using namespace udg;

class test_QMruSortedToolButtonWithMenu : public QObject {

    Q_OBJECT

private slots:
    void addAction_ShouldNotAddNullAction();
    void addAction_ShouldNotAddDuplicateAction();
    void addAction_ShouldAddActionsToCorrectPlace();

    void addActions_ShouldAddAllActionsInOrder();

    void actions_ShouldReturnEmptyList();
    void actions_ShouldReturnActionsInCorrectOrder();

    void hasAction_ShouldReturnExpectedValue();

    void setDefaultAction_ShouldIgnoreNullAction();
    void setDefaultAction_ShouldAddAction();
    void setDefaultAction_ShouldRearrangeActionsAsExpected();

    void class_TriggeredActionsShouldBeRearranged();

};

void test_QMruSortedToolButtonWithMenu::addAction_ShouldNotAddNullAction()
{
    QMruSortedToolButtonWithMenu button;
    button.addAction(new QAction(this));    // add an action to have a default action
    button.addAction(nullptr);

    QVERIFY(button.menu()->actions().isEmpty());
}

void test_QMruSortedToolButtonWithMenu::addAction_ShouldNotAddDuplicateAction()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action = new QAction(this);
    button.addAction(action);
    button.addAction(action);

    QVERIFY(button.actions().size() == 1);
}

void test_QMruSortedToolButtonWithMenu::addAction_ShouldAddActionsToCorrectPlace()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    QAction *action3 = new QAction(this);
    button.addAction(action1);
    button.addAction(action2);
    button.addAction(action3);
    QList<QAction*> actions = button.actions();

    QCOMPARE(actions[0], action1);
    QCOMPARE(actions[1], action2);
    QCOMPARE(actions[2], action3);
}

void test_QMruSortedToolButtonWithMenu::addActions_ShouldAddAllActionsInOrder()
{
    QMruSortedToolButtonWithMenu button;
    QList<QAction*> actions;
    actions << new QAction(this) << new QAction(this) << new QAction(this);
    button.addActions(actions);

    QCOMPARE(button.actions(), actions);
}

void test_QMruSortedToolButtonWithMenu::actions_ShouldReturnEmptyList()
{
    QMruSortedToolButtonWithMenu button;

    QVERIFY(button.actions().isEmpty());
}

void test_QMruSortedToolButtonWithMenu::actions_ShouldReturnActionsInCorrectOrder()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    QAction *action3 = new QAction(this);
    button.addAction(action1);
    button.addAction(action2);
    button.addAction(action3);
    QList<QAction*> actions = button.actions();

    QCOMPARE(actions[0], button.defaultAction());
    QCOMPARE(actions[1], button.menu()->actions()[0]);
    QCOMPARE(actions[2], button.menu()->actions()[1]);
}

void test_QMruSortedToolButtonWithMenu::hasAction_ShouldReturnExpectedValue()
{
    QMruSortedToolButtonWithMenu button;

    QCOMPARE(button.hasAction(nullptr), false);

    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    QAction *action3 = new QAction(this);
    button.addAction(action1);
    button.addAction(action2);

    QCOMPARE(button.hasAction(action1), true);
    QCOMPARE(button.hasAction(action2), true);
    QCOMPARE(button.hasAction(action3), false);
}

void test_QMruSortedToolButtonWithMenu::setDefaultAction_ShouldIgnoreNullAction()
{
    QMruSortedToolButtonWithMenu button;
    button.addAction(new QAction(this));    // add an action to have a default action
    button.setDefaultAction(nullptr);

    QVERIFY(button.defaultAction() != nullptr);
}

void test_QMruSortedToolButtonWithMenu::setDefaultAction_ShouldAddAction()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    button.setDefaultAction(action1);
    button.setDefaultAction(action2);

    QVERIFY(button.hasAction(action1));
    QVERIFY(button.hasAction(action2));
}

void test_QMruSortedToolButtonWithMenu::setDefaultAction_ShouldRearrangeActionsAsExpected()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    QAction *action3 = new QAction(this);
    button.addAction(action1);
    button.addAction(action2);
    button.addAction(action3);

    button.setDefaultAction(action1);
    QCOMPARE(button.actions(), QList<QAction*>() << action1 << action2 << action3);

    button.setDefaultAction(action3);
    QCOMPARE(button.actions(), QList<QAction*>() << action3 << action1 << action2);

    button.setDefaultAction(action1);
    QCOMPARE(button.actions(), QList<QAction*>() << action1 << action3 << action2);

    button.setDefaultAction(action2);
    QCOMPARE(button.actions(), QList<QAction*>() << action2 << action1 << action3);
}

void test_QMruSortedToolButtonWithMenu::class_TriggeredActionsShouldBeRearranged()
{
    QMruSortedToolButtonWithMenu button;
    QAction *action1 = new QAction(this);
    QAction *action2 = new QAction(this);
    QAction *action3 = new QAction(this);
    button.addAction(action1);
    button.addAction(action2);
    button.addAction(action3);

    action3->trigger();
    QCOMPARE(button.actions(), QList<QAction*>() << action3 << action1 << action2);

    action2->trigger();
    QCOMPARE(button.actions(), QList<QAction*>() << action2 << action3 << action1);

    action3->trigger();
    QCOMPARE(button.actions(), QList<QAction*>() << action3 << action2 << action1);

    action1->trigger();
    QCOMPARE(button.actions(), QList<QAction*>() << action1 << action3 << action2);
}

DECLARE_TEST(test_QMruSortedToolButtonWithMenu)

#include "test_qmrusortedtoolbuttonwithmenu.moc"
