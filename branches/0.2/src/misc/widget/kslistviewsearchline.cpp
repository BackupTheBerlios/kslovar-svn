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
  QString searchString, rawString;
  if(s.isEmpty())
  {
    return true;
  }

  if(!KSData::instance()->literalSearch())
  {
    searchString = static_cast<KSListViewItem*> (const_cast<QListViewItem*> (item))->getSearch();
  }
  else
  {
    searchString = static_cast<KSListViewItem*> (const_cast<QListViewItem*> (item))->text(0);
  }

  if(!KSData::instance()->backSearch())
  {
    if(match(searchString, s, false))
    {
      return true;
    }
  }
  else
  {
    if(match(searchString, s, true))
    {
      return true;
    }
  }

  return false;
}

bool KSListViewSearchLine::match(const QString& checked, const QString& theChecker, bool reverse, bool caseSensitive) const
{
  int sourceLength, searchLength;
  QString source, search;

  if(!caseSensitive)
  {
    source = checked.upper();
    search = theChecker.upper();
  }
  else
  {
    source = checked;
    search = theChecker;
  }

  if(!reverse)
  {
    sourceLength = checked.length() -1;
    searchLength = theChecker.length() -1;
    for(int count = 0; count <= sourceLength; count++)
    {
      if(source[count] != search[count])
      {
        return false;
      }
      else
      {
        if(count == searchLength)
        {
          return true;
        }
      }
    }
  }
  else
  {
    sourceLength = checked.length();
    searchLength = theChecker.length();
    for(int count = sourceLength; count > 0; count--)
    {
      if(source[count] != search[searchLength])
      {
        return false;
      }
      else
      {
        if(searchLength == 0)
        {
          return true;
        }
        searchLength--;
      }
    }
  }

  return false;
}

KSListViewSearchLine::~KSListViewSearchLine()
{
}


#include "kslistviewsearchline.moc"
