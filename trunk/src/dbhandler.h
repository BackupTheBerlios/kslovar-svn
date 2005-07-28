/***************************************************************************
 *   Copyright (C) 2005 by Gregor Kališnik   *
 *   gregor@podnapisi.net   *
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

class QString;
typedef struct sqlite3;

/**
* class to read dictionaries from SQLite databases
* @short Database handling class
* @author Gregor Kališnik <gregor@podnapisi.net>
*/

class DBHandler
{
  public:
    
    /**
    * Class's constructor that opens a connection to the database
    * @param databasePath Path to the databases
    */
    DBHandler(QString databasePath);
    /**
    * Method to read from databases
    * @param sqlQuery Query to execute. One at the time!
    * @return Returns the text of the phrase or false
    * @todo Improve error handling
    */
    QString query(QString sqlQuery);
    
    /**
    * Desctructor that closes the connection
    */
    ~DBHandler();
    
  private:
    sqlite3 *db;
};
