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
#include "ksxslhandler.h"

#include "misc/ksdata.h"
#include "configuration/configuration.h"

#include <libxml/globals.h>
#include <libxml/parser.h>

#include <libxslt/xsltconfig.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>

#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qregexp.h>

#include <kdebug.h>
#include <kapplication.h>
#include <kstandarddirs.h>
#include <klocale.h>

xsltStylesheetPtr styleSheet;
xmlDocPtr xslDoc;

KSXSLHandler::KSXSLHandler(const QString &document)
{
  setXSL(document);
}

void KSXSLHandler::setXSL(const QString &document, QString style)
{
  styleSheet=0;
  xslDoc=0;

  QCString rawDocument=openXSL(document, style);
  xslDoc=xmlParseMemory(rawDocument, rawDocument.length());
  if(xslDoc)
  {
    styleSheet=xsltParseStylesheetDoc(xslDoc);
  }
  else
  {
    kdWarning() << "Wrong XSL format!" << endl;
    xmlFreeDoc(xslDoc);
    xslDoc=0;
  }
}

QString KSXSLHandler::parse(QString xmlString)
{
  //check if the message is allready a HTML
  if(xmlString.find("<?xml version='1.0' encoding='UTF-8'?>")==-1)
  {
    return xmlString;
  }

  QStringList temp=KSData::instance()->getPartOfSpeech();
  for(QStringList::Iterator count=temp.begin();count!=temp.end();count++)
  {
    xmlString.replace("<type>"+QString::number(KSData::instance()->getPartOfSpeechId(*count))+"</type>", "<type>"+*count+"</type>");
  }

  QCString xmlCString=xmlString.utf8();
  QString result;

  xmlDocPtr xmlDoc=xmlParseMemory(xmlCString, xmlCString.length());
  if(xmlDoc)
  {
    if(styleSheet)
    {
      static QCString appPath( QString::fromLatin1("\"%1\"").arg( KApplication::kApplication()->dirs()->findDirs("appdata", QString::fromLatin1("styles/data") ).front() ).utf8() );

      static const char* params[3] = {
        "appdata",
        appPath,
        NULL
      };

      xmlDocPtr xmlResult=xsltApplyStylesheet(styleSheet, xmlDoc, params);
      if(xmlResult)
      {
        xmlChar *temp;
        int size;
        xmlDocDumpMemory(xmlResult, &temp, &size);
        result=QString::fromUtf8(QCString((char *)temp, size+1));
        xmlFree(temp);
        xmlFreeDoc(xmlResult);
      }
      else
      {
        kdWarning() << "There is no phrase." << endl;
      }
    }
    else
    {
      kdWarning() << "No stylesheet loaded." << endl;
    }
  }
  else
  {
    kdWarning() << "Wrong XML format." << endl;
  }
  xmlFreeDoc(xmlDoc);
  xmlDoc=0;
  return result;
}

QCString KSXSLHandler::openXSL(QString path, QString style)
{
  QString temp;
  QFile input(path);
  if(input.open(IO_ReadOnly))
  {
    QTextStream stream(&input);
    while(!stream.atEnd())
    {
      temp+=stream.readLine();
    }
    input.close();
  }

  if(style == "null")
  {
    style = Configuration::dictionaryStyle();
  }

  //TODO: an if to divide betwen dictionaries. (or switch) ???
  temp.replace("{explanations-examples}", i18n("Explanations and examples"));
  temp.replace("{explanations}", i18n("Explanations"));
  temp.replace("{examples}", i18n("Examples"));
  temp.replace("{synonym}", i18n("Synonym"));
  temp.replace("{antonym}", i18n("Antonym"));
  temp.replace("{word-family}", i18n("Word family"));
  temp.replace("{styles-dir}", path.remove(QRegExp("/"+style+"-.+")));
  temp.replace("{images-dir}", path+"/images");
  return temp.utf8();
}

KSXSLHandler::~KSXSLHandler()
{
  xsltFreeStylesheet(styleSheet);
  xmlFreeDoc(xslDoc);
}


