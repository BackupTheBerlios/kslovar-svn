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
#include <kmessagebox.h>

#include <kdebug.h>

DBHandler *DBHandler::m_instance=0L;
QString DBHandler::m_currentPath=0L;

DBHandler::DBHandler(QString databasePath)
{
  sqlite3_open(databasePath, &m_db);
}

QString DBHandler::readText(QString id)
{
  sqlite3_stmt *stmt;
  //QString database="user_dictionary";
  /*if(!user)
  {
    database="dictionary";
}*/
  
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
  int error;
  
  if( sqlQuery.isEmpty() )
  {
    kdError() << "[DBHandler] Query is not assigned!";
  }
  
  if(returnResult)
  {
    const char *tail;
  
    error=sqlite3_prepare(m_db, sqlQuery.utf8(), sqlQuery.length(), output, &tail);
  }
  else
  {
    error=sqlite3_exec(m_db, sqlQuery.utf8(), NULL, NULL, NULL);
  }

  if(error!=SQLITE_OK)
  {
    kdDebug() << "SQLITE err code: " << error << endl;
    KMessageBox::detailedError(0, i18n("Error executing SQL query."), i18n("Query was "+sqlQuery));
  }
}

QStringList DBHandler::readIndex(int * count)
{
  int error;
  QString temp;
  QStringList output;
  
  sqlite3_stmt *stmt;
  
  query("SELECT name, search, id FROM phrases ORDER BY search ASC;", &stmt);
  
  while(true)
  {
    error = sqlite3_step(stmt);
    if ( error == SQLITE_DONE || error == SQLITE_ERROR )
    {
      break;
    }
    temp = QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 0 ) );
    temp = temp + "/" + QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 2 ) ) + "/" + QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 1 ) );
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
    /*query1="BEGIN TRANSACTION; CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT ); CREATE TABLE user_dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , rand INTEGER , new INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name VARCHAR( 36 ) , search VARCHAR( 36 ) , ido INTEGER UNIQUE , idu INTEGER UNIQUE ); ";
    query1=query1+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' ); COMMIT;";*/
    query1="BEGIN TRANSACTION; CREATE TABLE media ( id INTEGER PRIMARY KEY AUTOINCREMENT , mime TEXT , data BLOB ); CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , id_media INTEGER , modified INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT , search TEXT ); ";
    query1=query1+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' ); COMMIT;";
    //query1=query1.utf8();
  }
  else
  {
    query1="UPDATE dictionary SET text='"+text+"' WHERE id='0';";
  }
  
  query(query1, 0L, false);
}

DBHandler *DBHandler::Instance(QString path)
{
  if((!m_instance) || (m_currentPath!=path))
  {
    m_instance=new DBHandler(path);
  }
  
  return m_instance;
}

void DBHandler::saveWord(QString word, QString text, bool add, QString id)
{
  QString query1;
  if(add)
  {
    query1="INSERT INTO phrases ( name , search ) VALUES ( '"+word+"' , '"+word+"' ); INSERT INTO dictionary ( text ) VALUES ( '"+text+"' );";
  }
  else
  {
    query1="UPDATE dictionary SET text='"+text+"' WHERE id='"+id+"';";
  }
  query(query1, 0L, false);
}

DBHandler::~DBHandler()
{
  sqlite3_close(m_db);
  m_instance=0L;
}
