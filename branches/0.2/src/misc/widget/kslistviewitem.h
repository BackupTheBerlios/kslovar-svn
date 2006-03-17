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
#ifndef KSLISTVIEWITEM_H
#define KSLISTVIEWITEM_H

#include <klistview.h>

class KSListView;

/**
 * @short Class for handling List view items.
 * For more info see KDE's documentation.
 * @author Gregor Kališnik
 */
class KSListViewItem : public KListViewItem
{
  public:
    /**
     * Constructor for making KSListViewItem.
     * For more info see KDE's documentation.
     * @param parent Parent of the class. (KListView)
     * @param label1 Label to be shown (Example: Name of the word).
     * @param id ID of the word. (hidden)
     * @param search Search string of the word (hidden).
     */
    KSListViewItem(KListView *parent = 0, const QString &label1=0, const QString &id=0, const QString &search=0);
    /**
     * @see KSListViewItem#KSListViewItem(KListView *parent = 0, const QString &label1=0, const QString &id=0, const QString &search=0)
     */
    KSListViewItem(KSListView *parent = 0, const QString &label1=0, const QString &id=0, const QString &search=0);
    /**
     * @see KSListViewItem#KSListViewItem(KListView *parent = 0, const QString &label1=0, const QString &id=0, const QString &search=0)
     */
    KSListViewItem(const QString &label1=0, const QString &id=0, const QString &search=0);
    /**
     * @see KSListViewItem#KSListViewItem(KListView *parent = 0, const QString &label1=0, const QString &id=0, const QString &search=0)
     */
    KSListViewItem(KListView *parent = 0, QListViewItem *after=0, const QString &label1=0, const QString &id=0);

    /**
     * An empty destructor.
     */
    ~KSListViewItem();

    /**
     * Gets the ID of the word.
     *
     * @return ID of the word.
     */
    QString getId();
    /**
     * Gets the search string.
     * @return Search string of the word.
     */
    QString getSearch();
    /**
     * Sets the ID of the word.
     * @param id ID of the word.
     */
    void setId(int id);

  private:
    QString m_id;
    QString m_search;

};

#endif
