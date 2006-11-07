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

#include <qvaluelist.h>
#include <qstringlist.h>
#include <qmap.h>

KSXMLHandler::KSXMLHandler(const QString &XML)
  : QObject()
{
  if(XML.isNull())
  {
    m_xmlDocument=QDomDocument("default");
    m_root=m_xmlDocument.createElement("phrase");
    m_xmlDocument.appendChild(m_root);
  }
  else
  {
    m_xmlDocument.setContent(XML);
    m_root=m_xmlDocument.documentElement();
  }
}

QString KSXMLHandler::readString(const QString &search)
{
  for(QDomNode count=m_xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==search)
    {
      return count.toElement().text();
    }
  }
  return QString::null;
}

QValueList<KSExplanation> KSXMLHandler::readExplanation(bool transitional)
{
  QValueList<KSExplanation> output;
  for(QDomNode count=m_xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
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
        if(transitional)
        {
          if(listing.nodeName()=="explanation2")
          {
            temp.explanation=listing.toElement().text();
            continue;
          }
          if(listing.nodeName()=="example2")
          {
            temp.example=listing.toElement().text();
            continue;
          }
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
  for(QDomNode count=m_xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==search)
    {
      output << count.toElement().text();
    }
  }
  return output;
}

QMap<QString, QString> KSXMLHandler::readQMapList(const QString &search)
{
  QMap<QString, QString> output;
  for(QDomNode count=m_xmlDocument.firstChild().firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==search)
    {
      output[count.toElement().attribute("id")] = count.toElement().text();
    }
  }
  return output;
}

void KSXMLHandler::addString(const QString &name, const QString &value, const QString &attributeName, const QString &attributeValue)
{
  QDomElement element=m_xmlDocument.createElement(name);
  element.appendChild(m_xmlDocument.createTextNode(value));
  if(!attributeName.isNull())
  {
    element.setAttribute(attributeName, attributeValue);
  }
  m_root.appendChild(element);
}

void KSXMLHandler::createNode(const QString &name)
{
  m_element=m_xmlDocument.createElement(name);
  m_root.appendChild(m_element);
}

void KSXMLHandler::addChildString(const QString &name, const QString &value)
{
  QDomElement element=m_xmlDocument.createElement(name);
  element.appendChild(m_xmlDocument.createTextNode(value));
  m_element.appendChild(element);
}

QString KSXMLHandler::parse()
{
  QString output=m_xmlDocument.toString().replace("\"", "'");
  output.replace("<?xml version = '1.0' encoding = 'UTF-8'?>", "<?xml version='1.0' encoding='UTF-8'?>");
  if(output.find("<?xml version='1.0' encoding='UTF-8'?>")==-1)
  {
    output.prepend("<?xml version='1.0' encoding='UTF-8'?>");
  }
  return output;
}

void KSXMLHandler::appendString(const QString &name, const QString &value, const QString &attributeName, const QString &attributeValue)
{
  if(!search(name, value))
  {
    QDomElement element=m_xmlDocument.createElement(name);
    element.appendChild(m_xmlDocument.createTextNode(value));
    if(!attributeName.isNull())
    {
      element.setAttribute(attributeName, attributeValue);
    }
    m_root.appendChild(element);
  }
}

void KSXMLHandler::removeString(const QString &name, const QString &value)
{
  for(QDomNode count=m_root.firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==name)
    {
      if(count.toElement().text()==value)
      {
        m_root.removeChild(count);
        return;
      }
    }
  }
}

bool KSXMLHandler::search(const QString &name, const QString &value)
{
  for(QDomNode count=m_root.firstChild();!count.isNull();count=count.nextSibling())
  {
    if(count.nodeName()==name)
    {
      if(count.toElement().text()==value)
      {
        return true;
      }
    }
  }
  return false;
}

KSXMLHandler::~KSXMLHandler()
{
}


#include "ksxmlhandler.moc"
