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
#ifndef KSOUTPUTHANDLER_H
#define KSOUTPUTHANDLER_H

#include <qevent.h>

#define LIST 1000
#define CLEAR 1001
#define NORESULT 1002
#define TEXT 1003

/**
 * @short Class for handling output packages. (data)
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSOutputHandler : public QCustomEvent
{
  public:
    /**
     * Constructor for outputing packages for list of phrases. (Used with the search)
     */
    KSOutputHandler(const QString &name, const QString &id, const QString &search);
    /**
     * Construct (currently unused) for outputing results of a normal string SELECT command. (It could replace processString())
     * @see KSDBHandler#processString()
     * @param text Result from query.
     */
    KSOutputHandler(const QString &text);

    /**
     * For reading the name of the word, gotted from search.
     * @return Name of the word.
     */
    QString getName();
    /**
     * For reading the ID of the word, gotted from search.
     * @return ID of the word.
     */
    QString getId();
    /**
     * For reading the search string of the word, gotted from search.
     * @return Search string of the word.
     */
    QString getSearch();

    /**
     * For reading result from a query.
     * @return Result from query.
     * @return @c QString::null No results.
     */
    QString getText();

    /**
     * Empty destructor.
     */
    ~KSOutputHandler();

  private:
    QString m_name, m_id, m_search;
    QString m_text;

};

#endif
