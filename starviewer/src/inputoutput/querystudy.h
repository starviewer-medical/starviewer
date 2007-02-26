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

#ifndef QUERYSTUDY
#define QUERYSUTY
#define HAVE_CONFIG_H 1

#include <assoc.h>
#include <dimse.h>

class PacsConnection;
#include <ofcond.h>
#include "studylistsingleton.h"
#include "pacsserver.h"
#include "studymask.h"

/// This class helps to interactive with the pacs, allow us to find studies in the pacs setting a search mask. Very important for this class a connection and a mask search must be setted befoer query Studies
namespace udg{

class Study;
class Status;

class QueryStudy
{

public:

    /** This action sets the connection that we will use to connect to the pacs
     * @param connection Openned connection to the pacs
     * @param study Mask
     */
   QueryStudy( PacsConnection , StudyMask );

    /** Sets the connection to the PACS
     * @param PACS's connection
     */
   void setConnection( PacsConnection connection );

    /** Query studies to the pacs
     * @param status
     */
   Status find();

    /** This action sets the mask that we will use to search the studies in to the pacs. This mask is created by mask class
     * @param search Mask
     */
   void setMask( StudyMask );

    /** get the list study with the results of the query
     * @return A pointer to the ListStudy with the results of the query
     */
   StudyListSingleton* getStudyList();

private:

    T_ASC_Association *m_assoc; // request DICOM association;

    StudyListSingleton* m_studyListSingleton;
    DcmDataset *m_mask;

};
};
#endif
