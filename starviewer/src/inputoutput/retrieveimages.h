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
#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */

#include "diutil.h"
#include "dcfilefo.h"
#include "ofconapp.h"//necessari per fer les sortides per pantalla de les dcmtk
#include "dicommask.h"

#include "pacsconnection.h"
#include "imagelistsingleton.h"
#include "starviewersettings.h"

/** This class helps to interactive with the pacs, retrieve images that match with the mask
 */
class status;
namespace udg{

class RetrieveImages
{

public:

   ///constructor de la classe
   RetrieveImages();

    /** This action sets the connection that we will use to connect to the pacs
     * @param   connection [in] Study's Open connection to the pacs
     */
   void setConnection(PacsConnection connection);

   /** Sets the network to use for retrieve the images
    * @param Network to use
    */
   void setNetwork (T_ASC_Network * network);

    /** This action sets the mask that we will use to retrieve the image in to the pacs.
    * @param mask Màscara de cerca
    */
   void setMask( DicomMask mask );

   /** Download the study's Image
    * @return state of the method
    */
   Status retrieve();

private:

    T_ASC_Association *m_assoc; // request DICOM association;
    T_ASC_Network *m_net;
    DcmDataset *m_mask;

};
};
#endif
