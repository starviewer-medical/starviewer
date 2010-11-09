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

// Llibreries QT
#include <QTreeWidgetItem>

namespace udg {

const QString NotAvailableValue(QObject::tr("N/A"));

QDICOMDumpBrowser::QDICOMDumpBrowser(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);

    createConnections();
}

QDICOMDumpBrowser::~QDICOMDumpBrowser()
{

}

void QDICOMDumpBrowser::createConnections()
{
    // Connectem els butons
    connect(m_searchTagLineEdit, SIGNAL(textChanged(const QString &)), SLOT(searchTag(const QString &)));
}


void QDICOMDumpBrowser::searchTag(const QString &textTosearch)
{	
    /*
	QFont fontNode;
	QList<QTreeWidgetItem *> llistaNodes = m_tagsListQTree->findItems("", Qt::MatchContains);
	QList<QTreeWidgetItem *> llistaNodesIgualsColumna1  = m_tagsListQTree->findItems(_textTosearch, Qt::MatchContains,0);
	QList<QTreeWidgetItem *> llistaNodesIgualsColumna2  = m_tagsListQTree->findItems(_textTosearch, Qt::MatchContains,1);
	QList<QTreeWidgetItem *> llistaNodesIgualsColumna3  = m_tagsListQTree->findItems(_textTosearch, Qt::MatchContains,2);

    foreach (QTreeWidgetItem *node, llistaNodes)
    {
		fontNode = node -> font(0);
        if (llistaNodesIgualsColumna1.contains(node) || llistaNodesIgualsColumna2.contains(node) || llistaNodesIgualsColumna3.contains(node))
		{	
			fontNode.setBold(true);
		}
		else
		{	
			fontNode.setBold(false);
		}
		
		node->setFont(0,fontNode);
		node->setFont(1,fontNode);
		node->setFont(2,fontNode);
	}
    */
}

void QDICOMDumpBrowser::setCurrentDisplayedImage(Image *currentImage)
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile(currentImage->getPath());

    if (ok)
    {	
        bool resizeColumnsToContents = m_tagsListQTree->topLevelItemCount() == 0;

        m_tagsListQTree->clear();

        QTreeWidgetItem *rootTreeItem = m_tagsListQTree->invisibleRootItem();

        QList<DICOMAttribute*> dicomAttributesList = dicomReader.getDICOMAttributes();
        
        foreach (DICOMAttribute *dicomAttribute, dicomAttributesList)
        {
            if (dicomAttribute->isValueAttribute())
            {
                DICOMValueAttribute *value = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
                this->addLeaf(rootTreeItem, value);
            }
            else
            {
                DICOMSequenceAttribute *sequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
                this->addBranch(rootTreeItem, sequence);
            }
        }

        if (resizeColumnsToContents)
        {
            m_tagsListQTree->resizeColumnToContents(0);
            m_tagsListQTree->resizeColumnToContents(1);
        }
    }
}

void QDICOMDumpBrowser::addLeaf(QTreeWidgetItem *trunkTreeItem, DICOMValueAttribute *value)
{
    QTreeWidgetItem *leafTreeItem = new QTreeWidgetItem();
    leafTreeItem->setText(0, value->getTag()->getName());
    leafTreeItem->setText(1, value->getTag()->getKeyAsQString());
    if (*(value->getTag()) != DICOMPixelData && *(value->getTag()) != DICOMOverlayData)
    {
        leafTreeItem->setText(2, value->getValueAsQString());
    }
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

};
