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
#include "ksconversion.h"

#include "ui/ksconversionwdt.h"

#include "../misc/widget/kslistview.h"
#include "../misc/widget/kslistviewitem.h"

#include "../handler/ksdbhandler.h"

#include "../kslovar.h"

#include <qregexp.h>

#include <klocale.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <kdebug.h>

KSConversion::KSConversion(QWidget *parent, const char *name)
  : KDialogBase(parent, name, true, i18n("Conversion table"), Ok)
{
  m_mainWidget=new KSConversionWdt(this);
  setMainWidget(m_mainWidget);

  m_mainWidget->conversionList->setItemsRenameable(true);
  m_mainWidget->conversionList->addColumn(i18n("From"));
  m_mainWidget->conversionList->addColumn(i18n("To"));
  m_mainWidget->conversionList->setRenameable(1);

  populateConversionList();

  connect(m_mainWidget->addButton, SIGNAL(clicked()), this, SLOT(slotAddConversion()));
  connect(m_mainWidget->deleteButton, SIGNAL(clicked()), this, SLOT(slotDeleteConversion()));
  connect(m_mainWidget->conversionList, SIGNAL(itemRenamed(QListViewItem*, const QString&, int)), this, SLOT(slotSave(QListViewItem*, const QString&, int)));
}

void KSConversion::slotAddConversion()
{
  if(!KSDBHandler::instance(locate("appdata", "languages.ksl"))->processQuery("INSERT INTO conversion_table (fromc, toc) VALUES ('"+i18n("From character")+"', '"+i18n("To character")+"');"))
  {
    KMessageBox::error(this, i18n("Error saving conversion!"));
    return;
  }
  new KListViewItem(m_mainWidget->conversionList, i18n("From character"), i18n("To character"));
}

void KSConversion::slotDeleteConversion()
{
  if(!KSDBHandler::instance(locate("appdata", "languages.ksl"))->processQuery("DELETE FROM conversion_table WHERE fromc='"+m_mainWidget->conversionList->selectedItem()->text(0)+"';"))
  {
    KMessageBox::error(this, i18n("Error removing conversion!"));
    return;
  }
  delete m_mainWidget->conversionList->selectedItem();
}

void KSConversion::slotSave(QListViewItem*, const QString &text, int col)
{
  QListViewItem *item = m_mainWidget->conversionList->selectedItem();
  if(!col)
  {
    if(!KSDBHandler::instance(locate("appdata", "languages.ksl"))->processQuery("UPDATE conversion_table SET fromc='"+text+"' WHERE toc='"+item->text(1)+"';"))
    {
      KMessageBox::error(this, i18n("Error saving conversion!"));
    }
  }
  else
  {
    if(!KSDBHandler::instance(locate("appdata", "languages.ksl"))->processQuery("UPDATE conversion_table SET toc='"+text+"' WHERE fromc='"+item->text(0)+"';"))
    {
      KMessageBox::error(this, i18n("Error saving conversion!"));
    }
  }
}

void KSConversion::populateConversionList()
{
  QStringList conversionTable = KSDBHandler::instance(locate("appdata", "languages.ksl"))->processList("SELECT fromc, toc FROM conversion_table;", 2);
  for(QStringList::iterator count = conversionTable.begin(); count != conversionTable.end(); count++)
  {
    QString fromc = *count;
    QString toc = *count;
    fromc.remove(QRegExp("/.+"));
    toc.remove(fromc+"/");
    new KListViewItem(m_mainWidget->conversionList, fromc, toc);
  }
}

void KSConversion::slotOk()
{
  KSlovar::KSInstance()->loadLanguages();
  emit okClicked();
  accept();
}

KSConversion::~KSConversion()
{
}


#include "ksconversion.moc"
