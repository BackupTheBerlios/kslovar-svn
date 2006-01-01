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

#include "addphrase.h"

#include "ui/addphrasewdt.h"

#include "../kslovar.h"

#include "../handler/dbhandler.h"

#include "../misc/widget/kslistview.h"
#include "../misc/widget/kslistviewitem.h"

#include "../misc/ksdata.h"

#include <qregexp.h>

#include <klocale.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kspell.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kdebug.h>


AddPhrase::AddPhrase(QWidget *parent, QString caption)
  : KDialogBase(parent, "AddPhrase", true, caption, Ok|Cancel)
{
  initialize();
  populateAvailableList();
  enableButtonApply(false);

  setMainWidget(m_mainWidget);

  connectSlots();
}

void AddPhrase::slotAddExplanation()
{
  new KListViewItem(m_mainWidget->explanationList, i18n("Type an explanation."), i18n("Type an example."));
}

void AddPhrase::slotRemoveExplanation()
{
  m_mainWidget->explanationList->takeItem(m_mainWidget->explanationList->currentItem());
}

void AddPhrase::populateAvailableList()
{
  //m_words=Instances::mainInstance()->getPhrases();
  QValueList<KSPhrase> temp=KSData::instance()->getPhrases();
  /*for(QStringList::iterator it = m_words.begin(); it != m_words.end(); it++)
  {
    QString word = *it;
    QString search = word;
    QString id = search;
    new KSListViewItem(m_mainWidget->availableList, word.remove(QRegExp("/.+$")), id.remove(QRegExp("[^/\\d]+")).remove(QRegExp("^\\d+")).remove(QRegExp("\\d+$")).remove(QRegExp("/+")), search.remove(QRegExp("^.+/")));
}*/
  for(QValueList<KSPhrase>::iterator count=temp.begin();count!=temp.end();count++)
  {
    new KSListViewItem(m_mainWidget->availableList, (*count).name, QString::number((*count).id), (*count).search);
  }
}

void AddPhrase::slotAddWord()
{
  m_mainWidget->selectedList->insertItem(m_mainWidget->availableList->currentItem());
}

void AddPhrase::slotRemoveWord()
{
  m_mainWidget->availableList->insertItem(m_mainWidget->selectedList->currentItem());
}

void AddPhrase::slotBeginCheck()
{
  new KSpell(this, i18n("Spell Check"), this, SLOT(slotCheck(KSpell *)), 0, true, true);
}

void AddPhrase::slotCheck(KSpell *speller)
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

void AddPhrase::slotEndCheck(const QString& checked)
{
  QStringList check=QStringList::split(" \n ", checked);
  m_mainWidget->explanationList->currentItem()->setText(0, check.first());
  if(check.count()==2)
  {
    m_mainWidget->explanationList->currentItem()->setText(1, check.last());
  }
}

void AddPhrase::slotOk()
{
  QString explanations;
  for(QListViewItem *count=m_mainWidget->explanationList->firstChild();count;count=count->nextSibling())
  {
    explanations+="<explanations><explanation>"+count->text(0)+"</explanation><example>"+count->text(1)+"</example></explanations>";
  }
  QString seealso;

  for(QListViewItem *current=m_mainWidget->selectedList->firstChild();current;current=current->nextSibling())
  {
    KSListViewItem *item=static_cast<KSListViewItem*> (current);
    seealso+="<other><seealso id='"+item->getId()+"'>"+item->text(0)+"</seealso></other>";
  }

  QString text;
  text="<?xml version='1.0' encoding='UTF-8'?><phrase><word>"+m_mainWidget->wordEdit->text()+"</word><type>"+QString::number(KSData::instance()->getPartOfSpeechId(m_mainWidget->typeBox->currentText()))+"</type>"+explanations+seealso+"</phrase>";
  if(m_edit==true)
  {
    if(!DBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord(m_mainWidget->wordEdit->text(), text, false, m_id))
    {
      KMessageBox::error(this, i18n("Cannot edit phrase!"));
    }
    //Instances::mainInstance()->refresh();
    KSlovar::KSInstance()->refresh();
  }
  else
  {
    if(!DBHandler::instance(KSData::instance()->getDictionaryPath())->saveWord(m_mainWidget->wordEdit->text(), text, true, 0L))
    {
      KMessageBox::error(this, i18n("Cannot add new phrase!"));
    }
    //Instances::mainInstance()->openFile(KSData::instance()->getDictionaryPath());
    KSlovar::KSInstance()->openFile(KSData::instance()->getDictionaryPath());
  }

  emit okClicked();
  accept();
}

/*void AddPhrase::setPath(QString filename)
{
  m_path=filename;
}*/

void AddPhrase::setWord(QString text, QString id)
{
  QString word;


  text.remove(QRegExp("<\\?.+\\?>"));
  //m_text=text;
  m_id=id;
  m_edit=true;
  //populateAddPhraseDialog();

  QDomDocument phrase;
  phrase.setContent(text);
  QDomNode node=phrase.firstChild();
  for(node=node.firstChild();!node.isNull();node=node.nextSibling())
  {
    QString name=node.nodeName();
    if(name=="word")
    {
      m_mainWidget->wordEdit->setText(node.toElement().text());
      continue;
    }
    if(name=="type")
    {
      //KSData::instance()->getPartOfSpeechId(node.toElement().text());
      m_mainWidget->typeBox->setCurrentItem(node.toElement().text().toInt()-1);
      continue;
    }
    if(name=="explanations")
    {
      new KListViewItem(m_mainWidget->explanationList, node.firstChild().toElement().text(), node.lastChild().toElement().text());
      continue;
    }
    if(name=="other")
    {
      for(QDomNode node2=node.firstChild();!node2.isNull();node2=node2.nextSibling())
      {
        QString name2=node2.nodeName();
        if(name2=="seealso")
        {
          for(QListViewItem *count=m_mainWidget->availableList->firstChild();count;count=count->nextSibling())
          {
            if(node2.toElement().text()==count->text(0))
            {
              m_mainWidget->selectedList->insertItem(count);
            }
          }
          continue;
        }
      }
      continue;
    }
  }
  delete m_mainWidget->availableList->findItem(m_mainWidget->wordEdit->text(), 0);
}

void AddPhrase::populateAddPhraseDialog()
{
  /*QString word=m_text;
  word.remove(QRegExp("</h1>.+")).remove("<h1>");
  QString type=word;
  word.remove(QRegExp(",.+"));
  type.remove(word+", ");
  QString explanations=m_text;
  explanations.remove(QRegExp(".+<p>\\B")).remove(QRegExp("(</p>.+)|(</p>)")). remove("<li>");
  QString seealsos=m_text;
  seealsos.remove(QRegExp(".+See also:\\s+")).remove("</p>").remove(QRegExp("<a href=http://\\d+>"));
  QStringList explanation=QStringList::split("</li>", explanations);
  QStringList seealso=QStringList::split("</a> ", seealsos);

  m_mainWidget->wordEdit->setText(word);
  m_mainWidget->typeEdit->setText(type);
  for(QStringList::iterator count=explanation.begin();count!=explanation.end();count++)
  {
    QString temp=*count;
    temp.remove(QRegExp("<i>.+</i>"));
    QString temp2=*count;
    temp2.remove(temp).remove(QRegExp("<i>|</i>"));
    new KListViewItem(m_mainWidget->explanationList, temp, temp2);
  }
  for(QStringList::iterator count=seealso.begin();count!=seealso.end();count++)
  {
    for(QListViewItem *current=m_mainWidget->availableList->firstChild();current;current=current->nextSibling())
    {
      if(*count==current->text(0))
      {
        m_mainWidget->selectedList->insertItem(current);
      }
    }
  }
  delete m_mainWidget->availableList->findItem(word, 0);*/

}

void AddPhrase::populatePartsOfSpeech()
{
  m_mainWidget->typeBox->insertStringList(KSData::instance()->getPartOfSpeech());
}

void AddPhrase::initialize()
{
  KIconLoader *icons=new KIconLoader();

  m_mainWidget=new AddPhraseWdt(this);
  m_mainWidget->spellButton->setIconSet(icons->loadIconSet("spellcheck", KIcon::Toolbar));
  m_mainWidget->explanationList->setItemsRenameable(true);
  m_mainWidget->explanationList->addColumn(i18n("Explanation"));
  m_mainWidget->explanationList->addColumn(i18n("Example"));
  m_mainWidget->explanationList->setRenameable(1);

//  m_mainWidget->availableLabel->setText(i18n("List of available words"));
//  m_mainWidget->addedLabel->setText(i18n("List of selected words"));
  m_mainWidget->rightButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));

//  m_mainWidget->availableList->header()->hide();
  m_mainWidget->availableList->setColumnWidth(0, 193);
//  m_mainWidget->selectedList->header()->hide();
  m_mainWidget->selectedList->setColumnWidth(0, 193);

  m_mainWidget->availableList->setFullWidth(true);
  m_mainWidget->availableList->addColumn("name");
  m_mainWidget->selectedList->setFullWidth(true);
  m_mainWidget->selectedList->addColumn("name");

  m_mainWidget->typeBox->insertStringList(KSData::instance()->getPartOfSpeech());
}

void AddPhrase::connectSlots()
{
  connect(m_mainWidget->addButton, SIGNAL(clicked()), this, SLOT(slotAddExplanation()));
  connect(m_mainWidget->removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveExplanation()));
  connect(m_mainWidget->spellButton, SIGNAL(clicked()), this, SLOT(slotBeginCheck()));
  connect(m_mainWidget->rightButton, SIGNAL(clicked()), this, SLOT(slotAddWord()));
  connect(m_mainWidget->leftButton, SIGNAL(clicked()), this, SLOT(slotRemoveWord()));
}

#include "addphrase.moc"