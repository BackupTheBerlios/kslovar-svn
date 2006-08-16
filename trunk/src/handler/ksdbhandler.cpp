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

#ifdef SQLITE3_BUILD
#include "sqlite/sqlite3.h"
#else
#include <sqlite3.h>
#endif

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

  statusCode = sqlite3_exec(m_db, sqlQuery.utf8(), KSDBHandler::processQuery, this, NULL);

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
    rawQuery = "BEGIN TRANSACTION; CREATE TABLE head ( lang VARCHAR(20) , type INTEGER , author VARCHAR(20) , email VARCHAR(20) , revision INTEGER DEFAULT 0, server VARCHAR(20) , version VARCHAR(20) , greeting TEXT ); CREATE TABLE phrases ( id_phrase INTEGER NOT NULL PRIMARY KEY , name VARCHAR(20) UNIQUE , search VARCHAR(20) , text TEXT , modified INTEGER ); CREATE TABLE media ( id_media INTEGER NOT NULL PRIMARY KEY , mime VARCHAR(20) , data Blob ); CREATE TABLE type ( id_type INTEGER NOT NULL PRIMARY KEY , name VARCHAR(20) UNIQUE ); CREATE TABLE authors ( id_author INTEGER NOT NULL PRIMARY KEY , name VARCHAR(20) UNIQUE ON CONFLICT IGNORE , email VARCHAR(20) ); CREATE TABLE modified ( id_phrase INTEGER NOT NULL , id_author INTEGER NOT NULL , last_modified INTEGER , PRIMARY KEY (id_phrase,id_author)); COMMIT;";
    rawQuery = rawQuery + "INSERT INTO head ( lang , type, author , email , greeting , version ) VALUES ( '"+lang+"' , '"+type+"' , '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' , '"+text+"' , '0.1-rc3' ); CREATE INDEX phrases_index ON phrases (name DESC); BEGIN TRANSACTION;";

    //We make sure that the right parts of speech are loaded
    KSlovar::KSInstance()->loadPartOfSpeech(KSData::instance()->getLanguageId(lang));

    //We store the parts of speech into the dictionary.
    QValueList<KSElement> partOfSpeech = KSData::instance()->getPartOfSpeechList();
    for(QValueList<KSElement>::const_iterator count = partOfSpeech.begin(); count != partOfSpeech.end(); count++)
    {
      rawQuery += "INSERT INTO type (id_type, name) VALUES ('"+QString::number((*count).id)+"', '"+(*count).name+"');";
    }
    rawQuery = rawQuery + "COMMIT;";
  }
  else
  {
    rawQuery = "UPDATE head SET greeting='"+text+"' , lang='"+lang+"', type='"+type+"';";
  }

  return query(rawQuery);
}

bool KSDBHandler::saveWord(const QString &word, const QString &text, bool add, const QString &id)
{
  QString rawQuery, search;
  search = convertString(word);
  if(add)
  {
    rawQuery="INSERT INTO phrases ( name , search , text ) VALUES ( '"+word+"' , '"+search+"' , \""+text+"\" ); INSERT INTO authors ( name , email ) VALUES ( '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' );";
  }
  else
  {
    rawQuery="UPDATE phrases SET name='"+word+"', search='"+search+"' , text=\""+text+"\" WHERE id_phrase='"+id+"'; INSERT INTO authors ( id_author , name , email ) VALUES ( '"+id+"' , '"+Configuration::authorName()+"' , '"+Configuration::authorEmail()+"' );";
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
  KSResult temp = processString("SELECT id_phrase FROM phrases WHERE name='"+name+"';");
  QString output = temp["id_phrase"];
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
  QString sqlQuery = "SELECT name, id_phrase, search FROM phrases ORDER BY name DESC;";
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
  //We set the "stopping" variable to true.
  m_stop = true;
  m_locker.unlock();
}

void KSDBHandler::syncTypes()
{
  //A bool for knowing when to display the warning.
  bool remove = false;

  //We get the list of parts from KSData.
  KSlovar::KSInstance()->loadPartOfSpeech(KSData::instance()->getLanguage());
  QValueList<KSElement> partsOfSpeech = KSData::instance()->getPartOfSpeechList();

  //We also get the parts of speech list from the dictionary.
  QString rawQuery = "SELECT id_type, name FROM type;";
  query(rawQuery);

  //We store the result into a local variable. You don't know when will the thread start messing with it!
  QValueList<KSResult> result = m_result;

  //We figure out if there is a part of speech removed from the languages.ldft. If so, we warn the user.
  bool find = false;
  rawQuery = "BEGIN TRANSACTION;";
  for(QValueList<KSResult>::const_iterator count = result.begin(); count != result.end(); count++)
  {
    for(QValueList<KSElement>::iterator count2 = partsOfSpeech.begin(); count2 != partsOfSpeech.end(); count2++)
    {
      //The editing part of the query.
      if(((*count2).id == (*count)["id_type"]))
      {
        //If the name is not the same, but ID is, then the name of the part of speech has been changed. We commit this change to the dictionary.
        if((*count2).name == (*count)["name"])
        {
          rawQuery += "UPDATE type SET name='"+(*count2).name+"' WHERE id_type='"+(*count)["id_type"]+"';";
        }
        //Remove the type from the list, so we end up with the list of types that needs to be inserted.
        partsOfSpeech.remove(count2);
        find = true;
        break;
      }
    }

    //Add the removing part of the query.
    if(!find)
    {
      rawQuery += "DELETE FROM type WHERE id_type='"+(*count)["id_type"]+"';";
      remove = true;
    }
    else
    {
      find = false;
    }
  }

  //The inserting part of the query.
  for(QValueList<KSElement>::const_iterator count = partsOfSpeech.begin(); count != partsOfSpeech.end(); count++)
  {
    rawQuery += "INSERT INTO type (id_type, name) VALUES ('"+QString::number((*count).id)+"', '"+(*count).name+"');";
  }

  //The actual warning.
  if(remove)
  {
    if(KMessageBox::warningContinueCancel(KSlovar::KSInstance(), i18n("If you proceed with this update, some of the words will loose their part of speech. Are you sure you want to do this?"), i18n("Warning possible unexpected loss of data"), i18n("Remove")) == KMessageBox::Cancel)
    {
      return;
    }
  }

  //We finish the query and execute it.
  rawQuery += "COMMIT;";
  query(rawQuery);

  //Reload the parts of speech.
  KSlovar::KSInstance()->loadPartOfSpeech();

  KMessageBox::information(0, i18n("Synchronisation successful."));
}

KSDBHandler::~KSDBHandler()
{
  sqlite3_close(m_db);
}
