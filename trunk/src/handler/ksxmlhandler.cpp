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
#include "ksxmlhandler.h"

#include <qdom.h>
#include <qvaluelist.h>
#include <qstringlist.h>

QDomDocument xmlDocument;

KSXMLHandler::KSXMLHandler(const QString &XML)
  : QObject()
{
  if(XML.isNull())
  {
  }
  else
  {
    xmlDocument.setContent(XML);
  }
}

QString KSXMLHandler::readString(const QString &search)
{
  for(QDomNode count=xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==search)
    {
      return count.toElement().text();
    }
  }
  return QString::null;
}

QValueList<KSExplanation> KSXMLHandler::readExplanation()
{
  QValueList<KSExplanation> output;
  for(QDomNode count=xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()=="explanations")
    {
      KSExplanation temp;
      for(QDomNode listing=count.firstChild();!listing.isNull();listing=listing.nextSibling())
      {
        if(listing.nodeName()=="explanation")
        {
          temp.explanation=listing.toElement().text();
          continue;
        }
        if(listing.nodeName()=="example")
        {
          temp.example=listing.toElement().text();
          continue;
        }
      }
      output << temp;
    }
  }
  return output;
}

QStringList KSXMLHandler::readStringList(const QString &search)
{
  QStringList output;
  for(QDomNode count=xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==search)
    {
      output << count.toElement().text();
    }
  }
  return output;
}

KSXMLHandler::~KSXMLHandler()
{
}


#include "ksxmlhandler.moc"
