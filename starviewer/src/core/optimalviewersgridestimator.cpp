#include "optimalviewersgridestimator.h"
#include "mathtools.h"

namespace udg {

OptimalViewersGridEstimator::OptimalViewersGridEstimator()
{
    // El número de columnes haurà de ser sempre parell per no tenir problemes de que la imatge quedi partida.
    // Com a mínim el grid serà de 2 columnes x 1 fila
    m_gridsMap.insert(2, QPair<int, int>(1, 2));
    m_gridsMap.insert(4, QPair<int, int>(2, 2));
    m_gridsMap.insert(6, QPair<int, int>(3, 2));
    m_gridsMap.insert(8, QPair<int, int>(2, 4)); // Es podria afegir un 4,2 com alternativa
    m_gridsMap.insert(12, QPair<int, int>(3, 4));
    m_gridsMap.insert(16, QPair<int, int>(4, 4));
    m_gridsMap.insert(18, QPair<int, int>(3, 6));
    m_gridsMap.insert(20, QPair<int, int>(5, 4));
    m_gridsMap.insert(24, QPair<int, int>(4, 6)); // Es podria afegir un 6,4 com alternativa
    m_gridsMap.insert(28, QPair<int, int>(7, 4));
    m_gridsMap.insert(30, QPair<int, int>(5, 6));
    m_gridsMap.insert(32, QPair<int, int>(4, 8)); // Es podria afegir un 8,4 com alternativa
    m_gridsMap.insert(36, QPair<int, int>(6, 6));
    m_gridsMap.insert(40, QPair<int, int>(5, 8));
    m_gridsMap.insert(42, QPair<int, int>(7, 6));
    m_gridsMap.insert(48, QPair<int, int>(6, 8)); // Es podria afegir un 8,6 com alternativa

    QMapIterator<int, QPair<int, int> > iterator(m_gridsMap);
    iterator.toBack();
    if (iterator.hasPrevious())
    {
        iterator.previous();
        m_maximumNumberOfViewers = iterator.key();
    }
    else
    {
        m_maximumNumberOfViewers = 0;
    }
}

OptimalViewersGridEstimator::~OptimalViewersGridEstimator()
{
}

QPair<int, int> OptimalViewersGridEstimator::getOptimalGrid(int minimumCells)
{
    int optimalNumberOfCells = MathTools::roundUpToMultipleOfNumber(minimumCells, 2);

    while (!m_gridsMap.contains(optimalNumberOfCells) && optimalNumberOfCells <= m_maximumNumberOfViewers)
    {
        optimalNumberOfCells += 2;
    }
    
    return m_gridsMap.value((optimalNumberOfCells > m_maximumNumberOfViewers) ? m_maximumNumberOfViewers : optimalNumberOfCells);

}

} // End namespace udg
