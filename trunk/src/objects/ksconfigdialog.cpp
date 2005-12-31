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
#include "ksconfigdialog.h"

#include "ksdata.h"
#include "configuration.h"

KSConfigDialog::KSConfigDialog(QWidget *parent, const char *name, KConfigSkeleton *config)
  : KConfigDialog(parent, name, config), m_modified(false)
{
  connect(this, SIGNAL(okClicked()), this, SLOT(slotLocalOk()));
  connect(this, SIGNAL(applyClicked()), this, SLOT(slotLocalApply()));
}

void KSConfigDialog::manualUpdateButtons()
{
  enableButton(Apply, true);
  m_modified=true;

  emit widgetModified();
}

void KSConfigDialog::slotLocalOk()
{
  if(m_modified)
  {
    save();
  }
}

void KSConfigDialog::slotLocalApply()
{
  save();
}

void KSConfigDialog::save()
{
  Configuration::writeConfig();
  m_modified=false;
}

KSConfigDialog::~KSConfigDialog()
{
}


#include "ksconfigdialog.moc"
