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
#ifndef KSAPPEARANCE_H
#define KSAPPEARANCE_H

#include "ui/ksappearancewdt.h"

class QListViewItem;
class KHTMLPart;
class KSXSLHandler;

/**
 * @short Class responsible for Appearance configuration.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSAppearance: public KSAppearanceWdt {
Q_OBJECT
  public:
    /**
     * KSAppearance's constructor.
     * @param parent Define it's parent.
     * @param name Name of the widget. (Not caption!)
     */
    KSAppearance(QWidget *parent = 0, const char *name = 0);

  private slots:
    /**
     * Slot for selecting a style.
     * @param selected Selected item from KSListView.
     */
    void slotSelectStyle(QListViewItem *selected);
    /**
     * Slot for initiating copy job.
     */
    void slotCopyStyle();
    /**
     * Same as slotCopyStyle(), just thats deletes :).
     */
    void slotDeleteStyle();
    /**
     * Slot for editing the selected style.
     */
    void slotEditStyle();

  private:
    /**
     * Method that populates the list with styles.
     */
    void populateStyleList();

    QString m_exampleDefault;
    QString m_exampleTransitional;

    KHTMLPart *m_previewDefault;
    KSXSLHandler *m_defaultStyleParser;
    KHTMLPart *m_previewTransitional;
    KSXSLHandler *m_transitionalStyleParser;
};

#endif
