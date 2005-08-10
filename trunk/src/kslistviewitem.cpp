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
#include "kslistviewitem.h"

#include <klistview.h>

KSListViewItem::KSListViewItem(KListView *parent, QString label1, QString search, QString id)
  : KListViewItem(parent, label1), m_id(id), m_search(search)
{
}

QString KSListViewItem::getId()
{
  return m_id;
}

QString KSListViewItem::getSearch()
{
  return m_search;
}

KSListViewItem::~KSListViewItem()
{
}


//#include "kslistviewitem.moc"
