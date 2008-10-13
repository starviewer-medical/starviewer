#include "trilinearinterpolator.h"


namespace udg {


TrilinearInterpolator::TrilinearInterpolator()
{
    for ( int i = 0; i < 8; i++ ) m_increments[i] = 0;
}


TrilinearInterpolator::~TrilinearInterpolator()
{
}


}
