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

#ifndef UDGIDENTIFIER_H
#define UDGIDENTIFIER_H

#include <iostream>

namespace udg {

/**
    Encapsula un identificador numèric.
  */
class Identifier {
public:
    /// Constructor
    Identifier();
    Identifier(int id);

    /// Constructor de còpia
    Identifier(const Identifier &id);

    ~Identifier();

    /// Li assigna un nou valor a l'identificador
    void setValue(int newId);
    /// Ens indica si l'dentificador té un valor vàlid o no
    bool isNull();

    int getValue() const
    {
        return m_id;
    }

    bool operator != (const Identifier &identifier);
    bool operator==(const Identifier &id);

    bool operator <=(const Identifier &id);
    bool operator >(const Identifier &id);
    bool operator >=(const Identifier &id);
    bool operator <(const Identifier &id) const;
    Identifier& operator =(const Identifier &id);
    Identifier& operator=(const int id);
    Identifier operator+(const Identifier &id);
    Identifier operator+(const int id);
    Identifier operator+=(const Identifier &id);
    Identifier operator+=(const int id);
    Identifier operator-(const Identifier &id);
    Identifier operator-(const int id);
    Identifier operator-=(const Identifier &id);
    Identifier operator-=(const int id);

    friend std::ostream& operator << (std::ostream &out, const Identifier &id);
    friend bool operator==(const Identifier &id1, const Identifier &id2);
    friend unsigned int qHash(const Identifier &id);

private:
    int m_id;
};

};

#endif
