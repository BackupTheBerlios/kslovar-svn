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
#include "ksdata.h"

#include "configuration/configuration.h"

#include <kstaticdeleter.h>

KSData *KSData::m_instance = 0;
static KStaticDeleter<KSData> staticKSDataDeleter;

KSData::KSData()
  : QObject(), m_dictionaryHandler(0), m_languageHandler(0), m_mainList(0), m_selectedStyle(Configuration::dictionaryStyle()), m_literalSearch(false), m_backSearch(false)
{
}

KSData *KSData::instance()
{
  if(!KSData::m_instance)
  {
    staticKSDataDeleter.setObject(m_instance, new KSData());
  }
  return KSData::m_instance;
}

void KSData::setDictionaryPath(const QString &path)
{
  m_dictionaryPath = path;
}

QString KSData::getDictionaryPath()
{
  return m_dictionaryPath;
}

void KSData::setDictionary(KSDBHandler *dictionaryHandler)
{
  m_dictionaryHandler = dictionaryHandler;
}

KSDBHandler *KSData::getDictionary()
{
  return m_dictionaryHandler;
}

void KSData::addLanguage(const QString &name, int id)
{
  bool obstaja = false;
  KSElement temp;
  temp.name = name;
  temp.id = id;
  for(QValueList<KSElement>::iterator count = m_languages.begin(); count != m_languages.end(); count++)
  {
    if((*count).name == temp.name)
    {
      obstaja = true;
      break;
    }
  }
  if(!obstaja)
  {
    m_languages << temp;
  }
}

QStringList KSData::getLanguagesNames()
{
  QStringList temp;
  for(QValueList<KSElement>::iterator count = m_languages.begin(); count != m_languages.end(); count++)
  {
    temp << (*count).name;
  }
  return temp;
}

int KSData::getLanguageId(const QString &name)
{
  for(QValueList<KSElement>::iterator count = m_languages.begin(); count != m_languages.end(); count++)
  {
    if(name == (*count).name)
    {
      return (*count).id;
    }
  }
  return -1;
}

QString KSData::getLanguageName(int id)
{
  for(QValueList<KSElement>::iterator count = m_languages.begin(); count != m_languages.end(); count++)
  {
    if(id == (*count).id)
    {
      return (*count).name;
    }
  }
  return QString::null;
}

void KSData::addPartOfSpeech(const QString &name, int id)
{
  KSElement temp;
  temp.name=name;
  temp.id=id;
  m_partOfSpeech << temp;
}

QStringList KSData::getPartOfSpeech()
{
  QStringList temp;
  for(QValueList<KSElement>::iterator count=m_partOfSpeech.begin();count!=m_partOfSpeech.end();count++)
  {
    temp << (*count).name;
  }
  return temp;
}

int KSData::getPartOfSpeechId(const QString &name)
{
  for(QValueList<KSElement>::iterator count=m_partOfSpeech.begin();count!=m_partOfSpeech.end();count++)
  {
    if((*count).name==name)
    {
      return (*count).id;
    }
  }
  return -1;
}

QString KSData::getPartOfSpeechName(int id)
{
  for(QValueList<KSElement>::iterator count=m_partOfSpeech.begin();count!=m_partOfSpeech.end();count++)
  {
    if((*count).id==id)
    {
      return (*count).name;
    }
  }
  return QString::null;
}

void KSData::setLanguage(int id)
{
  m_languageId=id;
}

int KSData::getLanguage()
{
  return m_languageId;
}

void KSData::setLanguageHandler(KSDBHandler *languageHandler)
{
  m_languageHandler = languageHandler;
}

KSDBHandler *KSData::getLanguageHandler()
{
  return m_languageHandler;
}

void KSData::clearPartOfSpeech()
{
  m_partOfSpeech.clear();
}

void KSData::setMainList(KSListView *list)
{
  m_mainList = list;
}

KSListView *KSData::getMainList()
{
  return m_mainList;
}

void KSData::setStyle(const QString &selectedStyle)
{
  m_selectedStyle = selectedStyle;
}

QString KSData::getStyle()
{
  return m_selectedStyle;
}

void KSData::addConversion(const QChar &from, const QChar &to)
{
  m_convertTable[from] = to;
}

QMap<QChar, QChar> KSData::getConvertTable()
{
  return m_convertTable;
}

void KSData::setLiteralSearch(bool searchType)
{
  m_literalSearch = searchType;
}

bool KSData::literalSearch()
{
  return m_literalSearch;
}

void KSData::setBackSearch(bool searchType)
{
  m_backSearch = searchType;
}

bool KSData::backSearch()
{
  return m_backSearch;
}

void KSData::setType(int id)
{
  m_typeId = id;
}

int KSData::getType()
{
  return m_typeId;
}

KSData::~KSData()
{
  if(m_instance == this)
  {
    staticKSDataDeleter.setObject(m_instance, 0, false);
  }
}

#include "ksdata.moc"
