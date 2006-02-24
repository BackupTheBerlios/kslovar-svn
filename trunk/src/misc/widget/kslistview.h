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
#ifndef KSLISTVIEW_H
#define KSLISTVIEW_H

#include <klistview.h>

class QCustomEvent;

/**
@author Gregor Kališnik
*/
class KSListView : public KListView
{
  Q_OBJECT

  signals:
    void recievedPackage(bool found, bool completed);

  public:
  KSListView(QWidget *parent=0, const char *name=0);

  ~KSListView();

  void addFilter(int id);
  void delFilter(int id);

  protected:

    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual void customEvent(QCustomEvent *package);

  private slots:
    void slotUpdateConfiguration();

  private:
    double m_value;
    bool mouseConfig;
    QValueList<int> m_filter;
};

#endif
