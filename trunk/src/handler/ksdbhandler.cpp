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

#include "sqlite/sqlite3.h"

#include "misc/ksdata.h"

#include "configuration/configuration.h"

#include "handler/ksoutputhandler.h"

#include "kslovar.h"

#include <qapplication.h>
#include <qevent.h>
#include <qmutex.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>

KSDBHandler::KSDBHandler(const QString &databasePath)
  : QThread(), m_stop(false), m_reciever(KSlovar::KSInstance()), m_packets(-1)
{
  sqlite3_open(databasePath.utf8(), &m_db);
  query("PRAGMA auto_vacuum = 1;");
}

bool KSDBHandler::query(const QString &sqlQuery)
{
  m_result.clear();

  if(sqlQuery.isEmpty())
  {
    KMessageBox::error(0, i18n("No query defined in KSDBHandler."), i18n("Critical bug"));
    return false;
  }
  //sqlite3_interrupt(m_db);
  int statusCode;

  statusCode=sqlite3_exec(m_db, sqlQuery.utf8(), KSDBHandler::processQuery, this, NULL);

  if(statusCode == SQLITE_OK)
  {
    return true;
  }

  switch(statusCode)
  {
    case(SQLITE_CONSTRAINT):
    {
      KMessageBox::error(0, i18n("Phrase allready exists."));
      break;
    }
    default:
    {
      KMessageBox::detailedError(0, i18n("Unknown SQL error."), QString::fromUtf8(sqlite3_errmsg(m_db)) + " " + QString::number(statusCode));
    }
  }
  return false;
}

int KSDBHandler::processQuery(void *primaryArgument, int numberColumns, char **columnValues, char **columnNames)
{
  KSResult temp;
  KSDBHandler *handler = static_cast<KSDBHandler*> (primaryArgument);

  for(int count = 0; count < numberColumns; count++)
  {
    temp[QString::fromUtf8(columnNames[count])] = QString::fromUtf8(columnValues[count]);
  }
  handler->m_result << temp;

  if(handler->m_packets != -1)
  {
    if(handler->m_packets >= 1000)
    {
      KSOutputHandler *package = new KSOutputHandler(handler->m_packets);
      QApplication::postEvent(handler->m_reciever, package);
      handler->m_packets = 0;
    }
    handler->m_packets++;
  }

  return 0;
}

bool KSDBHandler::saveDictionary(const QString &text, const QString &lang, const QString &type, bool create)
{
  QString rawQuery;
  if(create)
  {
    rawQuery="BEGIN TRANSACTION; CREATE TABLE head ( lang INTEGER , type INTEGER , author TEXT , email TEXT , number_phrases INTEGER ); CREATE TABLE media ( id INTEGER PRIMARY KEY AUTOINCREMENT , mime TEXT , data BLOB ); CREATE TABLE dictionary ( id INTEGER PRIMARY KEY AUTOINCREMENT , text TEXT , modified INTEGER ); CREATE TABLE phrases ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT UNIQUE , search TEXT ); CREATE TABLE authors ( id INTEGER PRIMARY KEY AUTOINCREMENT , id_phrase INTEGER , name TEXT , email TEXT , first INTEGER );";
    rawQuery=rawQuery+"INSERT INTO dictionary ( id , text ) VALUES ( '0' , '"+text+"' );"+"INSERT INTO head ( lang , type, author , email , number_phrases ) VALUES ( '"+lang+"' , '"+type+"' , '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' , '0' ); CREATE INDEX phrases_index ON phrases (search ASC); COMMIT;";
  }
  else
  {
    rawQuery="UPDATE dictionary SET text='"+text+"' WHERE id='0'; UPDATE head SET lang='"+lang+"', type='"+type+"';";
  }

  return query(rawQuery);
}

bool KSDBHandler::saveWord(const QString &word, const QString &text, bool add, const QString &id)
{
  QString rawQuery, search;
  search=convertString(word);
  if(add)
  {
    rawQuery="BEGIN TRANSACTION; INSERT INTO phrases ( name , search ) VALUES ( '"+word+"' , '"+search+"' ); INSERT INTO dictionary ( text ) VALUES ( \""+text+"\" ); INSERT INTO authors ( id_phrase , name , email , first ) VALUES ( '"+QString::number(getId())+"' , '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' , '1' ); UPDATE head SET number_phrases=number_phrases+1 COMMIT;";
  }
  else
  {
    rawQuery="BEGIN TRANSACTION; UPDATE phrases SET name='"+word+"', search='"+search+"' WHERE id='"+id+"'; UPDATE dictionary SET text=\""+text+"\" WHERE id='"+id+"'; INSERT INTO authors ( id_phrase , name , email , first ) VALUES ( '"+id+"' , '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' , '0' ); COMMIT;";
  }
  return query(rawQuery);
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

KSResult KSDBHandler::processString(const QString &rawQuery)
{

  query(rawQuery);

  return m_result.first();
}

QValueList<KSResult> KSDBHandler::processList(const QString &rawQuery)
{

  query(rawQuery);

  return m_result;
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
  KSResult temp = processString("SELECT id FROM phrases WHERE name='"+name+"';");
  QString output = temp["id"];
  if(output.isNull())
  {
    return -1;
  }
  else
  {
    return output.toInt();
  }
}

void KSDBHandler::run()
{
  QString sqlQuery = "SELECT name, id, search FROM phrases ORDER BY name DESC;";
  m_packets = 0;

  msleep(100);
  query(sqlQuery);

  KSOutputHandler *package = new KSOutputHandler(m_result);
  QApplication::postEvent(m_reciever, package);

  m_packets = -1;
}

/*int KSDBHandler::processList(void *primaryArgument, int numberColumns, char **columnValues, char **columnNames)
{
  KSDBHandler *handler = static_cast<KSDBHandler*> (primaryArgument);
  if(handler->m_stop)
  {
    return 1;
  }
  QString id = QString::fromUtf8((const char*) columnValues[1]);
  QString name = QString::fromUtf8((const char*) columnValues[0]);
  QString search = QString::fromUtf8((const char*) columnValues[2]);

  return 0;
}*/

void KSDBHandler::stopThread()
{
  m_locker.lock();
  m_stop = true;
  m_locker.unlock();
}

KSDBHandler::~KSDBHandler()
{
  sqlite3_close(m_db);
}
