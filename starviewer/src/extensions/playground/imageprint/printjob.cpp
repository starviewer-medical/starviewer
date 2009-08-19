#include "printjob.h"

namespace udg
{
void PrintJob::setNumberOfCopies(int numberOfCopies)
{
    m_numberOfCopies = numberOfCopies;
}

int PrintJob::getNumberOfCopies()
{
   return m_numberOfCopies;
}
}