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

#ifndef UDGDICOMDUMPDEFAULTTAGSREADER_H
#define UDGDICOMDUMPDEFAULTTAGSREADER_H

#include <QXmlSchemaValidator>

class QXmlStreamReader;

namespace udg {

class DICOMTag;
class DICOMDumpDefaultTags;
class DICOMDumpDefaultTagsRestriction;

/**
    Classe encarregada de llegir els fitxers XML que contenen la llista de Default Tags
    que s'han de mostrar en el DICOMDump.
    També és l'encarregada de crear els diferent objectes DICOMDumpDefaultTags.
  */
class DICOMDumpDefaultTagsReader {

public:
    DICOMDumpDefaultTagsReader();

    ~DICOMDumpDefaultTagsReader();

    /// Mètode que llegeix un fitxer XML i retorna un objecte DICOMDumpDefaultTags.
    DICOMDumpDefaultTags* readFile(const QString &path);

private:
    /// Mètode que llegeix un DICOMTag i el retorna
    DICOMTag readTag(QXmlStreamReader *reader);

    /// Mètode que llegeix un DICOMDumpDefaultTagsRestriction i el retorna
    DICOMDumpDefaultTagsRestriction readRestriction(QXmlStreamReader *reader);

private:
    /// Validador que s'utilitza a l'hora de llegir els fitxers
    QXmlSchemaValidator m_defaultTagsSchemaValidator;
};

}

#endif
