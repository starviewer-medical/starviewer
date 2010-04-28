/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTRANSDIFFERENCETOOLDATA_H
#define UDGTRANSDIFFERENCETOOLDATA_H

#include "tooldata.h"
#include <QPair>
#include <QVector>

namespace udg {

class Q2DViewer;
class Volume;

/**
    Classe per guardar les dades de la tool de diferència amb translació.

    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class TransDifferenceToolData : public ToolData {
Q_OBJECT
public:
    TransDifferenceToolData();
    ~TransDifferenceToolData();

    /// Set del volum d'entrada
    void setInputVolume(Volume* input);

    /// Get del volum d'entrada
    Volume* getInputVolume(){return m_inputVolume;}

    /// Set del volum diferència
    void setDifferenceVolume(Volume* input);

    /// Get del volum diferència
    Volume* getDifferenceVolume(){return m_differenceVolume;}

    /// Get X de la translacio
    int getSliceTranslationX( int sl ){return m_sliceTranslations[sl].first;}

    /// Set X de la translacio
    void setSliceTranslationX( int sl, int trX ){m_sliceTranslations[sl].first = trX;}

    /// Increase X de la translacio
    void increaseSliceTranslationX( int sl, int trX ){m_sliceTranslations[sl].first += trX;}

    /// Get Y de la translacio
    int getSliceTranslationY( int sl ){return m_sliceTranslations[sl].second;}

    /// Set Y de la translacio
    void setSliceTranslationY( int sl, int trY ){m_sliceTranslations[sl].second = trY;}

    /// Increase Y de la translacio
    void increaseSliceTranslationY( int sl, int trY ){m_sliceTranslations[sl].second += trY;}

    /// Get la llesca de referència
    int getReferenceSlice( ){return m_referenceSlice;}

    /// Set la llesca de referència
    void setReferenceSlice( int sl ){m_referenceSlice = sl;}

    void setActualDisplacement(int dx, int dy){emit actualDisplacement(dx,dy);}

    void setFinalDisplacement(int dx, int dy){emit finalDisplacement(dx,dy);}

public slots:

signals:
    /// Envia el desplaçament que s'ha fet des de la posició d'origen
    void actualDisplacement(int,int);

    /// Envia el desplaçament que s'ha fet des de la posició d'origen
    void finalDisplacement(int,int);

private:
    /// Dades de les transformacions aplicades a cada llesca
    QVector< QPair<int,int> > m_sliceTranslations;

    ///Dades del volum original i la diferència
    Volume* m_inputVolume;
    Volume* m_differenceVolume;

    int m_dx,m_dy;

    /// Slice de referència
    int m_referenceSlice;
};

}

#endif
