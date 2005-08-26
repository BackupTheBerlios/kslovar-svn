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
#include "createdictionary.h"

#include "dbhandler.h"

#include "ui/createdictionarywdt.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kstandarddirs.h>
#include <kcombobox.h>

#include <kmessagebox.h>

CreateDictionary::CreateDictionary(QWidget *parent, const char *caption, QString nameDictionary, QString text)
  : KDialogBase(parent, "CreateDictionary", true, caption), m_name(nameDictionary), m_text(text)
{
  KMessageBox::information(this, "This is curently under rewrite. It doesn't work!"); //For the time being...

  KIconLoader *icons=new KIconLoader();
  m_mainWidget=new CreateDictionaryWdt(this);

  if(m_name)
  {
    m_mainWidget->nameEdit->setText(m_name);
    m_mainWidget->mainEdit->setText(m_text);
  }

  populateLanguages();

  m_mainWidget->boldButton->setIconSet(icons->loadIconSet("text_bold", KIcon::Toolbar));
  m_mainWidget->italicButton->setIconSet(icons->loadIconSet("text_italic", KIcon::Toolbar));
  m_mainWidget->underlineButton->setIconSet(icons->loadIconSet("text_under", KIcon::Toolbar));

  enableButtonApply(false);

  connect(m_mainWidget->boldButton, SIGNAL(clicked()), this, SLOT(slotBold()));
  connect(m_mainWidget->italicButton, SIGNAL(clicked()), this, SLOT(slotItalic()));
  connect(m_mainWidget->underlineButton, SIGNAL(clicked()), this, SLOT(slotUnderline()));
  connect(m_mainWidget->sizeInput, SIGNAL(valueChanged(int)), this, SLOT(slotSize(int)));

  setMainWidget(m_mainWidget);
}

void CreateDictionary::slotBold()
{
  if(m_mainWidget->mainEdit->bold())
  {
    m_mainWidget->mainEdit->setBold(false);
  }
  else
  {
    m_mainWidget->mainEdit->setBold(true);
  }
}

void CreateDictionary::slotItalic()
{
  if(m_mainWidget->mainEdit->italic())
  {
    m_mainWidget->mainEdit->setItalic(false);
  }
  else
  {
    m_mainWidget->mainEdit->setItalic(true);
  }
}

void CreateDictionary::slotUnderline()
{
  if(m_mainWidget->mainEdit->underline())
  {
    m_mainWidget->mainEdit->setUnderline(false);
  }
  else
  {
    m_mainWidget->mainEdit->setUnderline(true);
  }
}

void CreateDictionary::slotSize(int newSize)
{
  m_mainWidget->mainEdit->setPointSize(newSize);
}

void CreateDictionary::populateLanguages()
{
  QStringList text=DBHandler::Instance(locateLocal("data", "kslovar/languages.ksl"))->ProcessList("SELECT name FROM languages;");
  if(!text.isEmpty())
  {
    for(QStringList::iterator count=text.begin(); count!=text.end(); count++)
    {
      m_mainWidget->languageSelect->insertItem(*count);
    }
  }
}

CreateDictionary::~CreateDictionary()
{
}


#include "createdictionary.moc"
