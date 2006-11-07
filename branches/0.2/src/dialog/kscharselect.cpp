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
#include "kscharselect.h"

#include "kslovar.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qfont.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <kcharselect.h>
#include <klocale.h>

KSCharSelect::KSCharSelect(QWidget *reciever, QWidget *parent, const char *name)
  : KDialogBase(parent, name, true, i18n("Select character"), Close)
{
  //Create the layout
  QVBox *main = new QVBox(this);
  QHBox *misc = new QHBox(main);

  //Create the first row
  QLabel *tableLabel = new QLabel(i18n("Table"), this);
  tableLabel->resize(tableLabel->sizeHint());
  tableLabel->setMaximumWidth(tableLabel->sizeHint().width());
  misc->layout()->add(tableLabel);
  m_table = new QSpinBox(0, 255, 1, this);
  m_table->resize(m_table->sizeHint());
  m_table->setMaximumWidth(m_table->sizeHint().width());
  misc->layout()->add(m_table);
  misc->layout()->addItem(new QSpacerItem(1, 1));

  //The main part
  m_charTable = new KCharSelectTable(main, "Character table", main->font().family(), ' ', 0);

  setMainWidget(main);

  //Connect the signals...
  connect(m_charTable, SIGNAL(activated(const QChar&)), reciever, SLOT(slotAddChar(const QChar&)));
  connect(m_table, SIGNAL(valueChanged(int)), this, SLOT(slotTableChange(int)));
}

void KSCharSelect::slotTableChange(int table)
{
  m_charTable->setTableNum(table);
}

KSCharSelect::~KSCharSelect()
{
}


#include "kscharselect.moc"
