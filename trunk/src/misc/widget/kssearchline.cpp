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
#include "kssearchline.h"

#include "kslistviewitem.h"
#include "kslistview.h"

#include "misc/ksdata.h"
#include "misc/ksquery.h"

#include "handler/ksdbhandler.h"

#include <qtimer.h>

KSSearchLine::KSSearchLine(QWidget *parent, const char *name, KSListView *list)
  : KLineEdit(parent, name), m_outputList(list), m_searchQueue(0)
{
  connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotQueueSearch(const QString &)));
}

void KSSearchLine::processSearch()
{
  if(m_latestCriteria.isEmpty())
  {
    if(text().isEmpty())
    {
      return;
    }
    m_latestCriteria=text();
  }
  KSData::instance()->getDictionary()->addQueue(KSQuery(SEARCH, m_latestCriteria, m_outputList));
}

void KSSearchLine::setList(KSListView *list)
{
  m_outputList = list;
}

void KSSearchLine::slotQueueSearch(const QString &criteria)
{
  m_latestCriteria = criteria;
  m_searchQueue++;
  QTimer::singleShot(1000, this, SLOT(slotBeginSearch()));
}

void KSSearchLine::slotBeginSearch()
{
  m_searchQueue--;

  if(!m_searchQueue)
  {
    processSearch();
  }
}

KSSearchLine::~KSSearchLine()
{
}


#include "kssearchline.moc"
