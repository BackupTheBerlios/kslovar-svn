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
#include <qstringlist.h>
#include <kprogress.h>
#include <klocale.h>

#include <kdebug.h>


DBHandler::DBHandler(QString databasePath)
{
  
  sqlite3_open(databasePath, &db);
  sqlite3_busy_timeout(db, 0);
}

QString DBHandler::readText(QString id)
{
  sqlite3_stmt *stmt;
  
  QString query1="SELECT text FROM dictionary WHERE id='";
  query1=query1.append(id);
  query1=query1.append("' LIMIT 1;");
  
  query(query1, &stmt);
  
  sqlite3_step(stmt);
  QString output=QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 0 ) );
  sqlite3_reset(stmt);
  return output;
}

void DBHandler::query(QString sqlQuery, sqlite3_stmt ** output, bool returnResult)
{
  if( sqlQuery.isEmpty() )
  {
    kdError() << "[DBHandler] Query is not assigned!";
  }
  
  if(returnResult)
  {
    const char *tail;
  
    sqlite3_prepare(db, sqlQuery, sqlQuery.length(), output, &tail);
  }
  else
  {
    sqlite3_exec(db, sqlQuery, NULL, NULL, NULL);
  }
}

QStringList DBHandler::readIndex(int * count)
{
  int error;
  QString temp;
  QStringList output;
  
  sqlite3_stmt *stmt;
  
  query("SELECT id, name, search FROM phrases ORDER BY search ASC;", &stmt);
  
  while(true)
  {
    error = sqlite3_step(stmt);
    if ( error == SQLITE_DONE || error == SQLITE_ERROR )
    {
      break;
    }
    temp = QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 1 ) );
    temp = temp + "/" + QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 0 ) ) + "/" + QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 2 ) );
    output << temp;
    *count++;
  }
  sqlite3_reset(stmt);
  return output;
}

void DBHandler::saveDictionary(QString text, bool create)
{
  QString query1;
  if(create)
  {
  query1="BEGIN TRANSACTION; CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , audio VARCHAR( 36 ));";
  query1=query1+"INSERT INTO dictionary VALUES(0, '"+text+"', NULL);CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name VARCHAR( 36 ) , search VARCHAR( 36 )); COMMIT;";
  }
  else
  {
    query1="UPDATE dictionary SET text='"+text+"' WHERE id='0';";
  }
  
  query(query1, NULL, false);
}

DBHandler::~DBHandler()
{
  sqlite3_close(db);
}
