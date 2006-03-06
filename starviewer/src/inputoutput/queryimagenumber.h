/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QUERYIMAGENUMBER
#define QUERYIMAGENUMBER
#define HAVE_CONFIG_H 1


#include <assoc.h>
// #include <dimse.h>
// #include <dul.h>
// #include <diutil.h>
// #include <dcdict.h>
// #include <dcuid.h>
#include <ofcond.h>
// #include <cond.h>
#include "pacsserver.h"
#include "status.h"
#include "imagemask.h"
class PacsConnection;

/** Classe que per una màscara d'una sèrie compte les imatges que conté aquesta sèrie
 */
// Aquesta classe no es pot fer amb més d'un thread a la vegada degut a la variable global imageNumberGlobal 

namespace udg{
class QueryImageNumber{


public:
   QueryImageNumber(PacsConnection,ImageMask); 
   
   void setConnection( PacsConnection);

   void setMask(ImageMask *);

   Status count();
   
   int getImageNumber();
       
private:

    T_ASC_Association *m_assoc; // request DICOM association;
    DcmDataset *m_mask;

    int m_imageNumber;

};
};
#endif
