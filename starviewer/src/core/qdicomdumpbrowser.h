/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQDICOMDUMPTAGLIST_H
#define UDGQDICOMDUMPTAGLIST_H

#include "ui_qdicomdumpbrowserbase.h"
#include "dicomtag.h"
#include "dicomtagreader.h"

namespace udg {

class Image;
class DICOMValueAttribute;
class DICOMSequenceAttribute;
class DICOMDumpDefaultTags;

/** Aquest classe és l'encarregada de realitzar el dicomdump de la imatge que s'està visualizant per part de l'usuari.
    Totes les imatges tenen una sèrie de tags que es descodifiquen en aquesta classe i es visualitzen en forma d'arbre mitjançant un QTreeWidget
  */
class QDICOMDumpBrowser : public QDialog, private Ui::QDICOMDumpBrowserBase {
Q_OBJECT
public:
    QDICOMDumpBrowser(QWidget *parent = 0);
    ~QDICOMDumpBrowser();

    /// Genera el dicomdump de la imatge que se li passa per paràmetre
    void setCurrentDisplayedImage(Image *currentImage);

public slots:
    /// Cerca els nodes de l'arbre (Tags) que contenen el valor textToSearch
    void searchTag(const QString &textToSearch, bool highlightOnly = false);

private:
    /// Crea les connexions dels signals
    void createConnections();

    /// Mètode que afegeix una fulla al tronc o a una branca de l'arbre
    void addLeaf(QTreeWidgetItem *trunkTreeItem, DICOMValueAttribute *value);

    /// Mètode que afegeix una branca al tronc o a una branca de l'arbre
    void addBranch(QTreeWidgetItem *trunkTreeItem, DICOMSequenceAttribute *sequence);

    /// Neteja la cerca anterior
    void clearSearch();

    /// Inicialitza els arbres que es mostraran a les pestanyes de tags per defecte i tots els tags.
    void initializeQTrees(const DICOMTagReader &dicomReader);

    /// Actualitza l'arbre perquè només siguin visibles aquells tags que estan marcats com a importants.
    void hideNonDefaultTags(const DICOMTagReader &dicomReader);

    /// Mètode que ens retorna una llista del Tags a mostrar per defecte per la imatge amb la qual s'està treballant amb el DICOMDump.
    DICOMDumpDefaultTags* getDICOMDumpDefaultTags(const DICOMTagReader &dicomReader);

private slots:
    /// Fa una nova cerca
    void updateSearch();

private:

    QString m_lastImagePathDICOMDumpDisplayed;
};

}

#endif
