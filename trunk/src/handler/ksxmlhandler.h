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
#ifndef KSXMLHANDLER_H
#define KSXMLHANDLER_H

#include <qobject.h>
#include <qdom.h>


typedef struct
{
  QString explanation;
  QString example;
}KSExplanation;

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSXMLHandler : public QObject
{
  Q_OBJECT
  public:
    KSXMLHandler(const QString &XML=QString::null);

    QString readString(const QString &search);
    QValueList<KSExplanation> readExplanation();
    QStringList readStringList(const QString &search);

    void createNode(const QString &name);
    void addString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null);
    void addChildString(const QString &name, const QString &value);
    void appendString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null);
    void removeString(const QString &name, const QString &value);
    bool search(const QString &name, const QString &value);
    QString parse();

    ~KSXMLHandler();

  private:
    QDomDocument m_xmlDocument;
    QDomElement m_root;
    QDomElement m_element;

};

#endif
