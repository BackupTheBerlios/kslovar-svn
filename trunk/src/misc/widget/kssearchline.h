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

#define NONE 0
#define SYNONYM 1
#define ANTONYM 2
#define FAMILY 3

class KSListView;

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
*/
class KSSearchLine : public KLineEdit
{
Q_OBJECT
public:
    KSSearchLine(QWidget *parent = 0, const char *name = 0, KSListView *list = 0);

    void setList(KSListView *list);
    void processSearch();

    void setType(unsigned short type);

    ~KSSearchLine();

  private slots:
    void slotQueueSearch(const QString &criteria);
    void slotBeginSearch();

  private:
    KSListView *m_outputList;
    int m_searchQueue;
    QString m_latestCriteria;
    unsigned short m_type;

};

#endif
