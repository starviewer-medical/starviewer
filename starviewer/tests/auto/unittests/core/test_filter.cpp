#include "autotest.h"
#include "filter.h"

#include "filteroutput.h"

#include <vtkAlgorithm.h>

using namespace udg;

namespace {

class TestingVtkAlgorithm : public vtkAlgorithm {
public:
    TestingVtkAlgorithm() : m_updated(false) {}
    ~TestingVtkAlgorithm()
    {
        this->SetReferenceCount(0);
    }
    virtual void Update()
    {
        m_updated = true;
    }
public:
    bool m_updated;
};

class TestingFilter: public Filter {
public:
    TestingFilter()
    {
        m_algorithm = new TestingVtkAlgorithm();
    }
    virtual ~TestingFilter()
    {
        delete m_algorithm;
    }
public:
    TestingVtkAlgorithm *m_algorithm;
private:
    virtual vtkAlgorithm* getVtkAlgorithm() const
    {
        return m_algorithm;
    }
};

}

class test_Filter : public QObject {

    Q_OBJECT

private slots:
    void update_ShouldCallUpdateOnTheAlgorithm();
    void getOutput_ShouldReturnFilterOutputReferencingThis();

};

void test_Filter::update_ShouldCallUpdateOnTheAlgorithm()
{
    TestingFilter filter;
    filter.update();

    QVERIFY(filter.m_algorithm->m_updated);
}

void test_Filter::getOutput_ShouldReturnFilterOutputReferencingThis()
{
    TestingFilter filter;
    FilterOutput output = filter.getOutput();

    QCOMPARE(output.getFilter(), &filter);
}

DECLARE_TEST(test_Filter)

#include "test_filter.moc"

