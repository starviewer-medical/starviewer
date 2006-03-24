#include "queryscreen.h"
#include <qmessagebox.h>
#include <QString>

/** Aquesta cpp forma part de la queryscreen, conte el tractament d'errors de la classe queryscreen*/

namespace udg
{


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
                        break;
            case 2011 : text.insert(0,tr("Database is corrupted."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;
            case 2019 : text.insert(0,tr("Register duplicated."));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;
            case 2050 : text.insert(0,"Not Connected to database");
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        break;            
            default :   text.insert(0,tr("Internal Database error"));
                        text.append("\n");
                        text.append(tr("Error Number : "));
                        code.setNum(state->code(),10);
                        text.append(code);
                        QMessageBox::warning( this, tr("StarViewer"),text);
        }
        QMessageBox::critical( this, tr("StarViewer"),text);
    }    

}

};
