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
#ifndef KSCONFIGDIALOG_H
#define KSCONFIGDIALOG_H

#include <kconfigdialog.h>

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSConfigDialog : public KConfigDialog
{
  Q_OBJECT

  public:
    KSConfigDialog(QWidget *parent = 0, const char *name = 0, KConfigSkeleton *config = 0);

    static KSConfigDialog *instance();

    void manualUpdateButtons();

    ~KSConfigDialog();

  private slots:
    void slotLocalOk();
    void slotLocalApply();

  private:
    void save();


    bool m_modified;
    static KSConfigDialog *m_instance;

};

#endif