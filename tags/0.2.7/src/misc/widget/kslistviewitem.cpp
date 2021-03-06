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
#include "kslistviewitem.h"

#include "handler/ksdbhandler.h"

#include "kslistview.h"

KSListViewItem::KSListViewItem(KListView *parent, const QString &label1, const QString &id, const QString &search)
  : KListViewItem(parent, label1), m_id(id), m_search(search)
{
}

KSListViewItem::KSListViewItem(KSListView *parent, const QString &label1, const QString &id, const QString &search)
  : KListViewItem(parent, label1), m_id(id), m_search(search)
{
}

KSListViewItem::KSListViewItem(const QString &label1, const QString &id, const QString &search)
  : KListViewItem((KListView*) 0, label1), m_id(id), m_search(search)
{
}

KSListViewItem::KSListViewItem(KListView *parent, QListViewItem *after, const QString &label1, const QString &id)
  : KListViewItem(parent, after, label1), m_id(id)
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

void KSListViewItem::setId(int id)
{
  m_id.setNum(id);
}

void KSListViewItem::setName(const QString &name)
{
  setText(0, name);
  m_search = KSDBHandler::convertString(name);
}

KSListViewItem::~KSListViewItem()
{
}
