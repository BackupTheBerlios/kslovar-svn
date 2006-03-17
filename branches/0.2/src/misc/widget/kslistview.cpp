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
#include "kslistview.h"

#include "kslistviewitem.h"

#include "../../configuration/configuration.h"
#include "../../configuration/ksconfigdialog.h"

#include "../ksdata.h"

#include "../../kslovar.h"

#include "../../handler/ksdbhandler.h"
#include "../../handler/ksoutputhandler.h"

#include <qtimer.h>
#include <qheader.h>
#include <qpainter.h>

#include <klocale.h>

KSListView::KSListView(QWidget *parent, const char *name)
  : KListView(parent, name), mouseConfig(Configuration::mouseNavigation())
{
  header()->hide();
  connect( dynamic_cast<QObject*> (KSConfigDialog::instance()), SIGNAL(settingsChanged()), this, SLOT(slotUpdateConfiguration()) );
  verticalScrollBar()->installEventFilter(this);
  if(!Configuration::scrollBar())
  {
    setVScrollBarMode(AlwaysOff);
  }
}

bool KSListView::eventFilter(QObject *o, QEvent *e)
{
  if(o==viewport())
  {
    if(mouseConfig)
    {
      QMouseEvent *event = static_cast<QMouseEvent*>(e);
      if(e->type() == QEvent::MouseMove)
      {
        const double offset = static_cast<double>(visibleHeight())/50.0 + 5.0;
        m_value = ( event->y() - offset ) * ( static_cast<double>(verticalScrollBar()->maxValue()) / ( static_cast<double>(visibleHeight()) - offset * 2 ) );
        verticalScrollBar()->setValue(static_cast<int> (m_value));
      }
    }
  }
  return KListView::eventFilter(o, e);
}

void KSListView::slotUpdateConfiguration()
{
  mouseConfig=Configuration::mouseNavigation();
  if(!Configuration::scrollBar())
  {
    setVScrollBarMode(AlwaysOff);
  }
  else
  {
    setVScrollBarMode(AlwaysOn);
  }
}

void KSListView::customEvent(QCustomEvent *package)
{
  if(package->type() == LIST)
  {
    /*if(!isEnabled())
    {*/
    m_emptyText = i18n("No words found");
    if(KSData::instance()->getDictionary()->isSkiped())
    {
      return;
    }
      /*this->clear();
    this->setDisabled(false);*/
    //}

    KSOutputHandler *input = static_cast<KSOutputHandler*> (package);
    for(QValueList<int>::iterator count = m_filter.begin(); count != m_filter.end(); count++)
    {
      if(input->getId().toInt() == *count)
      {
        triggerUpdate();
        return;
      }
    }
    emit recievedPackage(true, false);
    new KSListViewItem(this, input->getName(), input->getId(), input->getSearch());
  }
  if(package->type() == CLEAR)
  {
    emit recievedPackage(false, false);
    m_emptyText = i18n("Search in progress...");
    this->clear();
    //new KSListViewItem(this, i18n("Search in progress..."));
    //this->setDisabled(true);
  }
  if(package->type() == NORESULT)
  {
    emit recievedPackage(false, true);
    m_emptyText = i18n("No words found");
    this->clear();
    //new KSListViewItem(this, i18n("No words found."));
    //this->setDisabled(true);
  }
}

void KSListView::addFilter(int id)
{
  m_filter.append(id);
}

void KSListView::delFilter(int id)
{
  m_filter.remove(id);
}

void KSListView::setEmptyText(const QString &text)
{
  m_emptyText = text;
  //triggerUpdate();
}

void KSListView::drawContentsOffset(QPainter *p, int ox, int oy, int cx, int cy, int cw, int ch)
{
  KListView::drawContentsOffset(p, ox, oy, cx, cy, cw, ch);

  if(childCount() == 0 && !m_emptyText.isEmpty())
  {
    p->setPen(Qt::darkGray);

    QStringList lines = QStringList::split("\n", m_emptyText);
    int ypos = 10 + p->fontMetrics().height();

    QStringList::Iterator end(lines.end());
    for(QStringList::Iterator str(lines.begin()); str != end; str++)
    {
      p->drawText((viewport()->width()/2)-(p->fontMetrics().width(*str)/2), ypos, *str);
      ypos += p->fontMetrics().lineSpacing();
    }
  }
}

KSListView::~KSListView()
{
}


#include "kslistview.moc"
