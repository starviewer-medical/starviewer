/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpbrowser.h"

#include "dicomtagreader.h"
#include "dicomvalueattribute.h"
#include "dicomsequenceitem.h"
#include "dicomsequenceattribute.h"
#include "image.h"
#include "dicomdumpdefaulttagsloader.h"
#include "dicomdumpdefaulttagsrepository.h"
#include "dicomdumpdefaulttagsrestriction.h"

// Llibreries QT
#include <QTreeWidgetItem>
#include <QListIterator>

namespace udg {

const QString NotAvailableValue(QObject::tr("N/A"));

QDICOMDumpBrowser::QDICOMDumpBrowser(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    createConnections();

    m_lastImagePathDICOMDumpDisplayed = "";
}

QDICOMDumpBrowser::~QDICOMDumpBrowser()
{

}

void QDICOMDumpBrowser::createConnections()
{
    // Connectem els butons
    connect(m_searchTagLineEdit, SIGNAL(textChanged(const QString &)), SLOT(updateSearch()));
    connect(m_highlightOnlyCheckBox, SIGNAL(clicked(bool)), SLOT(updateSearch()));
}

void QDICOMDumpBrowser::searchTag(const QString &textToSearch, bool highlightOnly)
{
    clearSearch();

    if (textToSearch.isEmpty())
    {
        return;
    }

    m_tagsListQTree->expandAll();

    QListIterator<QString> wordsIterator(textToSearch.split(QRegExp("\\s+"), QString::SkipEmptyParts));


    QTreeWidgetItemIterator iterator(m_tagsListQTree);
    while (*iterator)
    {
        bool found = false;
        wordsIterator.toFront();
        while (wordsIterator.hasNext() && !found)
        {
            QString word = wordsIterator.next();
            found = (*iterator)->text(0).contains(word, Qt::CaseInsensitive) || (*iterator)->text(1).contains(word, Qt::CaseInsensitive);
        }

        if (highlightOnly)
        {
            if (found)
            {
                (*iterator)->setSelected(true);
            }
        }
        else
        {
            if (found)
            {
                (*iterator)->setHidden(false);
                QTreeWidgetItem *parent = (*iterator)->parent();
                while (parent && parent->isHidden())
                {
                    parent->setHidden(false);
                    parent = parent->parent();
                }
            }
            else
            {
                (*iterator)->setHidden(true);
            }
        }

        ++iterator;
    }

    if (highlightOnly)
    {
        if (m_tagsListQTree->selectedItems().size() > 0)
        {
            m_tagsListQTree->scrollToItem(m_tagsListQTree->selectedItems().at(0));
        }
    }
}


void QDICOMDumpBrowser::setCurrentDisplayedImage(Image *currentImage)
{
    if (currentImage->getPath() != m_lastImagePathDICOMDumpDisplayed)
    {
        // \TODO Xapusa per sortir del pas, s'ha de fer ben fet el lazy loading
        if (DICOMDumpDefaultTagsRepository::getRepository()->getNumberOfItems() == 0)
        {
            DICOMDumpDefaultTagsLoader dicomDumpDefaultTagsLoader;
            dicomDumpDefaultTagsLoader.loadDefaults();
        }

        DICOMTagReader dicomReader;
        bool ok = dicomReader.setFile(currentImage->getPath());

        if (ok)
        {	
            bool resizeColumnsToContents = m_tagsListQTree->topLevelItemCount() == 0;

            initializeQTrees(dicomReader);

            if (resizeColumnsToContents)
            {
                m_tagsListQTree->resizeColumnToContents(0);
                m_tagsListQTree->resizeColumnToContents(1);
                m_defaultTagsQTree->resizeColumnToContents(0);
                m_defaultTagsQTree->resizeColumnToContents(1);
            }

            updateSearch();
        }
        
        m_lastImagePathDICOMDumpDisplayed  = currentImage->getPath();
    }
}


void QDICOMDumpBrowser::addLeaf(QTreeWidgetItem *trunkTreeItem, DICOMValueAttribute *value)
{
    QTreeWidgetItem *leafTreeItem = new QTreeWidgetItem();
    leafTreeItem->setText(0, value->getTag()->getName());
    leafTreeItem->setText(1, value->getTag()->getKeyAsQString());
    leafTreeItem->setText(2, value->getValueAsQString());
    
    trunkTreeItem->addChild(leafTreeItem);
}

void QDICOMDumpBrowser::addBranch(QTreeWidgetItem *trunkTreeItem, DICOMSequenceAttribute *sequence)
{
    QTreeWidgetItem *trunkBranchItem = new QTreeWidgetItem();
    trunkBranchItem->setText(0, sequence->getTag()->getName());
    trunkBranchItem->setText(1, sequence->getTag()->getKeyAsQString());
    trunkBranchItem->setText(2, "");

    foreach (DICOMSequenceItem *sequenceItem, sequence->getItems())
    {
        QTreeWidgetItem *qTreeSequenceItem = new QTreeWidgetItem();
        qTreeSequenceItem->setText(0, "Item");
        qTreeSequenceItem->setText(1, "");
        qTreeSequenceItem->setText(2, "");

        foreach (DICOMAttribute *dicomAttribute, sequenceItem->getAttributes())
        {
            if (dicomAttribute->isValueAttribute())
            {
                DICOMValueAttribute *subValue = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
                this->addLeaf(qTreeSequenceItem, subValue);
            }
            else
            {
                DICOMSequenceAttribute *subSequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
                this->addBranch(qTreeSequenceItem, subSequence);
            }
        }
        trunkBranchItem->addChild(qTreeSequenceItem);
    }
    trunkTreeItem->addChild(trunkBranchItem);
}

void QDICOMDumpBrowser::updateSearch()
{
    searchTag(m_searchTagLineEdit->text(), m_highlightOnlyCheckBox->isChecked());
}

void QDICOMDumpBrowser::clearSearch()
{
    m_tagsListQTree->clearSelection();

    QTreeWidgetItemIterator iterator(m_tagsListQTree);
    while (*iterator)
    {
        (*iterator)->setHidden(false);
        ++iterator;
    }
}

void QDICOMDumpBrowser::initializeQTrees(const DICOMTagReader &dicomReader)
{
    m_tagsListQTree->clear();
    m_defaultTagsQTree->clear();

    QList<DICOMAttribute*> dicomAttributesList = dicomReader.getDICOMHeader() + dicomReader.getDICOMDataSet(DICOMTagReader::ExcludeHeavyTags);
    foreach (DICOMAttribute *dicomAttribute, dicomAttributesList)
    {
        if (dicomAttribute->isValueAttribute())
        {
            DICOMValueAttribute *value = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
            this->addLeaf(m_tagsListQTree->invisibleRootItem(), value);
            this->addLeaf(m_defaultTagsQTree->invisibleRootItem(), value);
        }
        else
        {
            DICOMSequenceAttribute *sequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
            this->addBranch(m_tagsListQTree->invisibleRootItem(), sequence);
            this->addBranch(m_defaultTagsQTree->invisibleRootItem(), sequence);
        }
    }

    qDeleteAll(dicomAttributesList.begin(), dicomAttributesList.end());
    dicomAttributesList.clear();

    hideNonDefaultTags(dicomReader);
}

void QDICOMDumpBrowser::hideNonDefaultTags(const DICOMTagReader &dicomReader)
{
    DICOMDumpDefaultTags *defaultTags = getDICOMDumpDefaultTags(dicomReader);

    if (defaultTags != NULL)
    {
        QList<DICOMTag> defaultTagsToShow = defaultTags->getTagsToShow();
        if (!defaultTagsToShow.isEmpty())
        {
            m_defaultTagsQTree->expandAll();
            QTreeWidgetItemIterator iterator(m_defaultTagsQTree);
            while (*iterator)
            {
                (*iterator)->setHidden(true);
                ++iterator;
            }
            foreach (const DICOMTag &defaultTag, defaultTagsToShow)
            {
                foreach (QTreeWidgetItem *item, m_defaultTagsQTree->findItems(defaultTag.getKeyAsQString(), Qt::MatchExactly | Qt::MatchRecursive, 1))
                {
                    item->setHidden(false);
                    item->setText(0, defaultTag.getName());
                    QTreeWidgetItem *parent = item->parent();
                    while (parent && parent->isHidden())
                    {
                        parent->setHidden(false);
                        parent = parent->parent();
                    }
                }
            }
        }
        m_dumpTabWidget->setTabEnabled(0, true);
    }
    else
    {
       m_dumpTabWidget->setTabEnabled(0, false);
    }
}

DICOMDumpDefaultTags* QDICOMDumpBrowser::getDICOMDumpDefaultTags(const DICOMTagReader &dicomReader)
{
    QString uidReference = dicomReader.getValueAttributeAsQString(DICOMSOPClassUID);

    DICOMDumpDefaultTags* candidate = NULL;
    QList<DICOMDumpDefaultTags*> possiblesCandidatesList = DICOMDumpDefaultTagsRepository::getRepository()->getItems();

    foreach(DICOMDumpDefaultTags* possibleCandidate, possiblesCandidatesList)
    {
        if(uidReference == possibleCandidate->getSOPClassUID())
        {
            QListIterator<DICOMDumpDefaultTagsRestriction> iterator(possibleCandidate->getRestrictions());
            bool isPossibleCandidate = true;

            while (iterator.hasNext() && isPossibleCandidate)
            {
                DICOMDumpDefaultTagsRestriction restriction = iterator.next();
                QString value = dicomReader.getValueAttributeAsQString(restriction.getDICOMTag());
                isPossibleCandidate = restriction.isValidValue(value);
            }

            if (isPossibleCandidate)
            {
                if(candidate == NULL || candidate->getNumberOfRestrictions() < possibleCandidate->getNumberOfRestrictions())
                {
                    candidate = possibleCandidate;
                }
            }
        }
    }

    return candidate;
}

};
