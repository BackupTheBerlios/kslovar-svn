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
#ifndef KSCONVERSION_H
#define KSCONVERSION_H

#include <kdialogbase.h>

class KSConversionWdt;

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSConversion : public KDialogBase
{
  Q_OBJECT
  public:
    KSConversion(QWidget *parent = 0, const char *name = 0);

    ~KSConversion();

  private slots:
    void slotAddConversion();
    void slotDeleteConversion();
    void slotSave(QListViewItem*, const QString &text, int col);

  protected slots:
    void slotOk();

  private:
    KSConversionWdt *m_mainWidget;

    void populateConversionList();

};

#endif
