#include "dcfilefo.h"
#include <assoc.h>
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


 /*this file contains some structs necessaries in the program*/


/*This struct is used in pacsmove.cpp, it can't be included in the pacsmove class declaration because
  it's used from a callback action of function*/
struct StoreCallbackData
    {
        char* imageFileName;
        DcmFileFormat* dcmff;
        T_ASC_Association* assoc;
    };



/*This struct is used in pacsmove.cpp, it can't be included in the pacsmove class declaration because
  it's used from a callback action of function*/
typedef struct {
    T_ASC_Association *assoc;
    T_ASC_PresentationContextID presId;
    } MyCallbackInfo;


