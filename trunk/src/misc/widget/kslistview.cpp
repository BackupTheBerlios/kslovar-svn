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

#include "configuration/configuration.h"
#include "configuration/ksconfigdialog.h"

#include "misc/ksdata.h"

#include "kslovar.h"

#include "handler/ksdbhandler.h"
#include "handler/ksoutputhandler.h"

#include <qtimer.h>
#include <qheader.h>
#include <qpainter.h>

#include <klocale.h>

KSListView::KSListView(QWidget *parent, const char *name)
  : KListView(parent, name), m_value(0), m_smoothValue(0),
 m_smoothTimer(0), m_smoothTimerInterval(30), m_YDrag(0),
 m_continuousLineTimer(0), m_continuousPageTimer(0), m_continuousPressCounter(0),
 m_continuousLineTimerWait(10), m_continuousPageTimerWait(30),
 m_continuousLineTimerInterval(40), m_continuousPageTimerInterval(400),
 m_pressedButton(QStyle::SC_None), m_smoothLineStep(0), m_smoothPageStep(0),
 m_mousePressed(false)
{
  header()->hide();
  connect( dynamic_cast<QObject*> (KSConfigDialog::instance()), SIGNAL(settingsChanged()), this, SLOT(slotUpdateConfiguration()) );
  slotUpdateConfiguration();
}

bool KSListView::eventFilter(QObject *o, QEvent *e)
{
  if (o == verticalScrollBar()) {
    QScrollBar *bar = static_cast<QScrollBar*> (o);
    if (e->type() == QEvent::Wheel)
    {
      // OK, this is a wheel event, let's get our QWheelEvent in an unsafe way, due to a bug in RTTI of QT.
      QWheelEvent *event = static_cast<QWheelEvent*> (e);
      // Set new target value
      m_value -= event->delta();
      // Make sure it's in the boundaries of scroll bar
      m_value = QMAX( m_value, bar->minValue() );
      m_value = QMIN( m_value, bar->maxValue() );

      // Ignore the event
      return true;
    }
    else
    {
      if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick)
      {
      // We are intercepting all clicks and double clicks on the scrollbar. Unless we do so
      // scroll bar immediatly goes to the point wherever user's click requests it to.
      // Then smooth scroll begins, and animates the scrolling, but since the scrollbar
      // goes to the destionation point for a very small amount of time at the very beginning of
      // the click, this causes flickering. So we intercept each click, and make the scroll bar
      // go to it's destination by smoothly.

      // Get our QMouseEvent so that we can have our relative mouse position
        QMouseEvent *event = static_cast<QMouseEvent*> (e);
        m_pressedButton = style().querySubControl(QStyle::CC_ScrollBar, verticalScrollBar(), event->pos());

      // Get page/line step sizes. You may ask, why we are not doing this in setSmoothScrolling
      // the reason is, scroll bar might not be initialized at that moment. When we are receiving
      // MouseButtonPress or such event, we're sure that it's initialized!
        if (m_smoothLineStep == 0 && m_smoothPageStep == 0)
        {
          m_smoothLineStep = bar->lineStep();
          m_smoothPageStep = bar->pageStep();
        // Set page/line steps of the scroll bar to zero, we'll emulate them, smoothly!
        // If we don't set this to 0, when we pass the event to the button, the scollbar
        // will scroll the list too.
          verticalScrollBar()->setLineStep(0);
        }

      // OK, now we can understand which partion of the scroll bar is clicked, and do the requested thing
      // animated. Then set the step sizes to zero, and pass the event to the slider, so that user can
      // feel like he/she really pressed the buttons (on click color change).

        switch(m_pressedButton)
        {
          case (QStyle::SC_ScrollBarSlider):
          {
            m_YDrag = event->y();
            break;
          }
          case (QStyle::SC_ScrollBarSubLine):
          {
            m_value -= m_smoothLineStep;
          // Make sure if the targetScrollBarValue is in the scroll bar values range
            m_value = QMAX(m_value, verticalScrollBar()->minValue());
          // pass the event to the scroll bar so the button gets "clicked"
            return false;
            break;
          }
          case (QStyle::SC_ScrollBarSubPage):
          {
            m_value -= m_smoothPageStep;
        // Make sure if the targetScrollBarValue is in the scroll bar values range
            m_value = QMAX(m_value, verticalScrollBar()->minValue());
            break;
          }
          case (QStyle::SC_ScrollBarAddPage):
          {
            m_value += m_smoothPageStep;
        // Make sure if the targetScrollBarValue is in the scroll bar values range
            m_value = QMIN(m_value, verticalScrollBar()->maxValue());
            break;
          }
          case (QStyle::SC_ScrollBarAddLine):
          {
            m_value += m_smoothLineStep;
        // Make sure if the targetScrollBarValue is in the scroll bar values range
            m_value = QMIN(m_value, verticalScrollBar()->maxValue());
          // pass the event to the scroll bar so the button gets "clicked"
            return false;
            break;
          }
        }
      // Now, ignore the event.
        return true;
      }
      else
      {
        if (e->type() == QEvent::MouseMove)
        {
      // Get our QMouseEvent so that we can have our relative mouse position
          QMouseEvent *event = static_cast<QMouseEvent*>(e);
          if (m_pressedButton == QStyle::SC_ScrollBarSlider)
          {
        // Mouse movement distance for this MouseMove event
            double delta = event->y() - m_YDrag;
        // Update the drag start value so in the next MouseMove event we can calculate new movement distance
            m_YDrag = event->y();
        // The length which we can move the mouse over the bar
            double scale = bar->geometry().height() - bar->sliderRect().height() - 45;
        // Scale it to scroll bar value
            m_value += static_cast<int> (static_cast<double> ((bar->maxValue() / scale) * delta ));
          }
        }
        else
        {
          if (e->type() == QEvent::MouseButtonRelease)
          {
      // Reset waiting counter. This is used to wait before simulating continuous mouse press
            m_continuousPressCounter = 0;
      // Mark all buttons as not pressed now
            m_pressedButton = QStyle::SC_None;
      // Make sure if the targetScrollBarValue is in the scroll bar values range
            m_value = QMAX(m_value, bar->minValue());
            m_value = QMIN(m_value, bar->maxValue());

      // Pass the release event to the scroll bar, which will put the buttons in off-state
            return false;
          }
          else
          {
      // Pass the event to the scroll bar
            return false;
          }
        }
      }
    }
  }
  if (o == viewport())
  {
    if (e->type() == QEvent::MouseButtonPress)
    {
      // Mark that we have pressed the button. This will prevent the list from scrolling when
      // the current item has changed due to mouse click. It's fine when the keypresses cause
      // it scroll, but not mouse.
      m_mousePressed = true;
    }
    else
    {
      if (e->type() == QEvent::MouseButtonRelease)
      {
        m_mousePressed = false;
      }
      else
      {
        if (m_mouseConfig)
        {
          QMouseEvent *event = static_cast<QMouseEvent*> (e);
          if (e->type() == QEvent::MouseMove)
          {
            const double offset = static_cast<double> (visibleHeight()) / 50.0 + 5.0;
            m_value = (event->y() - offset) * (static_cast<double>(verticalScrollBar()->maxValue()) / (static_cast<double>  (visibleHeight()) - offset * 2));
            if (!Configuration::smoothScroll())
            {
              verticalScrollBar()->setValue(static_cast<int> (m_value));
            }
          }
        }
      }
    }
  }
  return KListView::eventFilter(o, e);
}

void KSListView::slotUpdateConfiguration()
{
  m_mouseConfig = Configuration::mouseNavigation();
  if (!Configuration::scrollBar())
  {
    setVScrollBarMode(AlwaysOff);
  }
  else
  {
    setVScrollBarMode(Auto);
  }
  if (Configuration::smoothScroll())
  {
    m_smoothTimer = startTimer((int) m_smoothTimerInterval);
    verticalScrollBar()->installEventFilter(this);
    connect(this, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotCurrentChanged(QListViewItem*)));
    m_continuousLineTimer = startTimer(m_continuousLineTimerInterval);
    m_continuousPageTimer = startTimer(m_continuousPageTimerInterval);
  }
  else
  {
    killTimer((int) m_smoothTimer);
    m_smoothTimer = 0;
    verticalScrollBar()->removeEventFilter(this);
    disconnect(this, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotCurrentChanged(QListViewItem*)));
    killTimer(m_continuousLineTimer);
    m_continuousLineTimer = 0;
    killTimer(m_continuousPageTimer);
    m_continuousPageTimer = 0;
  }
}

void KSListView::timerEvent(QTimerEvent *e)
{
  if (e->timerId() == m_smoothTimer)
  {
    // Find how war we are away from the target scroll bar value and divide it by our constant (it can be both negative/positive)
    double offset = static_cast<double> ((m_value - m_smoothValue) / 6.0);
    // Add the offset to our meta current value, this is the desired precise value
    m_smoothValue += offset;
    // Cast it to integer and update the vertical scroll bar value
    verticalScrollBar()->setValue(static_cast<int> (m_smoothValue));
  }
  else
  {
    if (e->timerId() == m_continuousLineTimer)
    {
    // If the button is being pressed for a longer time, get faster
    // X = time spent untill continuous button press begins to take effect
    // Wait for X more amount of time after the continuous button press actually begins, the start to accelerate the scrolling linearly
      double acceleration = static_cast<double> (( m_continuousPressCounter - m_continuousLineTimerWait * 2)) / static_cast<double> (m_continuousLineTimerWait);
    // Let's make sure if the acceleration coefficient is between 1 and 3
      acceleration = QMAX( 1, acceleration );
      acceleration = QMIN( 3, acceleration );

    // Check if any scrollbar buttons are being pressed right, if any, honor them
      if (m_pressedButton == QStyle::SC_ScrollBarSubLine)
      {
      // Check if the user has pressed for long enough to activate continuous mouse press effect
        if (m_continuousPressCounter++ > m_continuousLineTimerWait)
        {
        // if so start continuous scrolling
          m_value -= m_smoothLineStep * acceleration;
        // Make sure the target value is not below the minimum range.
          m_value = QMAX(m_value, verticalScrollBar()->minValue());
        }
      }
      else
      {
        if (m_pressedButton == QStyle::SC_ScrollBarAddLine)
        {
          if (m_continuousPressCounter++ > m_continuousLineTimerWait)
          {
        // if so start continuous scrolling
            m_value += m_smoothLineStep * acceleration;
        // Make sure the target value is not aboce the maximum range.
            m_value = QMIN(m_value, verticalScrollBar()->maxValue());
          }
        }
      }

    }
    else
    {
      if (e->timerId() == m_continuousPageTimer)
      {
    // If the button is being pressed for a longer time, get faster
    // X = time spent untill continuous button press begins to take effect
    // Wait for X more amount of time after the continuous button press actually begins, the start to accelerate the scrolling linearly
        double acceleration = static_cast<double> ((m_continuousPressCounter - m_continuousPageTimerWait * 2)) / static_cast<double> (m_continuousPageTimerWait);
    // Let's make sure if the acceleration coefficient is between 1 and 3
        acceleration = QMAX( 1, acceleration );
        acceleration = QMIN( 3, acceleration );

        if (m_pressedButton == QStyle::SC_ScrollBarSubPage)
        {
          if (m_continuousPressCounter++ > m_continuousPageTimerWait)
          {
        // if so start continuous scrolling
            m_value -= m_smoothPageStep + acceleration;
            m_value = QMAX(m_value, verticalScrollBar()->minValue());
          }
        }
        else
        {
          if (m_pressedButton == QStyle::SC_ScrollBarAddPage)
          {
            if (m_continuousPressCounter++ > m_continuousPageTimerWait)
            {
        // if so start continuous scrolling
              m_value += m_smoothPageStep * acceleration;
              m_value = QMIN(m_value, verticalScrollBar()->maxValue());
            }
          }
        }
      }
    }
  }
}

void KSListView::slotCurrentChanged(QListViewItem *item)
{
  if (!item)
  {
    return;
  }
  // If the current item changed due to mouse click then don't center it in the listview. Do this just for key presses.
  if (m_mousePressed)
  {
    m_mousePressed = false;
    return;
  }
  m_value = itemPos(item) - static_cast<double> (visibleHeight()/2.0) + item->height();
  // Make sure it's in the boundaries of scroll bar
  m_value = QMAX(m_value, verticalScrollBar()->minValue());
  m_value = QMIN(m_value, verticalScrollBar()->maxValue());
}

void KSListView::customEvent(QCustomEvent *package)
{
  if(package->type() == LIST)
  {
    m_emptyText = i18n("No words found");
/*    if(KSData::instance()->getDictionary()->isSkiped())
    {
      return;
  }*/

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
  }
  if(package->type() == NORESULT)
  {
    emit recievedPackage(false, true);
    m_emptyText = i18n("No words found");
    this->clear();
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
