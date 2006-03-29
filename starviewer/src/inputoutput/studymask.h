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

#ifndef STUDYMASK
#define STUDYMASK
#define HAVE_CONFIG_H 1

#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#ifndef CONST
#include "const.h"    
#endif
#include "status.h"

#include <string>
/** This class construct a mask to search the patients in the pacs. Cada camp que volem que la cerca ens retorni li hem de fer el set, sinó retornarà valor 
null per aquell camp. Per tots els camps podem passar, null o cadena buida que significa que buscarà tots els valors d'aquell camp, o passar-li un valor en concret, on només retornarà els compleixin aquell criteri.
 */
namespace udg{

class StudyMask{

public:

    StudyMask();

    Status setPatientName(std::string);
    Status setPatientBirth(std::string date);
    Status setPatientBirth(std::string dateMin,std::string dateMax);
    Status setPatientSex(std::string);  
    Status setPatientId(std::string);
    Status setPatientAge(std::string);  
    
    Status setStudyId(std::string);
    Status setStudyDate(std::string date);
    Status setStudyDate(std::string dateMin,std::string dateMax);
    Status setStudyDescription(std::string);   
    Status setStudyModality(std::string);
    Status setStudyTime(std::string);
    Status setStudyTime(std::string timeMin,std::string timeMax);
    Status setStudyUID(std::string);
    
    Status setInstitutionName(std::string);
    Status setAccessionNumber(std::string);
        
    std::string getStudyUID();
    
    DcmDataset* getMask();
    
private:

   DcmDataset *m_mask;
   
   void retrieveLevel();
};
};
#endif
