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
#include "kslanguages.h"

#include "../ui/languageswdt.h"
#include "../objects/kslistview.h"
#include "../objects/kslistviewitem.h"
#include "../dbhandler.h"

#include <qregexp.h>

#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kdebug.h>

KSLanguages::KSLanguages(QWidget *parent, const char *caption, QString language, QString path, int langid, const char *name)
  : KDialogBase(parent, name, true, caption), m_path(path), m_id(langid)
{
  enableButtonApply(false);

  m_mainWidget=new LanguagesWdt(this);

  m_mainWidget->primaryList->setItemsRenameable(true);
  m_mainWidget->primaryList->setFullWidth(true);
  m_mainWidget->primaryList->addColumn("name");
  m_mainWidget->primaryList->setSorting(-1);
  m_mainWidget->secondaryList->setItemsRenameable(true);
  m_mainWidget->secondaryList->setFullWidth(true);
  m_mainWidget->secondaryList->addColumn("name");
  m_mainWidget->secondaryList->setSorting(-1);

  if(!language.isNull())
  {
    m_mainWidget->nameEdit->setText(language);
    populatePrimaryList(langid);
  }
  else
  {
    int st=0;
    QStringList test=DBHandler::instance(m_path)->processString("SELECT id FROM languages;");
    for(QStringList::iterator count=test.begin();count!=test.end();count++)
    {
      st++;
    }
    m_id=st;
    kdDebug() << m_id << endl;
  }

  connect(m_mainWidget->primaryList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(slotPopulateSecondaryList(QListViewItem*)));
  connect(m_mainWidget->addPrimaryButton, SIGNAL(clicked()), this, SLOT(slotAddPrimary()));
  connect(m_mainWidget->deletePrimaryButton, SIGNAL(clicked()), this, SLOT(slotDeletePrimary()));
  connect(m_mainWidget->addSecondaryButton, SIGNAL(clicked()), this, SLOT(slotAddSecondary()));
  connect(m_mainWidget->deleteSecondaryButton, SIGNAL(clicked()), this, SLOT(slotDeleteSecondary()));

  connect(m_mainWidget->primaryList, SIGNAL(itemAdded(QListViewItem*)), this, SLOT(slotEnableApply()));
  connect(m_mainWidget->primaryList, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotEnableApply()));
  connect(m_mainWidget->secondaryList, SIGNAL(itemAdded(QListViewItem*)), this, SLOT(slotEnableApply()));
  connect(m_mainWidget->secondaryList, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotEnableApply()));

  setMainWidget(m_mainWidget);
}

void KSLanguages::populatePrimaryList(int id)
{
  QStringList text=DBHandler::instance(m_path)->processList("SELECT id, name FROM posp WHERE langid='"+QString::number(id)+"';", 2);
  if(!text.isEmpty())
  {
    for(QStringList::iterator count=text.begin(); count!=text.end(); count++)
    {
      QString text=*count;
      QString id=*count;
      new KSListViewItem(m_mainWidget->primaryList, text.remove(QRegExp(".+/")), id.remove(QRegExp("/.+")));
    }
  }
}

void KSLanguages::slotPopulateSecondaryList(QListViewItem *selectedItem)
{
  m_mainWidget->secondaryList->clear();
  QStringList text=DBHandler::instance(m_path)->processList("SELECT id, name FROM poss WHERE pospid='"+static_cast<KSListViewItem*> (selectedItem)->getId()+"' AND langid='"+QString::number(m_id)+"';", 2);
  if(!text.isEmpty())
  {
    for(QStringList::iterator count=text.begin(); count!=text.end(); count++)
    {
      QString text=*count;
      QString id=*count;
      new KSListViewItem(m_mainWidget->secondaryList, text.remove(QRegExp(".+/")), id.remove(QRegExp("/.+")));
    }
  }
}

void KSLanguages::slotAddPrimary()
{
  KSListViewItem *item;
  QListViewItem *last=m_mainWidget->primaryList->lastItem();
  if(!last)
  {
    item=new KSListViewItem(m_mainWidget->primaryList, i18n("Name"));
    item->setId(1);
  }
  else
  {
    item=new KSListViewItem(m_mainWidget->primaryList, last, i18n("Name"));
    item->setId(static_cast<KSListViewItem*> (last)->getId().toInt()+1);
  }
}

void KSLanguages::slotDeletePrimary()
{
  delete m_mainWidget->primaryList->selectedItem();
}

void KSLanguages::slotAddSecondary()
{
  KSListViewItem *item;
  QListViewItem *last=m_mainWidget->secondaryList->lastItem();
  if(!last)
  {
    item=new KSListViewItem(m_mainWidget->secondaryList, i18n("Name"));
    item->setId(1);
  }
  else
  {
    item=new KSListViewItem(m_mainWidget->secondaryList, last, i18n("Name"));
    item->setId(static_cast<KSListViewItem*> (last)->getId().toInt()+1);
  }
}

void KSLanguages::slotDeleteSecondary()
{
  delete m_mainWidget->secondaryList->selectedItem();
}

void KSLanguages::slotEnableApply()
{
  enableButtonApply(true);
}

void KSLanguages::slotOk()
{
  accept();
}

void KSLanguages::slotApply()
{
  enableButtonApply(false);
}

bool KSLanguages::save()
{
  DBHandler::instance(m_path)->processQuery("DELETE * FROM languages WHERE id='"+QString::number(m_id)+"'; DELETE * FROM posp WHERE langid='"+QString::number(m_id)+"'; DELETE * FROM poss WHERE langid='"+QString::number(m_id)+"';");
}

KSLanguages::~KSLanguages()
{
}


#include "kslanguages.moc"
