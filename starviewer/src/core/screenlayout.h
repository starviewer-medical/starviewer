/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGSCREENLAYOUT_H
#define UDGSCREENLAYOUT_H

#include <QList>
#include <QRect>

#include "screen.h"

namespace udg {

/**
    Classe per representar la distribució de pantalles
 */
class ScreenLayout {
public:
    ScreenLayout();
    ~ScreenLayout();

    /// Afegeix una pantalla al layout. Si la pantalla no és vàlida per afegir-se al layout actual, no s'afegirà i es retornarà fals, cert altrament.
    /// No s'afegiran pantalles amb ID's invàlids (<0) o que tinguin el mateix ID que una pantalla existent al layout.
    /// Tampoc s'afegirà una pantalla si aquesta és principal i ja n'existeix una al layout.
    bool addScreen(const Screen &screen);

    /// Retorna el nombre de pantalles al layout
    int getNumberOfScreens() const;
    
    /// Reseteja el layout eliminant totes les pantalles que contenia, deixant-lo buit
    void clear();
    
    /// Ens retorna la pantalla amb l'ID demanat. Si la pantalla no existeix, ens retornarà una Screen buida
    Screen getScreen(int screenID) const;

    /// Ens retorna l'id de la pantalla principal. En cas que no n'hi hagi cap de definida, ens retornarà -1
    int getPrimaryScreenID() const;
    
    /// Ens indica quina és la pantalla que es troba a la dreta/esquerra de la pantalla amb ID indicada.
    /// Retorna -1 si no n'hi ha cap pantalla en el costat indicat
    int getScreenOnTheRightOf(int screenID) const;
    int getScreenOnTheLeftOf(int screenID) const;

    /// Ens indica quina és la pantalla que es considera anterior/posterior en el layout respecte a la pantalla passada per paràmetre
    /// Retorna el mateix ID si no tenim més pantalles que la nostra i -1 si la pantalla passada no existeix.
    int getPreviousScreenOf(int screenID) const;
    int getNextScreenOf(int screenID) const;

private:
    /// Ens retorna l'índex de la pantalla amb ID screenID. Si no existeix, l'índex serà -1
    int getIndexOfScreen(int screenID) const;

private:
    /// Constant per determinar el llindar de proximitat entre finestres
    static const int SamePositionThreshold;
    
    /// Llista de pantalles
    QList<Screen> m_screens;
};

} // End namespace udg

#endif
