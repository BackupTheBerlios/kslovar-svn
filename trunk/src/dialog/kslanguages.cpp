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

#include "ui/languageswdt.h"

#include "../misc/widget/kslistview.h"
#include "../misc/widget/kslistviewitem.h"

#include "../handler/dbhandler.h"

#include "../kslovar.h"

#include <qregexp.h>
#include <qlistview.h>

#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

KSLanguages::KSLanguages(QWidget *parent, const char *caption, QString language, int langid, const char *name)
  : KDialogBase(parent, name, true, caption), m_id(langid)
{
  m_path=locate("appdata", "languages.ksl");
  enableButtonApply(false);

  m_mainWidget=new LanguagesWdt(this);

  m_mainWidget->typeList->setItemsRenameable(true);
  m_mainWidget->typeList->setFullWidth(true);
  m_mainWidget->typeList->addColumn("name");
  m_mainWidget->typeList->setSorting(-1);

  if(!language.isEmpty())
  {
    m_mainWidget->nameEdit->setText(language);
    populateTypeList();
    m_edit=true;
  }
  else
  {
    QStringList test=DBHandler::instance(m_path)->processList("SELECT id FROM language;");
    m_id=test.last().toInt()+1;
    m_edit=false;
  }

  connect(m_mainWidget->addTypeButton, SIGNAL(clicked()), this, SLOT(slotAddType()));
  connect(m_mainWidget->deleteTypeButton, SIGNAL(clicked()), this, SLOT(slotDeleteType()));

  connect(m_mainWidget->typeList, SIGNAL(itemAdded(QListViewItem*)), this, SLOT(slotEnableApply()));
  connect(m_mainWidget->typeList, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotEnableApply()));

  setMainWidget(m_mainWidget);
}

void KSLanguages::populateTypeList()
{
  QStringList types=DBHandler::instance(m_path)->processList("SELECT id, name FROM type WHERE id_lang='"+QString::number(m_id+1)+"';", 2);
  if(!types.isEmpty())
  {
    for(QStringList::iterator count=types.begin(); count!=types.end(); count++)
    {
      QString text=*count;
      QString id=*count;
      new KSListViewItem(m_mainWidget->typeList, text.remove(QRegExp(".+/")), id.remove(QRegExp("/.+")));
    }
  }
}

/*void KSLanguages::slotPopulateSecondaryList(QListViewItem *selectedItem)
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
}*/

void KSLanguages::slotAddType()
{
  KSListViewItem *item;
  QListViewItem *last=m_mainWidget->typeList->lastItem();
  if(!last)
  {
    item=new KSListViewItem(m_mainWidget->typeList, i18n("Name"));
    item->setId(1);
  }
  else
  {
    item=new KSListViewItem(m_mainWidget->typeList, last, i18n("Name"));
    item->setId(static_cast<KSListViewItem*> (last)->getId().toInt()+1);
  }
}

void KSLanguages::slotDeleteType()
{
  delete m_mainWidget->typeList->selectedItem();
}

/*void KSLanguages::slotAddSecondary()
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
}*/

void KSLanguages::slotEnableApply()
{
  enableButtonApply(true);
}

void KSLanguages::slotOk()
{
  if(!save())
  {
    KMessageBox::error(this, i18n("Cannot save language!"));
    return;
  }
  emit okClicked();
  accept();
}

void KSLanguages::slotApply()
{
  if(!save())
  {
    KMessageBox::error(this, i18n("Cannot save language!"));
    return;
  }
  enableButtonApply(false);
  emit applyClicked();
}

bool KSLanguages::save()
{
  if(m_edit)
  {
    if(!DBHandler::instance(m_path)->processQuery("UPDATE language SET name='"+m_mainWidget->nameEdit->text()+"' WHERE id='"+QString::number(m_id)+"'; DELETE FROM type WHERE langid='"+QString::number(m_id)+"';"))
    {
      return false;
    }
    for(QListViewItem *count=m_mainWidget->typeList->firstChild();count;count=count->nextSibling())
    {
      KSListViewItem *temp=static_cast<KSListViewItem*> (count);
      if(!DBHandler::instance(m_path)->processQuery("INSERT INTO type ( name , id_lang ) VALUES ( '"+temp->text(0)+"' , '"+QString::number(m_id)+"' );"))
      {
        return false;
      }
    }
  }
  else
  {
    if(!DBHandler::instance(m_path)->processQuery("INSERT INTO language ( name ) VALUES ( '"+m_mainWidget->nameEdit->text()+"' );"))
    {
      return false;
    }
    for(QListViewItem *count=m_mainWidget->typeList->firstChild();count;count=count->nextSibling())
    {
      KSListViewItem *temp=static_cast<KSListViewItem*> (count);
      if(!DBHandler::instance(m_path)->processQuery("INSERT INTO type ( name , id_lang ) VALUES ( '"+temp->text(0)+"' , '"+QString::number(m_id)+"' );"))
      {
        return false;
      }
    }
  }
  //Instances::mainInstance()->loadLanguages();
  KSlovar::KSInstance()->loadLanguages();
  return true;
}

KSLanguages::~KSLanguages()
{
}


#include "kslanguages.moc"
