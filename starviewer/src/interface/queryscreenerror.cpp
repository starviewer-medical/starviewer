#include "queryscreen.h"
#include <qmessagebox.h>
#include <qstring.h>

/** Aquesta cpp forma part de la queryscreen, conte el tractament d'errors de la classe queryscreen*/

namespace udg
{
/** Tracta els errors que s'han produït durant la accio retrieve al moment d'inserir un estudi
  *           @param state [in] Estat de l'acció retrieve
  */
void QueryScreen::retrieveErrorInsertStudy(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {
            case 2019 : text.insert(0,tr("The study has been retrieved."));
                        text.append("\n");
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}

/** Tracta els errors que s'han produït durant la accio retrieve al moment d'inserir un estudi
  *           @param state [in] Estat de l'acció retrieve
  */
void QueryScreen::retrieveErrorInsertSeries(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {   //Si dona l'error 2019 al inserir una sèrie hi ha incosistència a la base dades, l'estudi al que pertany la sèria
            //no existia a la bd, però si existeix alguna de les sèries. Aquest error no s'hauria de donar mai si les inserccions i deletes es fan 
            // consistentment.
            case 2019 : text.insert(0,tr("The study has been retrieved."));
                        text.append("\n");
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}

/** Tracta els errors que s'han produït durant una consulta a la cache
  *           @param state [in] Estat de l'acció retrieve
  */
void QueryScreen::queryCacheError(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {  case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}


/** Tracta els errors que s'han produït a la base de dades en general
  *           @param state [in] Estat del mètode
  */
void QueryScreen::databaseError(Status *state)
{

    QString text,code;
    if (!state->good())
    {
        switch(state->code())
        {  case 2001 : text.insert(0,tr("Database not found."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
                        break;
        }
    }    

}

};
