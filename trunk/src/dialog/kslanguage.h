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
#ifndef KSLANGUAGES_H
#define KSLANGUAGES_H

#include <kdialogbase.h>

class KSLanguageWdt;

/**
@author Gregor Kališnik
 */
class KSLanguage : public KDialogBase
{
  Q_OBJECT
  public:
    KSLanguage(QWidget *parent=0, const char *caption=0, QString language=0l, int langid=0l, const char *name=0);

    ~KSLanguage();

  protected slots:
    void slotOk();
    void slotApply();

  private:
    KSLanguageWdt *m_mainWidget;
    QString m_path;
    int m_id;
    bool m_edit;

    void populateTypeList();
    bool save();

  private slots:
    void slotAddType();
    void slotDeleteType();
    void slotEnableApply();

};

#endif
