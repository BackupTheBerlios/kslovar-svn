/***************************************************************************
 *   Copyright (C) 2005 by Gregor Kališnik   *
 *   gregor@podnapisi.net   *
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
#include "kslovar.h"
#include "dbhandler.h"

#include <kdialogbase.h>
#include <klocale.h>
#include <qlayout.h>
#include <qwidget.h>
#include <qgroupbox.h>
#include <klineedit.h>
#include <klistview.h>
#include <kpushbutton.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kspell.h>
#include <klistbox.h>
#include <qregexp.h>
#include <qlabel.h>
#include <ksconfig.h>
#include <kmessagebox.h>

#include <kdebug.h>


AddPhrase::AddPhrase(QWidget *parent, QString caption) : KDialogBase(parent, "AddPhrase", true, caption)
{
  KMessageBox::information(this, "This doesn't work. Really. You can't do with this nothing. It will produce only bigger blackness! But it's still cool to see what is going to be ;).", "Don't bother");
  KIconLoader *icons=new KIconLoader();
  
  m_mainWidget=new AddPhraseWdt(this);
  //m_spell=new KSpell(m_mainWidget->explanationList, i18n("Spell check"), this, SLOT(slotSpellCheck()));
  m_mainWidget->spellButton->setIconSet(icons->loadIconSet("spellcheck", KIcon::Toolbar));
  m_mainWidget->explanationList->setRenameable(1);
  
  m_mainWidget->availableLabel->setText(i18n("List of available words"));
  m_mainWidget->addedLabel->setText(i18n("List of selected words"));
  m_mainWidget->rightButton->setIconSet(icons->loadIconSet("forward", KIcon::Toolbar));
  m_mainWidget->leftButton->setIconSet(icons->loadIconSet("back", KIcon::Toolbar));
  populateAvailableList();
  
  setMainWidget(m_mainWidget);
  connect(m_mainWidget->addButton, SIGNAL(clicked()), this, SLOT(slotAddExplanation()));
  connect(m_mainWidget->removeButton, SIGNAL(clicked()), this, SLOT(slotRemoveExplanation()));
  connect(m_mainWidget->spellButton, SIGNAL(clicked()), this, SLOT(slotBeginCheck()));
  connect(m_mainWidget->rightButton, SIGNAL(clicked()), this, SLOT(slotAddWord()));
  connect(m_mainWidget->leftButton, SIGNAL(clicked()), this, SLOT(slotRemoveWord()));
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
  QStringList words=KSlovar::mainInstance()->getPhrases();
  for(QStringList::iterator it = words.begin(); it != words.end(); it++)
  {
    QString word = *it;
    m_mainWidget->availableList->insertItem(word.remove( QRegExp ("/.+$") ));
  }
}

void AddPhrase::slotAddWord()
{
  m_mainWidget->selectedList->insertItem(m_mainWidget->availableList->currentText());
  m_mainWidget->availableList->removeItem(m_mainWidget->availableList->currentItem());
}

void AddPhrase::slotRemoveWord()
{
  m_mainWidget->availableList->insertItem(m_mainWidget->selectedList->currentText());
  m_mainWidget->selectedList->removeItem(m_mainWidget->selectedList->currentItem());
}

void AddPhrase::slotBeginCheck()
{
  QListViewItem *current=m_mainWidget->explanationList->currentItem();
  if(!current)
  {
    return;
  }
  QString check;
  check=current->text(0)+" \n "+current->text(1);
  KSpellConfig *speller=KSlovar::spellInstance();
  KSpell::modalCheck(check, speller);
  QStringList checked=QStringList::split(" \n ", check);
  m_mainWidget->explanationList->currentItem()->setText(0, checked.first());
  m_mainWidget->explanationList->currentItem()->setText(1, checked.last());
}

#include "addphrase.moc"
