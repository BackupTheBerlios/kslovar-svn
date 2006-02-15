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
#include "kslistviewsearchline.h"

#include "kslistview.h"
#include "kslistviewitem.h"

#include "../ksdata.h"

#include <kdebug.h>

/*KSListViewSearchLine::KSListViewSearchLine(QWidget *parent, KSListView *listView, const char *name)
  : KListViewSearchLine(parent, static_cast<KListView*> (listView), name)
{
}*/

KSListViewSearchLine::KSListViewSearchLine(QWidget *parent, const char *name)
  : KListViewSearchLine(parent, name)
{
}

bool KSListViewSearchLine::itemMatches(const QListViewItem *item, const QString &s) const
{
  if(s.isEmpty())
  {
    return true;
  }

  if(!KSData::instance()->literalSearch())
  {
    if(static_cast<KSListViewItem*> (const_cast<QListViewItem*> (item))->getSearch().find(s, 0, 0) >= 0)
    {
      return true;
    }
  }
  else
  {
    if(static_cast<KSListViewItem*> (const_cast<QListViewItem*> (item))->text(0).find(s, 0, 0) >= 0)
    {
      return true;
    }
  }

  return false;
}

KSListViewSearchLine::~KSListViewSearchLine()
{
}


#include "kslistviewsearchline.moc"
