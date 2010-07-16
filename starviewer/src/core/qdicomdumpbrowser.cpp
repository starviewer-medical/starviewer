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
        QList<DICOMAttribute*> dicomAttributesList = dicomReader.getDICOMAttributes();
        
        foreach (DICOMAttribute *dicomAttribute, dicomAttributesList)
        {
            if (dicomAttribute->isValueAttribute())
            {       
                DICOMValueAttribute *value = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
                this->addLeafToRoot(value);
            }
            else 
            {
                DICOMSequenceAttribute *sequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
                this->addBranchToRoot(sequence);
            }
        }
    }
}

void QDICOMDumpBrowser::addLeafToRoot(DICOMValueAttribute *value)
{               
    QTreeWidgetItem *qTreeWidgetItem = new QTreeWidgetItem();
    qTreeWidgetItem->setText(0, value->getTag()->getName());
    qTreeWidgetItem->setText(1, value->getTag()->getKeyAsQString());
    qTreeWidgetItem->setText(2, value->getValueAsQString());
    m_tagsListQTree->addTopLevelItem(qTreeWidgetItem);
}

void QDICOMDumpBrowser::addLeafToBranch(QTreeWidgetItem *branch, DICOMValueAttribute *value)
{               
    QTreeWidgetItem *qTreeWidgetItem = new QTreeWidgetItem();
    qTreeWidgetItem->setText(0, value->getTag()->getName());
    qTreeWidgetItem->setText(1, value->getTag()->getKeyAsQString());
    qTreeWidgetItem->setText(2, value->getValueAsQString());
    branch->addChild(qTreeWidgetItem);
}

void QDICOMDumpBrowser::addBranchToRoot(DICOMSequenceAttribute *sequence)
{   
    QTreeWidgetItem *qTreeWidgetItemRoot = new QTreeWidgetItem();
    qTreeWidgetItemRoot->setText(0, sequence->getTag()->getName());
    qTreeWidgetItemRoot->setText(1, sequence->getTag()->getKeyAsQString());
    qTreeWidgetItemRoot->setText(2, "");
    m_tagsListQTree->addTopLevelItem(qTreeWidgetItemRoot);
    
    foreach (DICOMSequenceItem *sequenceItem, sequence->getItems())
    {
        QTreeWidgetItem *qTreeWidgetItem = new QTreeWidgetItem();
        qTreeWidgetItem->setText(0, "Item");
        qTreeWidgetItem->setText(1, "");
        qTreeWidgetItem->setText(2, "");
        qTreeWidgetItemRoot->addChild(qTreeWidgetItem);

        foreach (DICOMAttribute *dicomAttribute, sequenceItem->getAttributes())
        {
            if (dicomAttribute->isValueAttribute())
            {
                DICOMValueAttribute *value = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
                this->addLeafToBranch(qTreeWidgetItem, value);
            }
            else
            {
                DICOMSequenceAttribute *sequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
                this->addBranchToBranch(qTreeWidgetItem, sequence);
            }
        }
    }
}

void QDICOMDumpBrowser::addBranchToBranch(QTreeWidgetItem *branch, DICOMSequenceAttribute *sequence)
{
    QTreeWidgetItem *qTreeWidgetItemRoot = new QTreeWidgetItem();
    qTreeWidgetItemRoot->setText(0, sequence->getTag()->getName());
    qTreeWidgetItemRoot->setText(1, sequence->getTag()->getKeyAsQString());
    qTreeWidgetItemRoot->setText(2, "");
    branch->addChild(qTreeWidgetItemRoot);
    
    foreach (DICOMSequenceItem *sequenceItem, sequence->getItems())
    {
        QTreeWidgetItem *qTreeWidgetItem = new QTreeWidgetItem();
        qTreeWidgetItem->setText(0, "Item");
        qTreeWidgetItem->setText(1, "");
        qTreeWidgetItem->setText(2, "");
        qTreeWidgetItemRoot->addChild(qTreeWidgetItem);

        foreach (DICOMAttribute *dicomAttribute, sequenceItem->getAttributes())
        {
            if (dicomAttribute->isValueAttribute())
            {
                DICOMValueAttribute *value = dynamic_cast<DICOMValueAttribute*>(dicomAttribute);
                this->addLeafToBranch(qTreeWidgetItem, value);
            }
            else
            {
                DICOMSequenceAttribute *sequence = dynamic_cast<DICOMSequenceAttribute*>(dicomAttribute);
                this->addBranchToBranch(qTreeWidgetItem, sequence);
            }
        }
    }
}

};
