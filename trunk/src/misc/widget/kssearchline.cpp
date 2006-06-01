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
#include "kslistviewsearchline.h"

#include "misc/ksdata.h"
#include "misc/ksquery.h"

#include "handler/ksdbhandler.h"

#include "dialog/kscharselect.h"

#include <kiconloader.h>
#include <ktoolbar.h>
//#include <ktoolbarbutton.h>

//#include <qtimer.h>
#include <qapplication.h>
#include <qtoolbutton.h>
//#include <qiconset.h>

KSSearchLine::KSSearchLine(QWidget *parent, const char *name)
  : QHBox(parent, name)
{
  setSpacing(1);
  //QTimer::singleShot(0, this, SLOT(createWidget()));
  createWidget();
  //connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotQueueSearch(const QString &)));
}

void KSSearchLine::createWidget()
{
  m_clearButton = new QToolButton(this);
  m_clearButton->setIconSet(SmallIconSet(QApplication::reverseLayout() ? "clear_left" : "locationbar_erase"));
  m_clearButton->show();

  m_search = new KSListViewSearchLine(this);
  m_search->show();

  m_backButton = new QToolButton(this);
  m_backButton->setIconSet(SmallIconSet("previous"));
  m_backButton->setToggleButton(true);
  m_backButton->show();

  m_literalButton = new QToolButton(this);
  m_literalButton->setIconSet(SmallIconSet("filter"));
  m_literalButton->setToggleButton(true);
  m_literalButton->show();

  m_charButton = new QToolButton(this);
  m_charButton->setIconSet(SmallIconSet("kcharselect"));
  m_charButton->show();

  connect(m_clearButton, SIGNAL(clicked()), m_search, SLOT(clear()));
  connect(m_charButton, SIGNAL(clicked()), this, SLOT(slotCharSelect()));
  connect(m_backButton, SIGNAL(toggled(bool)), this, SLOT(slotToggleBack(bool)));
  connect(m_literalButton, SIGNAL(toggled(bool)), this, SLOT(slotToggleLiteral(bool)));
}

/*void KSSearchLine::processSearch()
{
  if(m_latestCriteria.isEmpty())
{
    if(text().isEmpty())
{
      return;
}
    m_latestCriteria=text();
}
  //KSData::instance()->getDictionary()->addQueue(KSQuery(SEARCH, m_latestCriteria, m_outputList));
}*/

void KSSearchLine::setList(KSListView *list)
{
  if(m_search)
  {
    m_search->setListView(static_cast<KListView*> (list));
  }
}

/*void KSSearchLine::slotQueueSearch(const QString &criteria)
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
}*/

void KSSearchLine::clear()
{
  if(m_search)
  {
    m_search->clear();
  }
}

void KSSearchLine::setText(const QString &text)
{
  if(m_search)
  {
    m_search->setText(text);
  }
}

void KSSearchLine::manualSearch(const QString &criteria)
{
  if(m_search)
  {
    m_search->updateSearch(criteria);
  }
}

void KSSearchLine::appendText(const QString &text)
{
  if(m_search)
  {
    m_search->setText(m_search->text() + text);
  }
}

void KSSearchLine::slotCharSelect()
{
  KSCharSelect *charSelect = new KSCharSelect(this);
  charSelect->show();
}

void KSSearchLine::slotAddChar(const QChar& selected)
{
  appendText(QString(selected));
}

void KSSearchLine::slotToggleLiteral(bool enabled)
{
  KSData::instance()->setLiteralSearch(enabled);
}

void KSSearchLine::slotToggleBack(bool enabled)
{
  KSData::instance()->setBackSearch(enabled);
}

KSSearchLine::~KSSearchLine()
{
}


#include "kssearchline.moc"
