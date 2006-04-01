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
#include "kslanguage.h"

#include "ui/kslanguagewdt.h"

#include "misc/widget/kslistview.h"
#include "misc/widget/kslistviewitem.h"
#include "misc/ksdata.h"

#include "handler/ksdbhandler.h"

#include "kslovar.h"

#include <qregexp.h>
#include <qlistview.h>

#include <klineedit.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>

KSLanguage::KSLanguage(QWidget *parent, const char *caption, const QString &language, int langid, const char *name)
  : KDialogBase(parent, name, true, caption), m_id(langid)
{
  m_path=locate("appdata", "languages.ksl");
  enableButtonApply(false);

  m_mainWidget=new KSLanguageWdt(this);

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
    QStringList test=KSData::instance()->getLanguageHandler()->processList("SELECT id FROM language;");
    m_id=test.last().toInt()+1;
    m_edit=false;
  }

  connect(m_mainWidget->addTypeButton, SIGNAL(clicked()), this, SLOT(slotAddType()));
  connect(m_mainWidget->deleteTypeButton, SIGNAL(clicked()), this, SLOT(slotDeleteType()));

  connect(m_mainWidget->typeList, SIGNAL(itemAdded(QListViewItem*)), this, SLOT(slotEnableApply()));
  connect(m_mainWidget->typeList, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotEnableApply()));

  setMainWidget(m_mainWidget);
}

void KSLanguage::populateTypeList()
{
  QStringList types=KSData::instance()->getLanguageHandler()->processList("SELECT id, name FROM type WHERE id_lang='"+QString::number(m_id+1)+"';", 2);
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

void KSLanguage::slotAddType()
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

void KSLanguage::slotDeleteType()
{
  delete m_mainWidget->typeList->selectedItem();
}

void KSLanguage::slotEnableApply()
{
  enableButtonApply(true);
}

void KSLanguage::slotOk()
{
  if(!save())
  {
    KMessageBox::error(this, i18n("Cannot save language!"));
    return;
  }
  emit okClicked();
  accept();
}

void KSLanguage::slotApply()
{
  if(!save())
  {
    KMessageBox::error(this, i18n("Cannot save language!"));
    return;
  }
  enableButtonApply(false);
  emit applyClicked();
}

bool KSLanguage::save()
{
  if(m_edit)
  {
    if(!KSData::instance()->getLanguageHandler()->processQuery("UPDATE language SET name='"+m_mainWidget->nameEdit->text()+"' WHERE id='"+QString::number(m_id)+"'; DELETE FROM type WHERE id_lang='"+QString::number(m_id)+"';"))
    {
      return false;
    }
    for(QListViewItem *count=m_mainWidget->typeList->firstChild();count;count=count->nextSibling())
    {
      KSListViewItem *temp=static_cast<KSListViewItem*> (count);
      if(!KSData::instance()->getLanguageHandler()->processQuery("INSERT INTO type ( name , id_lang ) VALUES ( '"+temp->text(0)+"' , '"+QString::number(m_id)+"' );"))
      {
        return false;
      }
    }
  }
  else
  {
    if(!KSData::instance()->getLanguageHandler()->processQuery("INSERT INTO language ( name ) VALUES ( '"+m_mainWidget->nameEdit->text()+"' );"))
    {
      return false;
    }
    for(QListViewItem *count=m_mainWidget->typeList->firstChild();count;count=count->nextSibling())
    {
      KSListViewItem *temp=static_cast<KSListViewItem*> (count);
      if(!KSData::instance()->getLanguageHandler()->processQuery("INSERT INTO type ( name , id_lang ) VALUES ( '"+temp->text(0)+"' , '"+QString::number(m_id)+"' );"))
      {
        return false;
      }
    }
  }
  KSlovar::KSInstance()->loadLanguages();
  return true;
}

KSLanguage::~KSLanguage()
{
}


#include "kslanguage.moc"
