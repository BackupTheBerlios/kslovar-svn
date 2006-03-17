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
#ifndef KSSEARCHLINE_H
#define KSSEARCHLINE_H

#include <klineedit.h>

class KSListView;

/**
 * @short Class for searching.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSSearchLine : public KLineEdit
{
  Q_OBJECT
  public:
    /**
     * Constructor for creating KSSearchLine.
     * @param parent Parent of the class.
     * @param name Name of the class.
     * @param list Pointer to the list.
     */
    KSSearchLine(QWidget *parent = 0, const char *name = 0, KSListView *list = 0);

    /**
     * Sets the list.
     * @param list Pointer to the list.
     */
    void setList(KSListView *list);
    /**
     * Sends the search criteria to the KSDBHandler's thread.
     */
    void processSearch();

    /**
     * An empty destructor.
     */
    ~KSSearchLine();

  private slots:
    /**
     * Slot for queueing search requests.
     * @param criteria Search criteria.
     */
    void slotQueueSearch(const QString &criteria);
    /**
     * Continuetion slot for queueing search requests.
     * @see KSSearchLine#slotQueueSearch(const QString &criteria)
     */
    void slotBeginSearch();

  private:
    KSListView *m_outputList;
    int m_searchQueue;
    QString m_latestCriteria;

};

#endif
