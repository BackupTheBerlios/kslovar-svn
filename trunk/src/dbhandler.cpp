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

#include "dbhandler.h"
#include "sqlite/sqlite3.h"

#include <qstring.h>

DBHandler::DBHandler(QString databasePath)
{
  
  sqlite3_open(databasePath, &db);
}

QString DBHandler::query(QString sqlQuery)
{
  if( sqlQuery.isEmpty() )
  {
    return false;
  }
  
  sqlite3_stmt *stmt;
  const char *tail;
  
  sqlite3_prepare(db, sqlQuery, sqlQuery.length(), &stmt, &tail);
  
  sqlite3_step(stmt);
  
  return QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 0 ) );
}

DBHandler::~DBHandler()
{
  sqlite3_close(db);
}
