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

#ifndef QUERYPACS
#define QUERYPACS

/// This class helps to interactive with the pacs, allow us to find studies in the pacs setting a search mask. Very important for this class a connection and a mask search must be setted befoer query Studies

struct T_ASC_Association;
class DcmDataset;
class T_DIMSE_C_FindRQ;
class T_DIMSE_C_FindRSP;

namespace udg{

class PacsConnection;
class Status;
class DicomMask;

class QueryPacs
{

public:

    /** Estableix la connexió a utilitzar per comunicar-se amb el PACS
      */
    void setConnection( PacsConnection connection );

    /** màscara dicom a cercar
     * @param mask màscara
     * @return estat del mètode
     */
    Status query( DicomMask mask);

private:

    T_ASC_Association *m_assoc; // request DICOM association;
    DcmDataset *m_mask;

    //fa el query al pacs
    Status query();

    /**Aquest és un mètode que és cridat en callback per les dcmtk, per cada objecte dicom que es trobi en el PACS que compleix la query dcmtk el crida. Aquest mètode ens insereix la llista d'estudis, sèries o imatges l'objecte dicom trobat en funció del nivell del que sigui l'objecte.
     */
    static void foundMatchCallback(
        void * /*callbackData*/ ,
        T_DIMSE_C_FindRQ * /*request*/ ,
        int responseCount,
        T_DIMSE_C_FindRSP *rsp,
        DcmDataset *responseIdentifiers
        );

};
};
#endif
