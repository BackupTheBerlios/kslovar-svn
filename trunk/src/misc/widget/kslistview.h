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
///@TODO need to remove this include!
#include <qstyle.h>

class QCustomEvent;

/**
 * @short Child class of KListView. Made for easier handling of lists.
 * For more info, visit the KDE's documentation.
 * @author Gregor Kališnik
 */
class KSListView : public KListView
{
  Q_OBJECT

  signals:
    /**
     * Signal for telling that the KSListView recieved a package.
     * @param found Tells if the word was found.
     * @param completed Tells if it is completed with the search.
     */
    void recievedPackage(bool found, bool completed);

  public:
    /**
     * Constructor for creating KSListView. See KDE documentation for more info.
     * @param parent Parent of this class.
     * @param name Name of this class.
     */
    KSListView(QWidget *parent=0, const char *name=0);

    /**
     * An empty destructor.
     */
    ~KSListView();

    /**
     * Sets the text to be shown when there is no items in the list.
     * @param text Text to be shown.
     */
    void setEmptyText(const QString &text);

    /**
     * Adds a word to the filter.
     */
    void addFilter(int id);
    /**
     * Deletes a word from the filter.
     */
    void delFilter(int id);

  protected:
    /**
     * Redeclared method for capturing recieved packages.
     * See Qt's documentation for more info.
     */
    virtual bool eventFilter(QObject *o, QEvent *e);
    /**
     * Redeclared method for processing the packages.
     * See Qt's documentation for more info.
     * @param package Pointer to the package.
     */
    virtual void customEvent(QCustomEvent *package);
    /**
     * Redefined method for showing the empty text.
     * See Qt's documentation for more info.
     */
    virtual void drawContentsOffset(QPainter *p, int ox, int oy, int cx, int cy, int cw, int ch);
    /**
     * Method for handling timer events.
     * See Qt's documentation for more info.
     */
    void timerEvent(QTimerEvent *e);

  private slots:
    /**
     * Method for updatig the widget, when the configuration changes.
     */
    void slotUpdateConfiguration();
    /**
     * Slot that changes the position of the vertical scroll bar when moving with arrow keys (changing selection).
     * @param item Pointer to the item, that was selected
     */
    void slotCurrentChanged(QListViewItem *item);

  private:
    double m_value;
    double m_smoothValue;
    double m_smoothTimer;
    double m_smoothTimerInterval;
    double m_YDrag;
    int m_continuousLineTimer;
    int m_continuousPageTimer;
    int m_continuousPressCounter;
    int m_continuousLineTimerWait;
    int m_continuousPageTimerWait;
    int m_continuousLineTimerInterval;
    int m_continuousPageTimerInterval;
    QStyle::SubControl m_pressedButton;
    int m_smoothLineStep;
    int m_smoothPageStep;
    bool m_mousePressed;
    bool m_mouseConfig;

    QValueList<int> m_filter;

    QString m_emptyText;
};

#endif
