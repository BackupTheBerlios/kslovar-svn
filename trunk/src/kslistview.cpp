/***************************************************************************
 *   Copyright (C) 2005 by Gregor Kališnik   *
 *   gregor@podnapisi.net   *
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
#include "kslistview.h"

#include <qtimer.h>

KSListView::KSListView(QWidget *parent, const char *name) : KListView(parent, name)
{
  verticalScrollBar()->installEventFilter(this);
  setVScrollBarMode(AlwaysOff);
}

bool KSListView::eventFilter(QObject *o, QEvent *e)
{
  if(o==viewport())
  {
    QMouseEvent *event = static_cast<QMouseEvent*>(e);
    if(e->type() == QEvent::MouseMove)
    {
      const double offset = static_cast<double>(visibleHeight())/50.0 + 5.0;
      m_value = ( event->y() - offset ) * ( static_cast<double>(verticalScrollBar()->maxValue()) / ( static_cast<double>(visibleHeight()) - offset * 2 ) );
      verticalScrollBar()->setValue(static_cast<int> (m_value));
    }
  }
  return KListView::eventFilter(o, e);
}

/*void KSListView::timerEvent(QTimerEvent *e)
{
  verticalScrollBar()->setValue(m_value);
}*/

KSListView::~KSListView()
{
}


#include "kslistview.moc"
