/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "parsexmlrispierrequest.h"

#include "dicommask.h"
#include "logging.h"

namespace udg {

bool ParseXmlRisPIERRequest::error()
{
    return m_errorParsing;
}

DicomMask ParseXmlRisPIERRequest::parseXml(QString xmlRisPIERRequest)
{
    DicomMask mask;

    buildBasicDicomMask(&mask);//Construïm la dicomMask indicant quins tags ens ha de retornar el PACS quan li fem la consulta

    QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlRisPIERRequest);

    setErrorParsing(true);//Inicialment indiquem error al parsejar, si trobem els valors esperats indicarem que no hi ha error

    INFO_LOG("Xml a parsejar: " + xmlRisPIERRequest);

    xmlReader->readNext();

    //Comprovem que sigui un document xml, esperem tag tipus  <?xml version="1.0" encoding="UTF-8"?>
    if (!xmlReader->isStartDocument())
    {
        ERROR_LOG("Sembla que no es tracta d'un document Xml");
        return mask;
    }

    xmlReader->readNext();//Anem a llegir el primer element del XML

    while (!xmlReader->atEnd())
    {
        //Només processarem tags d'obertura, els altres elements els ignorem
        if (xmlReader->isStartElement())
        {
            //Hem trobat el començament d'un tag, nosaltres només parsegem el tag <msg>
            if (xmlReader->name().toString().compare("Msg", Qt::CaseInsensitive) == 0)
            {
                parseTagMsg(xmlReader, &mask);
            }
            else 
            {
                ERROR_LOG("S'esperava el tag Msg i s'ha trobat el tag " + xmlReader->name().toString());
                xmlReader->readNext();
            }
        }
        else xmlReader->readNext();
    }

    if (xmlReader->error()) ERROR_LOG("S'ha produït un error parsejant el Xml: " + xmlReader->errorString());

    return mask;
}

void ParseXmlRisPIERRequest::parseTagMsg(QXmlStreamReader *xmlReader, DicomMask *mask)
{
/*  El Tag Msg té aquest format, té  l'atribut name que indica l'acció, i llavors conté a dintre el tag <Param>, que són  
    paràmetres que ens ajuden a cercar l'estudi pel qual hem de fer l'acció 

    <Msg Name="OpenStudies">
        <Param Name="AccessionNumber">00239RS00006780</Param>
    </Msg>
*/
    //Només contemplem un valor per  l'element name "OpenStudies" si en trobem un de diferent avisem
    if (xmlReader->attributes().value("Name").toString().compare("OpenStudies", Qt::CaseInsensitive) != 0)
    {
        ERROR_LOG("Es demana una acció no contemplada " + xmlReader->attributes().value("Name").toString());
    }

    xmlReader->readNext();

    while (!xmlReader->atEnd()) //Llegim els tags Param que conté el xml
    {
        //Només processem els tags d'obetura els altres elements els ignorem
        if (xmlReader->isStartElement())
        {
            if (xmlReader->name().toString().compare("Param", Qt::CaseInsensitive) == 0)
            {
                parseTagParam(xmlReader, mask); //parsegem el tag Param
            }
            else 
            {
                ERROR_LOG("S'esperava el tag Param i s'ha trobat el tag " + xmlReader->name().toString());
                xmlReader->readNext();
            }
        }
        else xmlReader->readNext();
    }
}

void ParseXmlRisPIERRequest::parseTagParam(QXmlStreamReader *xmlReader, DicomMask *mask)
{
/*  El Tag Param té el següent format, conté un atribut anomenat name que indica quin paràmetre de l'estudi és el valor que ens 
    passen

    <Param Name="AccessionNumber">00239RS00006780</Param>
*/

    /*Només processem el tipus de paràmetre que ens informa del valor de l'accession number, no cal processar-ne cap més
      perquè el RIS PIER només envia aquest*/
    if (xmlReader->attributes().value("Name").toString().compare("AccessionNumber", Qt::CaseInsensitive) == 0) 
    {
        xmlReader->readNext();
        if (xmlReader->isCharacters())
        {
            INFO_LOG("L'accession number que conté el xml es: " + xmlReader->text().toString());
            mask->setAccessionNumber(xmlReader->text().toString());
            setErrorParsing(false); //Indiquem que no s'ha produït cap error parsejant perquè hem trobat el paràmetre que esperàvem
        }
    }
    else 
    {
        ERROR_LOG("El xml conté un paràmetre que no tractem" + xmlReader->attributes().value("Name").toString());
        xmlReader->readNext();
    }
}

void ParseXmlRisPIERRequest::buildBasicDicomMask(DicomMask *mask)
{
    mask->setStudyUID("");
    mask->setStudyId("");
    mask->setPatientName("");
    mask->setPatientId("");
}

void ParseXmlRisPIERRequest::setErrorParsing(bool errorParsing)
{
    m_errorParsing = errorParsing;
}

}

