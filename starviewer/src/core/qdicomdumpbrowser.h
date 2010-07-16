/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQDICOMDUMPTAGLIST_H
#define UDGQDICOMDUMPTAGLIST_H

#include "ui_qdicomdumpbrowserbase.h"

namespace udg {

class Image;
class DICOMValueAttribute;
class DICOMSequenceAttribute;

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
    void searchTag(const QString &textToSearch);

private:
    /// Crea les connexions dels signals
    void createConnections();

    /// Afegeix un DICOMValueAttribute com una fulla de l'arrel de l'arbre
    void addLeafToRoot(DICOMValueAttribute *value);
    
    /// Afegeix un DICOMValueAttribute com una fulla d'una branca de l'arbre
    void addLeafToBranch(QTreeWidgetItem *branch, DICOMValueAttribute *value);

    /// Afegeix un DICOMSequenceAttribute com una branca de l'arbre
    void addBranchToRoot(DICOMSequenceAttribute *sequence);
    
    /// Afegeix un DICOMSequenceAttribute com una branca d'un altre branca de l'arbre
    void addBranchToBranch(QTreeWidgetItem *branch, DICOMSequenceAttribute *sequence);
};

}

#endif
