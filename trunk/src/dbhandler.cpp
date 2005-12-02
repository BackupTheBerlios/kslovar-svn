/***************************************************************************
 *   Copyright (C) 2005 by Gregor Kališnik                                 *
 *   gregor@podnapisi.net                                                  *
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

/*QString DBHandler::readText(QString id)
{
  sqlite3_stmt *stmt;
  //QString database="user_dictionary";
  /*if(!user)
{
  database="dictionary";
}*//*

  QString query1="SELECT text FROM dictionary WHERE id='";
  query1=query1.append(id);
  query1=query1.append("' LIMIT 1;");

  Query(query1, &stmt);

  sqlite3_step(stmt);
  QString output=QString::fromUtf8( (const char*) sqlite3_column_text( stmt, 0 ) );
  sqlite3_reset(stmt);
  return output;
}*/

bool DBHandler::query(QString sqlQuery, sqlite3_stmt ** output)
{
  int statusCode;

  if( sqlQuery.isEmpty() )
  {
    kdError() << "[DBHandler]->Query Query is not assigned!";
    return false;
  }


  const char *tail;

  statusCode=sqlite3_prepare(m_db, sqlQuery.utf8(), sqlQuery.length(), output, &tail);


  if(statusCode!=SQLITE_OK)
  {
    kdDebug() << "[DBHandler]->Query SQLITE err code: " << statusCode << endl;
    return false;
  }
  return true;
}

bool DBHandler::query(QString sqlQuery)
{
  int statusCode;

  if( sqlQuery.isEmpty() )
  {
    kdError() << "[DBHandler]->Query Query is not assigned!";
    return false;
  }

  statusCode=sqlite3_exec(m_db, sqlQuery.utf8(), NULL, NULL, NULL);

  if(statusCode!=SQLITE_OK)
  {
    kdDebug() << "[DBHandler]->Query SQLITE err code: " << statusCode << endl;
    return false;
  }
  return true;
}

/*QStringList DBHandler::readIndex(int * count)
{
  int error;
  QString temp;
  QStringList output;

  sqlite3_stmt *stmt;

  Query("SELECT name, search, id FROM phrases ORDER BY search ASC;", &stmt);

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
}*/

bool DBHandler::saveDictionary(QString text, bool create)
{
  QString rawQuery;
  if(create)
  {
    /*query1="BEGIN TRANSACTION; CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT ); CREATE TABLE user_dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , rand INTEGER , new INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name VARCHAR( 36 ) , search VARCHAR( 36 ) , ido INTEGER UNIQUE , idu INTEGER UNIQUE ); ";
    query1=query1+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' ); COMMIT;";*/
    rawQuery="BEGIN TRANSACTION; CREATE TABLE media ( id INTEGER PRIMARY KEY AUTOINCREMENT , mime TEXT , data BLOB ); CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , modified INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT , search TEXT ); ";
    rawQuery=rawQuery+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' ); COMMIT;";
    //query1=query1.utf8();
  }
  else
  {
    rawQuery="UPDATE dictionary SET text='"+text+"' WHERE id='0';";
  }

  if(!query(rawQuery))
  {
    return false;
  }
  return true;
}

DBHandler *DBHandler::instance(QString path)
{
  if((!m_instance) || (m_currentPath!=path))
  {
    m_instance=new DBHandler(path);
  }

  return m_instance;
}

bool DBHandler::saveWord(QString word, QString text, bool add, QString id)
{
  QString rawQuery;
  if(add)
  {
    rawQuery="INSERT INTO phrases ( name , search ) VALUES ( '"+word+"' , '"+word+"' ); INSERT INTO dictionary ( text ) VALUES ( \""+text+"\" );";
  }
  else
  {
    rawQuery="UPDATE dictionary SET text=\""+text+"\" WHERE id='"+id+"';";
  }
  if(!query(rawQuery))
  {
    return false;
  }
  return true;
}

/*bool DBHandler::Query(QString query)
{
  if(query.isEmpty())
{
    kdError() << "[DBHandler] Query not defined!" << endl;
    return false;
}

  int statusCode;

  const char *tail;
  statusCode=sqlite3_prepare(m_db, query.utf8(), query.length(), &m_rawOutput, &tail);

  if(statusCode!=SQLITE_OK)
{
    kdError() << "[DBHandler]->Query SQLITE err code: " << statusCode << endl;
    return false;
}
  return true;
}*/

QString DBHandler::processString(QString rawQuery, int columns)
{
  sqlite3_stmt *rawOutput;
  QString output;
  int statusCode;

  if(!query(rawQuery, &rawOutput))
  {
    return 0L;
  }

  statusCode=sqlite3_step(rawOutput);

  if(statusCode!=SQLITE_OK && statusCode < SQLITE_ROW)
  {
    kdError() << "[DBHandler]->ProcessOutput SQLITE err code: " << statusCode << endl;
    return 0L;
  }

  //QString output=QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, 0 ) );
  output = QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, 0 ) );
  for(int count=1; count<columns; count++)
  {
    output+="/"+QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, count ) );
  }

  statusCode=sqlite3_reset(rawOutput);

  if(statusCode!=SQLITE_OK)
  {
    kdError() << "[DBHandler]->ProcessOutput->Reset SQLITE err code: " << statusCode << endl;
    return 0L;
  }

  return output;
}

QStringList DBHandler::processList(QString rawQuery, int columns)
{
  QString temp;
  QStringList output;
  sqlite3_stmt *rawOutput;
  int statusCode;

  if(!query(rawQuery, &rawOutput))
  {
    return 0L;
  }

  while(true)
  {
    statusCode = sqlite3_step(rawOutput);
    if ( statusCode == SQLITE_DONE || statusCode == SQLITE_ERROR )
    {
      break;
    }
    temp = QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, 0 ) );
    for(int count=1; count<columns; count++)
    {
      temp+="/"+QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, count ) );
    }
    output << temp;
  }
  sqlite3_reset(rawOutput);
  return output;
}

bool DBHandler::processQuery(QString rawQuery)
{
  return query(rawQuery);
}

DBHandler::~DBHandler()
{
  sqlite3_close(m_db);
  m_instance=0L;
}
