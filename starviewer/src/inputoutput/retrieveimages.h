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
#ifndef RETRIEVEIMAGES
#define RETRIEVEIMAGES

#define HAVE_CONFIG_H 1
#include "osconfig.h" /* make sure OS specific configuration is included first */

#include "diutil.h"
#include "dcfilefo.h"
#include "dcuid.h"
#include "dcdict.h"
#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"    /* for dcmtk version name */
#include "dcxfer.h"
#include "studymask.h"

#include "pacsconnection.h"
#include "imagelistsingleton.h"
#include "status.h"

/** This class helps to interactive with the pacs, retrieve images that match with the mask
 */
 class string;
namespace udg{
class RetrieveImages{

public:
 
   RetrieveImages();
   void setConnection(PacsConnection connection);
   void setNetwork (T_ASC_Network * network);
   void setMask(StudyMask);
   Status moveSCU();
       
private:
    
    T_ASC_Association *m_assoc; // request DICOM association;
    T_ASC_Network *m_net;
    DcmDataset *m_mask;

};
};
#endif
