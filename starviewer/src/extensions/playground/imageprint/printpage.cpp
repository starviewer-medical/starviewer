#include "printpage.h"
#include "image.h"

namespace udg 
{

void PrintPage::setPageNumber(int pageNumber)
{
    m_pageNumber = pageNumber;
}

int PrintPage::getPageNumber()
{
    return m_pageNumber;
}

void PrintPage::setImagesToPrint(QList<Image*> imagesToPrint)
{
    m_imagesToPrint = imagesToPrint;
}

QList<Image*> PrintPage::getImagesToPrint()
{
    return m_imagesToPrint;
}
}