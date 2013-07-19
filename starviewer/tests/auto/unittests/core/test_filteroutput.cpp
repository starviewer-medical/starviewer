#include "autotest.h"
#include "filteroutput.h"

#include "filter.h"

#include <vtkBooleanTexture.h>
#include <vtkImageData.h>

using namespace udg;

namespace {

class TestingFilter: public Filter {
public:
    TestingFilter() : m_updated(false)
    {
        m_algorithm = vtkBooleanTexture::New();
    }
    virtual ~TestingFilter()
    {
        m_algorithm->Delete();
    }
    virtual void update()
    {
        Filter::update();
        m_updated = true;
    }
public:
    vtkBooleanTexture *m_algorithm;
    bool m_updated;
private:
    virtual vtkAlgorithm* getVtkAlgorithm() const
    {
        return m_algorithm;
    }
};

}

class test_FilterOutput : public QObject {

    Q_OBJECT

private slots:
    void getFilter_ShouldReturnTheAssociatedFilter();
    void getVtkAlgorithmOutput_ShouldReturnVtkAlgorithmOutputFromTheAssociatedFilter();
    void getVtkImageData_ShouldUpdateTheAssociatedFilterAndReturnItsOutputDataObject();

};

void test_FilterOutput::getFilter_ShouldReturnTheAssociatedFilter()
{
    TestingFilter filter;
    FilterOutput output(&filter);

    QCOMPARE(output.getFilter(), &filter);
}

void test_FilterOutput::getVtkAlgorithmOutput_ShouldReturnVtkAlgorithmOutputFromTheAssociatedFilter()
{
    TestingFilter filter;
    FilterOutput output(&filter);

    QCOMPARE(output.getVtkAlgorithmOutput(), filter.m_algorithm->GetOutputPort());
}

void test_FilterOutput::getVtkImageData_ShouldUpdateTheAssociatedFilterAndReturnItsOutputDataObject()
{
    TestingFilter filter;
    FilterOutput output(&filter);

    QCOMPARE(output.getVtkImageData(), static_cast<vtkImageData*>(filter.m_algorithm->GetOutputDataObject(0)));
    QVERIFY(filter.m_updated);
}

DECLARE_TEST(test_FilterOutput)

#include "test_filteroutput.moc"

