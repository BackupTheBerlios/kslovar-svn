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

#include <qstring.h>

class QCString;

/**
 * @short Class for merging XML and XSLT into HTML.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSXSLHandler{
  public:
    /**
     * Constructor that makes the KSXSLHandler.
     * @param document Path to the XSLT document (style).
     */
    KSXSLHandler(const QString &document);

    /**
     * Changes the XSLT style.
     * @param document Path to the XSLT document (style).
     * @param style Currently used style. Used for correct parsing (replacing {...} strings). DEFAULT: QString::null
     */
    void setXSL(const QString &document, QString style = QString::null);
    /**
     * Merges XML and XSLT and produces HTML.
     * @param xmlString XML string to merge.
     *
     * @return HTML in a QString.
     */
    QString parse(QString xmlString);

    /**
     * Destructor that frees and closes any opened XSLT document.
     */
    ~KSXSLHandler();

  private:
    /**
     * Opens a XSLT style and parse it.
     * @param path Path to the document.
     * @param style Currently used style.
     *
     * @return Returns QCString for making an xslDoc.
     */
    QCString openXSL(QString path, QString style);

};

#endif
