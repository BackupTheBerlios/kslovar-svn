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

#include "kslovar.h"

#include "handler/ksdbhandler.h"
#include "handler/ksxmlhandler.h"

#include "misc/widget/kslistview.h"
#include "misc/widget/kslistviewitem.h"
#include "misc/widget/kssearchline.h"

#include "misc/ksdata.h"

#include <qregexp.h>
#include <qdom.h>
#include <qtabwidget.h>

#include <klocale.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kspell.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kprogress.h>
#include <kdebug.h>
#include <klistview.h>


KSPhrase::KSPhrase(QWidget *parent, const QString &caption)
  : KDialogBase(parent, "KSPhrase", true, caption, Ok|Apply|Cancel), m_modified(false)
{
  m_create=true;
  initialize();
  enableButtonApply(false);

  setMainWidget(m_mainWidget);

  connectSlots();
}

void KSPhrase::slotAddExplanation()
{
  if(!KSData::instance()->getType())
  {
    new KListViewItem(m_mainWidget->explanationList, i18n("Type an explanation."), i18n("Type an example."));
  }
  else
  {
    new KListViewItem(m_mainWidget->explanationList, i18n("Type an explanation for the first language."), i18n("Type an example for the first language."), i18n("Type an explanation for the second language."), i18n("Type an example for the second language."));
  }
  slotModified();
}

void KSPhrase::slotRemoveExplanation()
{
  m_mainWidget->explanationList->takeItem(m_mainWidget->explanationList->currentItem());
  slotModified();
}

void KSPhrase::slotAddSynonym()
{
  //Remove a allready existing word in the delete list.
  for(QStringList::iterator count = m_deletedSynonyms.begin(); count != m_deletedSynonyms.end(); count++)
  {
    if(*count == m_mainWidget->availableSynonymList->currentItem()->text(0))
    {
      m_deletedSynonyms.remove(count);
      break;
    }
  }

  //Delete the word from the available list in the antonym section.
  delete m_mainWidget->availableAntonymList->findItem(m_mainWidget->availableSynonymList->currentItem()->text(0), 0);

  /*m_mainWidget->availableSynonymList->addFilter(static_cast <KSListViewItem*> (m_mainWidget->availableSynonymList->currentItem())->getId().toInt());
  m_mainWidget->availableAntonymList->addFilter(static_cast <KSListViewItem*> (m_mainWidget->availableSynonymList->currentItem())->getId().toInt());*/

  //Put the word to the selected list.
  m_mainWidget->selectedSynonymList->insertItem(m_mainWidget->availableSynonymList->currentItem());

  slotModified();
}

void KSPhrase::slotRemoveSynonym()
{
  //Add the word to the deleted list.
  m_deletedSynonyms << m_mainWidget->selectedSynonymList->currentItem()->text(0);
  //KSListViewItem *temp=static_cast<KSListViewItem*> (m_mainWidget->selectedSynonymList->currentItem());

  /*m_mainWidget->availableSynonymList->delFilter(static_cast <KSListViewItem*> (m_mainWidget->selectedSynonymList->currentItem())->getId().toInt());
  m_mainWidget->availableAntonymList->delFilter(static_cast <KSListViewItem*> (m_mainWidget->selectedSynonymList->currentItem())->getId().toInt());*/

  //Temporarily we save the reference to the word. And convert :)
  KSListViewItem *tempWord = static_cast<KSListViewItem*> (m_mainWidget->selectedSynonymList->currentItem());

  //Put the word back to the available lists.
  new KSListViewItem(m_mainWidget->availableAntonymList, tempWord->text(0), tempWord->getId(), tempWord->getSearch());
  m_mainWidget->availableSynonymList->insertItem(m_mainWidget->selectedSynonymList->currentItem());

  //delete m_mainWidget->selectedSynonymList->currentItem();
  slotModified();
}

void KSPhrase::slotAddAntonym()
{
  //Remove a allready existing word in the delete list.
  for(QStringList::iterator count=m_deletedAntonyms.begin();count!=m_deletedAntonyms.end();count++)
  {
    if(*count==m_mainWidget->availableAntonymList->currentItem()->text(0))
    {
      m_deletedAntonyms.remove(count);
      break;
    }
  }

  //Delete the word from the available list in the synonym section.
  delete m_mainWidget->availableSynonymList->findItem(m_mainWidget->availableAntonymList->currentItem()->text(0), 0);

  /*m_mainWidget->availableSynonymList->addFilter(static_cast <KSListViewItem*> (m_mainWidget->availableAntonymList->currentItem())->getId().toInt());
  m_mainWidget->availableAntonymList->addFilter(static_cast <KSListViewItem*> (m_mainWidget->availableAntonymList->currentItem())->getId().toInt());*/

  //Put the word to the selected list.
  m_mainWidget->selectedAntonymList->insertItem(m_mainWidget->availableAntonymList->currentItem());

  slotModified();
}

void KSPhrase::slotRemoveAntonym()
{
  //Add the word to the deleted list.
  m_deletedAntonyms << m_mainWidget->selectedAntonymList->currentItem()->text(0);
  //KSListViewItem *temp=static_cast<KSListViewItem*> (m_mainWidget->selectedAntonymList->currentItem());

  /*m_mainWidget->availableSynonymList->delFilter(static_cast <KSListViewItem*> (m_mainWidget->selectedAntonymList->currentItem())->getId().toInt());
  m_mainWidget->availableAntonymList->delFilter(static_cast <KSListViewItem*> (m_mainWidget->selectedAntonymList->currentItem())->getId().toInt());*/

  //Temporarily we save the reference to the word. And convert :)
  KSListViewItem *tempWord = static_cast<KSListViewItem*> (m_mainWidget->selectedAntonymList->currentItem());

  //Put the word back to the available lists.
  new KSListViewItem(m_mainWidget->availableSynonymList, tempWord->text(0), tempWord->getId(), tempWord->getSearch());
  m_mainWidget->availableAntonymList->insertItem(m_mainWidget->selectedAntonymList->currentItem());

  //delete m_mainWidget->selectedAntonymList->currentItem();
  slotModified();
}

void KSPhrase::slotAddFamily()
{
  //Remove a allready existing word in the delete list.
  for(QStringList::iterator count = m_deletedFamily.begin(); count != m_deletedFamily.end(); count++)
  {
    if(*count == m_mainWidget->availableFamilyList->currentItem()->text(0))
    {
      m_deletedFamily.remove(count);
      break;
    }
  }

  //m_mainWidget->availableFamilyList->addFilter(static_cast <KSListViewItem*> (m_mainWidget->availableFamilyList->currentItem())->getId().toInt());

  //Put the word to the selected list.
  m_mainWidget->selectedFamilyList->insertItem(m_mainWidget->availableFamilyList->currentItem());

  slotModified();
}

void KSPhrase::slotRemoveFamily()
{
  //Add the word to the deleted list.
  m_deletedFamily << m_mainWidget->selectedFamilyList->currentItem()->text(0);

  //Put the word back to the available list.
  m_mainWidget->availableFamilyList->insertItem(m_mainWidget->selectedFamilyList->currentItem());

  //delete m_mainWidget->selectedFamilyList->currentItem();

  slotModified();
}

void KSPhrase::slotBeginCheck()
{
  new KSpell(this, i18n("Spell Check"), this, SLOT(slotCheck(KSpell *)), 0, true, true);
}

void KSPhrase::slotCheck(KSpell *speller)
{
  connect(speller, SIGNAL(done(const QString&)), this, SLOT(slotEndCheck(const QString&)));
  QListViewItem *current = m_mainWidget->explanationList->currentItem();
  if(!current)
  {
    speller->cleanUp();
    return;
  }
  QString check;
  check = current->text(0)+" \n "+current->text(1);
  speller->check(check);
}

void KSPhrase::slotEndCheck(const QString& checked)
{
  QStringList check = QStringList::split(" \n ", checked);
  if((check.first() != m_mainWidget->explanationList->currentItem()->text(0))||(check.last()!=m_mainWidget->explanationList->currentItem()->text(1)))
  {
    m_modified=true;
    enableButtonApply(true);
  }
  m_mainWidget->explanationList->currentItem()->setText(0, check.first());
  if(check.count() == 2)
  {
    m_mainWidget->explanationList->currentItem()->setText(1, check.last());
  }
  KMessageBox::information(this, i18n("Spellcheck complete."));
}

void KSPhrase::save()
{
  QValueList<KSElement> saveSynonyms, saveAntonyms, saveFamily;

  m_XMLHandler=new KSXMLHandler();
  m_XMLHandler->addString("word", m_mainWidget->wordEdit->text());

  if(!KSData::instance()->getType()) //Two different types of dictionaries...
  {
    m_XMLHandler->addString("type", QString::number(KSData::instance()->getPartOfSpeechId(m_mainWidget->typeBox->currentText())));

    for(QListViewItem *count = m_mainWidget->explanationList->firstChild(); count; count = count->nextSibling()) //Add explanations and examples
    {
      m_XMLHandler->createNode("explanations");
      m_XMLHandler->addChildString("explanation", count->text(0));
      m_XMLHandler->addChildString("example", count->text(1));
    }

    for(QListViewItem *count = m_mainWidget->selectedSynonymList->firstChild(); count; count = count->nextSibling()) //Add synonyms
    {
      KSElement temp;
      KSListViewItem *current = static_cast<KSListViewItem*> (count);

      m_XMLHandler->addString("synonym", current->text(0), "id", current->getId());
      temp.name = current->text(0);
      temp.id = current->getId().toInt();
      saveSynonyms << temp;
    }

    for(QListViewItem *count = m_mainWidget->selectedAntonymList->firstChild(); count; count = count->nextSibling()) //Add antonyms
    {
      KSElement temp;
      KSListViewItem *current = static_cast<KSListViewItem*> (count);

      m_XMLHandler->addString("antonym", current->text(0), "id", current->getId());
      temp.name = current->text(0);
      temp.id = current->getId().toInt();
      saveAntonyms << temp;
    }

    for(QListViewItem *count = m_mainWidget->selectedFamilyList->firstChild(); count; count = count->nextSibling()) //Add word familys
    {
      KSElement temp;
      KSListViewItem *current = static_cast<KSListViewItem*> (count);

      m_XMLHandler->addString("word-family", current->text(0), "id", current->getId());
      temp.name = current->text(0);
      temp.id = current->getId().toInt();
      saveFamily << temp;
    }

  }
  else
  {
    for(QListViewItem *count = m_mainWidget->explanationList->firstChild(); count; count=count->nextSibling()) //Add examples and explanations
    {
      m_XMLHandler->createNode("explanations");
      m_XMLHandler->addChildString("explanation", count->text(0));
      m_XMLHandler->addChildString("example", count->text(1));
      m_XMLHandler->addChildString("explanation2", count->text(2));
      m_XMLHandler->addChildString("example2", count->text(3));
    }
  }

  QString xml = m_XMLHandler->parse();
  if(m_edit == true)
  {
    if(!KSData::instance()->getDictionary()->saveWord(m_mainWidget->wordEdit->text(), xml, false, m_id))
    {
      KMessageBox::error(this, i18n("Cannot edit phrase!"));
      return;
    }

    //We edit the existing entry in the list.
    KSData::instance()->getMainList()->findItemId(m_id.toInt())->setName(m_mainWidget->wordEdit->text());
  }
  else
  {
    if(!KSData::instance()->getDictionary()->saveWord(m_mainWidget->wordEdit->text(), xml, true, 0L))
    {
      KMessageBox::error(this, i18n("Cannot add new phrase!"));
      return;
    }
    m_id = QString::number(KSData::instance()->getDictionary()->getId());

    //We add the new item to the list.
    new KSListViewItem(KSData::instance()->getMainList(), m_mainWidget->wordEdit->text(), m_id, KSDBHandler::convertString(m_mainWidget->wordEdit->text()));
    //KSlovar::KSInstance()->openFile(KSData::instance()->getDictionaryPath());
  }

  if(!KSData::instance()->getType()) //Two different types of dictionary...
  {
    for(QValueList<KSElement>::iterator count = saveSynonyms.begin(); count != saveSynonyms.end(); count++) //Adding this word to its new synonyms
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number((*count).id)+"';")["text"]);
      m_XMLHandler->appendString("synonym", m_mainWidget->wordEdit->text(), "id", m_id);
      KSData::instance()->getDictionary()->saveWord((*count).name, m_XMLHandler->parse(), false, QString::number((*count).id));
    }

    for(QValueList<KSElement>::iterator count = saveAntonyms.begin(); count!=saveAntonyms.end(); count++) //Adding this word to its new antonyms
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number((*count).id)+"';")["text"]);
      m_XMLHandler->appendString("antonym", m_mainWidget->wordEdit->text(), "id", m_id);
      KSData::instance()->getDictionary()->saveWord((*count).name, m_XMLHandler->parse(), false, QString::number((*count).id));
    }

    for(QStringList::iterator count = m_deletedSynonyms.begin(); count != m_deletedSynonyms.end(); count++) //Remove this word from its former synonyms
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number(KSData::instance()->getDictionary()->getId(*count))+"';")["text"]);
      m_XMLHandler->removeString("synonym", m_mainWidget->wordEdit->text());
      KSData::instance()->getDictionary()->saveWord(*count, m_XMLHandler->parse(), false, QString::number(KSData::instance()->getDictionary()->getId(*count)));
    }

    for(QStringList::iterator count = m_deletedAntonyms.begin(); count != m_deletedAntonyms.end(); count++) //Remove this word from its former antonyms
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number(KSData::instance()->getDictionary()->getId(*count))+"';")["text"]);
      m_XMLHandler->removeString("antonym", m_mainWidget->wordEdit->text());
      KSData::instance()->getDictionary()->saveWord(*count, m_XMLHandler->parse(), false, QString::number(KSData::instance()->getDictionary()->getId(*count)));
    }

    for(QValueList<KSElement>::iterator count = saveFamily.begin(); count != saveFamily.end(); count++) //Adding this word to its new word families
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number((*count).id)+"';")["text"]);
      m_XMLHandler->appendString("word-family", m_mainWidget->wordEdit->text(), "id", m_id);
      KSData::instance()->getDictionary()->saveWord((*count).name, m_XMLHandler->parse(), false, QString::number((*count).id));
    }

    for(QStringList::iterator count = m_deletedFamily.begin(); count != m_deletedFamily.end(); count++) //Remove this word from its former word families
    {
      m_XMLHandler = new KSXMLHandler(KSData::instance()->getDictionary()->processString("SELECT text FROM phrases WHERE id_phrase='"+QString::number(KSData::instance()->getDictionary()->getId(*count))+"';")["text"]);
      m_XMLHandler->removeString("word-family", m_mainWidget->wordEdit->text());
      KSData::instance()->getDictionary()->saveWord(*count, m_XMLHandler->parse(), false, QString::number(KSData::instance()->getDictionary()->getId(*count)));
    }
  }

  KSlovar::KSInstance()->refresh();

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

void KSPhrase::setWord(QString text, const QString &id)
{
  //We parse the text.
  text.remove(QRegExp("<\\?.+\\?>"));
  m_id = id;
  m_edit = true;

  //The XML handler is born!
  m_XMLHandler = new KSXMLHandler(text);
  m_mainWidget->wordEdit->setText(m_XMLHandler->readString("word"));

  if(!KSData::instance()->getType())
  {
    //Get the part of speech.
    m_mainWidget->typeBox->setCurrentItem(KSData::instance()->getPartOfSpeechName(m_XMLHandler->readString("type").toInt()));

    //Read the explaantions-examples list.
    QValueList<KSExplanation> explanations = m_XMLHandler->readExplanation();
    for(QValueList<KSExplanation>::iterator count = explanations.begin(); count != explanations.end(); count++)
    {
      new KListViewItem(m_mainWidget->explanationList, (*count).explanation, (*count).example);
    }

    //Delete from available list to selected list - Synonyms
    QMap<QString,QString> synonym = m_XMLHandler->readQMapList("synonym");
    for(QMap<QString,QString>::iterator count = synonym.begin(); count != synonym.end(); count++)
    {
      new KSListViewItem(m_mainWidget->selectedSynonymList, count.data(), count.key());
      /*m_mainWidget->availableAntonymList->addFilter(count.key().toInt());
      m_mainWidget->availableSynonymList->addFilter(count.key().toInt());*/
      delete m_mainWidget->availableAntonymList->findItem(count.data(), 0);
      delete m_mainWidget->availableSynonymList->findItem(count.data(), 0);
    }

    //Delete from available list to selected list - Antonyms
    QMap<QString,QString> antonym = m_XMLHandler->readQMapList("antonym");
    for(QMap<QString,QString>::iterator count = antonym.begin(); count != antonym.end(); count++)
    {
      new KSListViewItem(m_mainWidget->selectedAntonymList, count.data(), count.key());
      delete m_mainWidget->availableAntonymList->findItem(count.data(), 0);
      delete m_mainWidget->availableSynonymList->findItem(count.data(), 0);
    }

    //Delete from available list to selected list - Family
    QMap<QString,QString> family = m_XMLHandler->readQMapList("word-family");
    for(QMap<QString,QString>::iterator count = family.begin(); count != family.end(); count++)
    {
      new KSListViewItem(m_mainWidget->selectedFamilyList, count.data(), count.key());
      delete m_mainWidget->availableFamilyList->findItem(count.data(), 0);
    }

    //Delete the opened word from the available lists.
    delete m_mainWidget->availableAntonymList->findItem(m_XMLHandler->readString("word"), 0);
    delete m_mainWidget->availableSynonymList->findItem(m_XMLHandler->readString("word"), 0);
    delete m_mainWidget->availableFamilyList->findItem(m_XMLHandler->readString("word"), 0);
  }
  else
  {
    //Read the explaantions-examples list.
    QValueList<KSExplanation> explanations=m_XMLHandler->readExplanation(true);
    for(QValueList<KSExplanation>::iterator count=explanations.begin();count!=explanations.end();count++)
    {
      new KListViewItem(m_mainWidget->explanationList, (*count).explanation, (*count).example, (*count).explanation2, (*count).example2);
    }
  }

  //We don't need XML handler anymore. So we murder him ;)
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
  m_mainWidget->explanationList->setEmptyText(i18n("Click the Add button to start adding explanations and examples"));

  m_mainWidget->rightSynonymButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftSynonymButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

  m_mainWidget->rightAntonymButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftAntonymButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

  m_mainWidget->rightFamilyButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftFamilyButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

  m_mainWidget->availableSynonymList->setFullWidth(true);
  m_mainWidget->availableSynonymList->addColumn("name");
  m_mainWidget->selectedSynonymList->setFullWidth(true);
  m_mainWidget->selectedSynonymList->addColumn("name");

  m_mainWidget->availableAntonymList->setFullWidth(true);
  m_mainWidget->availableAntonymList->addColumn("name");
  m_mainWidget->selectedAntonymList->setFullWidth(true);
  m_mainWidget->selectedAntonymList->addColumn("name");

  m_mainWidget->availableFamilyList->setFullWidth(true);
  m_mainWidget->availableFamilyList->addColumn("name");
  m_mainWidget->selectedFamilyList->setFullWidth(true);
  m_mainWidget->selectedFamilyList->addColumn("name");

  m_mainWidget->typeBox->insertStringList(KSData::instance()->getPartOfSpeech());

  if(KSData::instance()->getType())
  {
    //Change the explanation list
    m_mainWidget->explanationList->addColumn(i18n("Explanation one"));
    m_mainWidget->explanationList->addColumn(i18n("Example one"));
    m_mainWidget->explanationList->setRenameable(1);
    m_mainWidget->explanationList->addColumn(i18n("Explanation two"));
    m_mainWidget->explanationList->setRenameable(2);
    m_mainWidget->explanationList->addColumn(i18n("Example two"));
    m_mainWidget->explanationList->setRenameable(3);

    m_mainWidget->tabWidget->setTabEnabled(m_mainWidget->synonym, false);
    m_mainWidget->tabWidget->setTabEnabled(m_mainWidget->antonym, false);
    m_mainWidget->tabWidget->setTabEnabled(m_mainWidget->wordFamily, false);

    //Disable parts of speech
    m_mainWidget->typeBox->setShown(false);
  }
  else
  {
    m_mainWidget->explanationList->addColumn(i18n("Explanation"));
    m_mainWidget->explanationList->addColumn(i18n("Example"));
    m_mainWidget->explanationList->setRenameable(1);

    //Connect search lines
    m_mainWidget->synonymSearch->setList(m_mainWidget->availableSynonymList);
    m_mainWidget->antonymSearch->setList(m_mainWidget->availableAntonymList);
    m_mainWidget->familySearch->setList(m_mainWidget->availableFamilyList);

    m_mainWidget->availableSynonymList->setEmptyText(i18n("There are no words."));
    m_mainWidget->availableAntonymList->setEmptyText(i18n("There are no words."));
    m_mainWidget->availableFamilyList->setEmptyText(i18n("There are no words."));

    m_mainWidget->availableAntonymList->setSorting(-1);
    m_mainWidget->availableSynonymList->setSorting(-1);
    m_mainWidget->availableFamilyList->setSorting(-1);

    //Load the words
    populateAvailableWords();

    /*new KSListViewItem(m_mainWidget->availableSynonymList, i18n("Begin search by typing a"));
    new KSListViewItem(m_mainWidget->availableSynonymList, i18n("word into the search bar."));

    new KSListViewItem(m_mainWidget->availableAntonymList, i18n("Begin search by typing a"));
    new KSListViewItem(m_mainWidget->availableAntonymList, i18n("word into the search bar."));

    new KSListViewItem(m_mainWidget->availableFamilyList, i18n("Begin search by typing a"));
    new KSListViewItem(m_mainWidget->availableFamilyList, i18n("word into the search bar."));

    m_mainWidget->availableSynonymList->setDisabled(true);
    m_mainWidget->availableAntonymList->setDisabled(true);
    m_mainWidget->availableFamilyList->setDisabled(true);*/
  }
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
  connect(m_mainWidget->rightFamilyButton, SIGNAL(clicked()), this, SLOT(slotAddFamily()));
  connect(m_mainWidget->leftFamilyButton, SIGNAL(clicked()), this, SLOT(slotRemoveFamily()));
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

void KSPhrase::populateAvailableWords()
{
  /*KProgressDialog *progressDialog = new KProgressDialog(0, "loading", i18n("Loading phrases"), i18n("Please wait until all the words are processed."));
  KProgress *progress = progressDialog->progressBar();*/
  //Load the main list.
  KSListView *mainList = KSData::instance()->getMainList();
  //progress->setTotalSteps(mainList->childCount());
  //int processed = 0;
  //progressDialog->show();
  //Add the words
  for(QListViewItem *count = mainList->firstChild(); count; count = count->nextSibling())
  {
    //processed++;
    //progress->setProgress(processed);
    KSListViewItem *current = static_cast<KSListViewItem*> (count);
    new KSListViewItem(m_mainWidget->availableSynonymList, current->text(0), current->getId(), current->getSearch());
    new KSListViewItem(m_mainWidget->availableAntonymList, current->text(0), current->getId(), current->getSearch());
    new KSListViewItem(m_mainWidget->availableFamilyList, current->text(0), current->getId(), current->getSearch());
  }
}

#include "ksphrase.moc"
