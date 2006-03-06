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

#ifndef SERIESMASK
#define SERIESMASK
#define HAVE_CONFIG_H 1

//necessitem tipus d'aquests classes que són structs,per tant no se'ns permet fer "forward declaration"
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#include "status.h"

#ifndef CONST
#include "const.h"     
#endif


/** This class construct a mask to search series in the pacs 
 */
namespace udg{
class SeriesMask{

public:

    SeriesMask();

    Status setSeriesNumber(const char *);
    Status setSeriesDate(const char* date);
    Status setSeriesDate(const char* dateMin,const char* dateMax);
    Status setSeriesTime(const char *);
    Status setSeriesTime(const char* timeMin,const char* timeMax);
    Status setSeriesDescription(const char*);  
    Status setSeriesModality(const char *);  
    Status setSeriesOperator(const char *);
    Status setSeriesBodyPartExaminated(const char*);
    Status setSeriesProtocolName(const char *);
    
    Status setStudyUID(const char *);
    Status setSeriesUID(const char* date);
    
    DcmDataset* getSeriesMask();
   
private:

   DcmDataset *m_seriesMask;
   
   void retrieveLevel();

};
};
#endif
