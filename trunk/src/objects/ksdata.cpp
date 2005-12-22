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

#include <qstringlist.h>
#include <qvaluelist.h>

KSData *KSData::m_instance=0l;

KSData::KSData()
  : QObject()
{
}

KSData *KSData::instance()
{
  if(!KSData::m_instance)
  {
    KSData::m_instance=new KSData();
  }
  return KSData::m_instance;
}

void KSData::setDictionaryPath(QString path)
{
  m_dictionaryPath=path;
}

QString KSData::getDictionaryPath()
{
  return m_dictionaryPath;
}

void KSData::addLanguage(QString name, int id)
{
  KSElement temp;
  temp.name=name;
  temp.id=id;
  m_languages << temp;
}

QStringList KSData::getLanguagesNames()
{
  QStringList temp;
  for(QValueList<KSElement>::iterator count=m_languages.begin();count!=m_languages.end();count++)
  {
    temp << (*count).name;
  }
  return temp;
}

QString KSData::getLanguageId(QString name)
{
  for(QValueList<KSElement>::iterator count=m_languages.begin();count!=m_languages.end();count++)
  {
    if(name==(*count).name)
    {
      return QString::number((*count).id);
    }
  }
  return QString::null;
}

void KSData::addPartOfSpeech(QString name, int id)
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

int KSData::getPartOfSpeechId(QString name)
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

void KSData::clearPartOfSpeech()
{
  m_partOfSpeech.clear();
}

KSData::~KSData()
{
}

#include "ksdata.moc"
