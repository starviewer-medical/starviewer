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

#include <ofcond.h>
#include "pacsserver.h"

/// Classe que per una màscara d'una sèrie compte les imatges que conté aquesta sèrie. Aquesta classe no és reentrant
// Aquesta classe no es pot fer amb més d'un thread a la vegada degut a la variable global imageNumberGlobal 

namespace udg{

class PacsConnection;
class ImageMask;
class Status;

class QueryImageNumber
{

public:
   
    /** Constructor Class
     * @param Connection to use to query the image's number
     * @param Mask to search 
     */
   QueryImageNumber( PacsConnection , ImageMask ); 
   
    /** Sets the connection to us, to query the image number
     * @param Pacs connection
     */
   void setConnection( PacsConnection );

    /** This action sets the mask that we will use to count image in to the pacs. 
     * @param  Image's mask
     */
   void setMask( ImageMask * );

    /** This action count the number of images, that complies the criterium of the Image Mask
     * @return The status of the action
     */
   Status count();
   
    /** Return the number of images.
     * @return the number of images
     */
   int getImageNumber();
       
private:

    T_ASC_Association *m_assoc; // request DICOM association;
    DcmDataset *m_mask;

    int m_imageNumber;

};
};
#endif
