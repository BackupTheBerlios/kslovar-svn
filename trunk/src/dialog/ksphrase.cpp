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

#include "ksphrase.h"

#include "ui/ksphrasewdt.h"

#include "../kslovar.h"

#include "../handler/ksdbhandler.h"
#include "../handler/ksxmlhandler.h"

#include "../misc/widget/kslistview.h"
#include "../misc/widget/kslistviewitem.h"

#include "../misc/ksdata.h"

#include <qregexp.h>
#include <qdom.h>

#include <klocale.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kspell.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kdebug.h>


KSPhrase::KSPhrase(QWidget *parent, QString caption)
  : KDialogBase(parent, "KSPhrase", true, caption, Ok|Apply|Cancel), m_modified(false)
{
  m_create=true;
  initialize();
  populateAvailableList();
  enableButtonApply(false);

  setMainWidget(m_mainWidget);

  connectSlots();
}

void KSPhrase::slotAddExplanation()
{
  new KListViewItem(m_mainWidget->explanationList, i18n("Type an explanation."), i18n("Type an example."));
  slotModified();
}

void KSPhrase::slotRemoveExplanation()
{
  m_mainWidget->explanationList->takeItem(m_mainWidget->explanationList->currentItem());
  slotModified();
}

void KSPhrase::populateAvailableList()
{
  QValueList<KSPhrases> temp=KSData::instance()->getPhrases();
  for(QValueList<KSPhrases>::iterator count=temp.begin();count!=temp.end();count++)
  {
    new KSListViewItem(m_mainWidget->availableSynonymList, (*count).name, QString::number((*count).id), (*count).search);
    new KSListViewItem(m_mainWidget->availableAntonymList, (*count).name, QString::number((*count).id), (*count).search);
  }
}

void KSPhrase::slotAddSynonym()
{
  delete m_mainWidget->availableAntonymList->findItem(m_mainWidget->availableSynonymList->currentItem()->text(0), 0);

  m_mainWidget->selectedSynonymList->insertItem(m_mainWidget->availableSynonymList->currentItem());
  slotModified();
}

void KSPhrase::slotRemoveSynonym()
{
  KSListViewItem *temp=static_cast<KSListViewItem*> (m_mainWidget->selectedSynonymList->currentItem());
  new KSListViewItem(m_mainWidget->availableAntonymList, temp->text(0), temp->getId());

  m_mainWidget->availableSynonymList->insertItem(m_mainWidget->selectedSynonymList->currentItem());
  slotModified();
}

void KSPhrase::slotAddAntonym()
{
  delete m_mainWidget->availableSynonymList->findItem(m_mainWidget->availableAntonymList->currentItem()->text(0), 0);

  m_mainWidget->selectedAntonymList->insertItem(m_mainWidget->availableAntonymList->currentItem());
  slotModified();
}

void KSPhrase::slotRemoveAntonym()
{
  KSListViewItem *temp=static_cast<KSListViewItem*> (m_mainWidget->selectedAntonymList->currentItem());
  new KSListViewItem(m_mainWidget->availableSynonymList, temp->text(0), temp->getId());

  m_mainWidget->availableAntonymList->insertItem(m_mainWidget->selectedAntonymList->currentItem());
  slotModified();
}

void KSPhrase::slotBeginCheck()
{
  new KSpell(this, i18n("Spell Check"), this, SLOT(slotCheck(KSpell *)), 0, true, true);
}

void KSPhrase::slotCheck(KSpell *speller)
{
  connect(speller, SIGNAL(done(const QString&)), this, SLOT(slotEndCheck(const QString&)));
  QListViewItem *current=m_mainWidget->explanationList->currentItem();
  if(!current)
  {
    speller->cleanUp();
    return;
  }
  QString check;
  check=current->text(0)+" \n "+current->text(1);
  speller->check(check);
}

void KSPhrase::slotEndCheck(const QString& checked)
{
  QStringList check=QStringList::split(" \n ", checked);
  if((check.first()!=m_mainWidget->explanationList->currentItem()->text(0))||(check.last()!=m_mainWidget->explanationList->currentItem()->text(1)))
  {
    m_modified=true;
    enableButtonApply(true);
  }
  m_mainWidget->explanationList->currentItem()->setText(0, check.first());
  if(check.count()==2)
  {
    m_mainWidget->explanationList->currentItem()->setText(1, check.last());
  }
}

void KSPhrase::save()
{
  QValueList<KSElement> saveSynonyms, saveAntonyms;

  m_XMLHandler=new KSXMLHandler();
  m_XMLHandler->addString("word", m_mainWidget->wordEdit->text());
  m_XMLHandler->addString("type", QString::number(KSData::instance()->getPartOfSpeechId(m_mainWidget->typeBox->currentText())));

  for(QListViewItem *count=m_mainWidget->explanationList->firstChild();count;count=count->nextSibling())
  {
    m_XMLHandler->createNode("explanations");
    m_XMLHandler->addChildString("explanation", count->text(0));
    m_XMLHandler->addChildString("example", count->text(1));
  }

  for(QListViewItem *count=m_mainWidget->selectedSynonymList->firstChild();count;count=count->nextSibling())
  {
    KSElement temp;
    KSListViewItem *current=static_cast<KSListViewItem*> (count);

    m_XMLHandler->addString("synonym", current->text(0), "id", current->getId());
    temp.name=current->text(0);
    temp.id=current->getId().toInt();
    saveSynonyms << temp;
  }

  for(QListViewItem *count=m_mainWidget->selectedAntonymList->firstChild();count;count=count->nextSibling())
  {
    KSElement temp;
    KSListViewItem *current=static_cast<KSListViewItem*> (count);

    m_XMLHandler->addString("antonym", current->text(0), "id", current->getId());
    temp.name=current->text(0);
    temp.id=current->getId().toInt();
    saveAntonyms << temp;
  }

  QString xml=m_XMLHandler->parse();
  if(m_edit==true)
  {
    if(!KSDBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord(m_mainWidget->wordEdit->text(), xml, false, m_id))
    {
      KMessageBox::error(this, i18n("Cannot edit phrase!"));
    }
    KSlovar::KSInstance()->refresh();
  }
  else
  {
    if(!KSDBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord(m_mainWidget->wordEdit->text(), xml, true, 0L))
    {
      KMessageBox::error(this, i18n("Cannot add new phrase!"));
    }
    m_id=QString::number(KSDBHandler::instance(KSData::instance()->getDictionaryPath())->getId(m_mainWidget->wordEdit->text()));
    KSlovar::KSInstance()->openFile(KSData::instance()->getDictionaryPath());
  }

  for(QValueList<KSElement>::iterator count=saveSynonyms.begin();count!=saveSynonyms.end();count++)
  {
    m_XMLHandler=new KSXMLHandler(KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+QString::number((*count).id)+"';"));
    m_XMLHandler->appendString("synonym", m_mainWidget->wordEdit->text(), "id", m_id);
    KSDBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord((*count).name, m_XMLHandler->parse(), false, QString::number((*count).id));
  }

  for(QValueList<KSElement>::iterator count=saveAntonyms.begin();count!=saveAntonyms.end();count++)
  {
    m_XMLHandler=new KSXMLHandler(KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+QString::number((*count).id)+"';"));
    m_XMLHandler->appendString("antonym", m_mainWidget->wordEdit->text(), "id", m_id);
    KSDBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord((*count).name, m_XMLHandler->parse(), false, QString::number((*count).id));
  }

  delete m_XMLHandler;
  m_XMLHandler=0;

  m_modified=false;
}

void KSPhrase::slotOk()
{
  if(m_modified)
  {
    save();
  }
  emit okClicked();
  accept();
}

void KSPhrase::slotApply()
{
  if(m_modified)
  {
    save();
  }
  enableButtonApply(false);
  emit applyClicked();
}

void KSPhrase::setWord(QString text, QString id)
{
  text.remove(QRegExp("<\\?.+\\?>"));
  m_id=id;
  m_edit=true;

  m_XMLHandler=new KSXMLHandler(text);
  m_mainWidget->wordEdit->setText(m_XMLHandler->readString("word"));
  m_mainWidget->typeBox->setCurrentItem(m_XMLHandler->readString("type").toInt()-1);

  QValueList<KSExplanation> explanations=m_XMLHandler->readExplanation();
  for(QValueList<KSExplanation>::iterator count=explanations.begin();count!=explanations.end();count++)
  {
    new KListViewItem(m_mainWidget->explanationList, (*count).explanation, (*count).example);
  }

  QStringList synonym=m_XMLHandler->readStringList("synonym");
  for(QStringList::iterator count=synonym.begin();count!=synonym.end();count++)
  {
    for(QListViewItem *current=m_mainWidget->availableSynonymList->firstChild();current;current=current->nextSibling())
    {
      if(*count==current->text(0))
      {
        m_mainWidget->selectedSynonymList->insertItem(current);
        delete m_mainWidget->availableAntonymList->findItem(*count, 0);
      }
    }
  }

  QStringList antonym=m_XMLHandler->readStringList("antonym");
  for(QStringList::iterator count=antonym.begin();count!=antonym.end();count++)
  {
    for(QListViewItem *current=m_mainWidget->availableAntonymList->firstChild();current;current=current->nextSibling())
    {
      if(*count==current->text(0))
      {
        m_mainWidget->selectedAntonymList->insertItem(current);
        delete m_mainWidget->availableSynonymList->findItem(*count, 0);
      }
    }
  }

  delete m_mainWidget->availableSynonymList->findItem(m_mainWidget->wordEdit->text(), 0);
  delete m_mainWidget->availableAntonymList->findItem(m_mainWidget->wordEdit->text(), 0);
  delete m_XMLHandler;
  m_XMLHandler=0;

}

void KSPhrase::populatePartsOfSpeech()
{
  m_mainWidget->typeBox->insertStringList(KSData::instance()->getPartOfSpeech());
}

void KSPhrase::initialize()
{
  KIconLoader *icons=new KIconLoader();

  m_mainWidget=new KSPhraseWdt(this);
  m_mainWidget->spellButton->setIconSet(icons->loadIconSet("spellcheck", KIcon::Toolbar));
  m_mainWidget->explanationList->setItemsRenameable(true);
  m_mainWidget->explanationList->addColumn(i18n("Explanation"));
  m_mainWidget->explanationList->addColumn(i18n("Example"));
  m_mainWidget->explanationList->setRenameable(1);

  m_mainWidget->rightSynonymButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftSynonymButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

  m_mainWidget->rightAntonymButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftAntonymButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

  m_mainWidget->availableSynonymList->setFullWidth(true);
  m_mainWidget->availableSynonymList->addColumn("name");
  m_mainWidget->selectedSynonymList->setFullWidth(true);
  m_mainWidget->selectedSynonymList->addColumn("name");

  m_mainWidget->availableAntonymList->setFullWidth(true);
  m_mainWidget->availableAntonymList->addColumn("name");
  m_mainWidget->selectedAntonymList->setFullWidth(true);
  m_mainWidget->selectedAntonymList->addColumn("name");

  m_mainWidget->typeBox->insertStringList(KSData::instance()->getPartOfSpeech());
}

void KSPhrase::connectSlots()
{
  connect(m_mainWidget->addButton, SIGNAL(clicked()), this, SLOT(slotAddExplanation()));
  connect(m_mainWidget->removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveExplanation()));
  connect(m_mainWidget->spellButton, SIGNAL(clicked()), this, SLOT(slotBeginCheck()));
  connect(m_mainWidget->rightSynonymButton, SIGNAL(clicked()), this, SLOT(slotAddSynonym()));
  connect(m_mainWidget->leftSynonymButton, SIGNAL(clicked()), this, SLOT(slotRemoveSynonym()));
  connect(m_mainWidget->rightAntonymButton, SIGNAL(clicked()), this, SLOT(slotAddAntonym()));
  connect(m_mainWidget->leftAntonymButton, SIGNAL(clicked()), this, SLOT(slotRemoveAntonym()));
  connect(m_mainWidget->wordEdit, SIGNAL(textChanged(const QString&)), this, SLOT(slotModified()));
  connect(m_mainWidget->typeBox, SIGNAL(activated(int)), this, SLOT(slotModified()));
  connect(m_mainWidget->explanationList, SIGNAL(itemRenamed(QListViewItem*)), this, SLOT(slotModified()));
}

void KSPhrase::slotModified()
{
  if(m_create)
  {
    m_create=false;
  }
  else
  {
    m_modified=true;
    enableButtonApply(true);
  }
}

#include "ksphrase.moc"
