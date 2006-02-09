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

#include "ksdbhandler.h"

#include "../sqlite/sqlite3.h"
#include "../misc/ksdata.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kstaticdeleter.h>

KSDBHandler *KSDBHandler::m_instance=0L;
QString KSDBHandler::m_currentPath=0L;
static KStaticDeleter<KSDBHandler> staticKSDBHandlerDeleter;

KSDBHandler::KSDBHandler(const QString &databasePath)
  : QObject()
{
  m_currentPath=databasePath;
  sqlite3_open(databasePath.utf8(), &m_db);
  query("PRAGMA auto_vacuum = 1;");
}

bool KSDBHandler::query(const QString &sqlQuery, sqlite3_stmt ** output)
{
  int statusCode;

  if( sqlQuery.isEmpty() )
  {
    kdError() << "[KSDBHandler]->Query Query is not assigned!";
    return false;
  }


  const char *tail;

  statusCode=sqlite3_prepare(m_db, sqlQuery.utf8(), sqlQuery.length(), output, &tail);


  if(statusCode!=SQLITE_OK)
  {
    kdDebug() << "[KSDBHandler]->Query SQLITE err code: " << statusCode << endl
        << "Error Description: " << QString::fromAscii(sqlite3_errmsg(m_db)) << endl;
    return false;
  }
  return true;
}

bool KSDBHandler::query(const QString &sqlQuery)
{
  int statusCode;

  if( sqlQuery.isEmpty() )
  {
    kdError() << "[KSDBHandler]->Query Query is not assigned!";
    return false;
  }

  statusCode=sqlite3_exec(m_db, sqlQuery.utf8(), NULL, NULL, NULL);

  if(statusCode!=SQLITE_OK)
  {
    kdDebug() << "[KSDBHandler]->Query SQLITE err code: " << statusCode << endl
        << "Error Description: " << QString::fromAscii(sqlite3_errmsg(m_db)) << endl;
    return false;
  }
  return true;
}

bool KSDBHandler::saveDictionary(const QString &text, const QString &lang, bool create)
{
  QString rawQuery;
  QString type;
  type.setNum(0);
  if(create)
  {
    rawQuery="BEGIN TRANSACTION; CREATE TABLE head ( lang INTEGER , type INTEGER , version TEXT ); CREATE TABLE media ( id INTEGER PRIMARY KEY AUTOINCREMENT , mime TEXT , data BLOB ); CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , modified INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT UNIQUE , search TEXT );";
    rawQuery=rawQuery+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' );"+"INSERT INTO head ( lang , type ) VALUES ( '"+lang+"' , '"+type+"' ); CREATE INDEX dictionary_index ON dictionary (id); CREATE INDEX phrases_index ON phrases (id, name); COMMIT;";
  }
  else
  {
    rawQuery="UPDATE dictionary SET text='"+text+"' WHERE id='0'; UPDATE head SET lang='"+lang+"';";
  }

  if(!query(rawQuery))
  {
    return false;
  }
  return true;
}

KSDBHandler *KSDBHandler::instance(const QString &path)
{
  if((!KSDBHandler::m_instance) || (KSDBHandler::m_currentPath!=path))
  {
    staticKSDBHandlerDeleter.setObject(KSDBHandler::m_instance, new KSDBHandler(path));
  }

  return KSDBHandler::m_instance;
}

bool KSDBHandler::saveWord(const QString &word, const QString &text, bool add, const QString &id)
{
  bool output;
  QString rawQuery, search;
  search=convertString(word);
  if(add)
  {
    rawQuery="INSERT INTO phrases ( name , search ) VALUES ( '"+word+"' , '"+search+"' );";
    if(output=query(rawQuery))
    {
      rawQuery="INSERT INTO dictionary ( text ) VALUES ( \""+text+"\" );";
      output=query(rawQuery);
    }
  }
  else
  {
    rawQuery="UPDATE phrases SET name='"+word+"', search='"+search+"' WHERE id='"+id+"';";
    if(output=query(rawQuery))
    {
      rawQuery="UPDATE dictionary SET text=\""+text+"\" WHERE id='"+id+"';";
      output=query(rawQuery);
    }
  }
  return output;
}

QString KSDBHandler::processString(const QString &rawQuery, int columns)
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
    kdError() << "[KSDBHandler]->ProcessOutput SQLITE err code: " << statusCode << endl;
    return 0L;
  }

  output = QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, 0 ) );
  for(int count=1; count<columns; count++)
  {
    output+="/"+QString::fromUtf8( (const char*) sqlite3_column_text( rawOutput, count ) );
  }

  statusCode=sqlite3_reset(rawOutput);

  if(statusCode!=SQLITE_OK)
  {
    kdError() << "[KSDBHandler]->ProcessOutput->Reset SQLITE err code: " << statusCode << endl;
    return 0L;
  }

  return output;
}

QStringList KSDBHandler::processList(const QString &rawQuery, int columns)
{
  QString temp;
  QStringList output;
  sqlite3_stmt *rawOutput;
  int statusCode;

  if(!query(rawQuery, &rawOutput))
  {
    return output;
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

bool KSDBHandler::processQuery(const QString &rawQuery)
{
  return query(rawQuery);
}

int KSDBHandler::getId()
{
  return sqlite3_last_insert_rowid(m_db);
}

int KSDBHandler::getId(const QString& name)
{
  return processString("SELECT id FROM phrases WHERE name='"+name+"';").toInt();
}

QString KSDBHandler::convertString(const QString &input)
{
  QMap<QChar, QChar> table=KSData::instance()->getConvertTable();
  QString converting;
  for(ushort count=0;count<input.length();count++)
  {
    if(table.contains(input[count].lower()))
    {
      converting.append(table[input[count].lower()]);
    }
    else
    {
      converting.append(input[count]);
    }
    if(input[count].upper()==input[count])
    {
      converting.replace(converting[count], converting[count].upper());
    }
  }
  return converting;
}

KSDBHandler::~KSDBHandler()
{
  query("REINDEX phrases; REINDEX dictionary;");
  sqlite3_close(m_db);
  if(m_instance==this)
  {
    staticKSDBHandlerDeleter.setObject(m_instance, 0, false);
  }
}
