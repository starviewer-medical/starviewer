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
#ifndef QUERYSERIES
#define QUERYSERIES
#define HAVE_CONFIG_H 1

#include <assoc.h>
#include <ofcond.h>
#include "serieslistsingleton.h"
#include "pacsserver.h"
#include "seriesmask.h"
#include "status.h"
class PacsConnection;

/// This class helps to interactive with the pacs, allow us to find series in the pacs, setting a search mask. Very important a connection and a mask search must be setted before query Series!!

namespace udg{
class QuerySeries
{

class Series;

public:

    /** This action sets the connection that we will use to connect to the pacs
     * @param Open connection to the pacs
     */
   QuerySeries( PacsConnection , SeriesMask );

    /** Sets and openn connection to search a series
     * @param pacs Connection
     */
   void setConnection( PacsConnection );

   /// This action finds the series in the pacs with the established mask
   Status find();

    /** This action sets the mask that we will use to search the series in to the pacs. This mask is created by mask class
     * @param Series maks
     */
   void setMask( SeriesMask * );

    /** get the list study with the results of the query
     * @return  A pointer to the ListSeries with the results of the query
     */
   SeriesListSingleton* getSeriesList();

private:

    T_ASC_Association *m_assoc; // request DICOM association;
    SeriesListSingleton* m_seriesListSingleton;
    DcmDataset *m_mask;

};
};
#endif
