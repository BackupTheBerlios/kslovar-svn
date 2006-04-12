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

/**
 * @short Structure for handling explanations. (maybe transformed into class?)
 */
typedef struct
{
  QString explanation;
  QString example;
  QString explanation2;
  QString example2;
}KSExplanation;

/**
 * @short Class for parsing XML strings.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSXMLHandler : public QObject
{
  Q_OBJECT
  public:
    /**
     * Constructor for creating KSXMLHandler.
     * @param XML XML string for parsing.
     */
    KSXMLHandler(const QString &XML=QString::null);

    /**
     * Reads a value from an element.
     * @param search Name of the element.
     * @return Value string of the element.
     * @return @c QString::null No result.
     */
    QString readString(const QString &search);
    /**
     * Reads all explanations inside an XML and returnes them in a QValueList<KSExplanation>.
     * @param transitional True if we are using an transitional dictionary. False by default.
     * @return Returns the result. (explanations with examples)
     */
    QValueList<KSExplanation> readExplanation(bool transitional = false);
    /**
     * Like readString() but used for more possible results. (Synonyms for example)
     * @param search Name of the element.
     * @return Returns a QStringList with results.
     */
    QStringList readStringList(const QString &search);
    /**
     * Reads attributes from elements. (Stupid name, I know... :D)
     * @param search Name of the element.
     * @return QMap with attributes. Keys are the id of the attributes.
     */
    QMap<QString, QString> readQMapList(const QString &search);

    /**
     * Creates a new node.
     * @param name Name of the node.
     */
    void createNode(const QString &name);
    /**
     * Adds a value into an element. Optional, you can add an attribute.
     * @param name Name of the element.
     * @param value Value to be added.
     * @param attributeName Name of the attribute. OPTIONAL
     * @param attributeValue Attribute's value. OPTIONAL
     */
    void addString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null);
    /**
     * Same as addString(), but cannot add attribute. DEPRECEATED!
     * @see KSXMLHandler#addString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null)
     * @param name Name of the element.
     * @param value Value of the element.
     */
    void addChildString(const QString &name, const QString &value);
    /**
     * Same as createNode(), but adds only when there is no exsiting elements with the name.
     * @see KSXMLHandler#addString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null)
     * @param name Name of the element.
     * @param value Value of the element.
     * @param attributeName Name of the attribute. OPTIONAL
     * @param attributeValue Value of the attribute. OPTIONAL
     */
    void appendString(const QString &name, const QString &value, const QString &attributeName=QString::null, const QString &attributeValue=QString::null);
    /**
     * Removes a string.
     * @param name Name of the element.
     * @param value Value of the element.
     */
    void removeString(const QString &name, const QString &value);
    /**
     * Searches for an element.
     * @param name Name of the element.
     * @param value Value of the element.
     *
     * @return @c true Element exists.
     * @return @c false Element doesn't exist.
     */
    bool search(const QString &name, const QString &value);
    /**
     * Generate XML string from the inserted data.
     *
     * @return XML formatted QString.
     */
    QString parse();

    /**
     * An empty destructor.
     */
    ~KSXMLHandler();

  private:
    QDomDocument m_xmlDocument;
    QDomElement m_root;
    QDomElement m_element;

};

#endif
