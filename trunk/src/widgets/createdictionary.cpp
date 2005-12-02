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
#include "createdictionary.h"

#include "../dbhandler.h"
#include "../objects/ksdata.h"

#include "../ui/createdictionarywdt.h"
#include "kslanguages.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <ktextedit.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kstandarddirs.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kfiledialog.h>

CreateDictionary::CreateDictionary(QWidget *parent, const char *caption, QString nameDictionary, QString text, bool edit)
  : KDialogBase(parent, "CreateDictionary", true, caption), m_name(nameDictionary), m_text(text), m_edit(edit)
{
  //KMessageBox::information(this, "This is curently under rewrite. It doesn't work!"); //For the time being...

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

  //Currently disabled, because of quick presentation...
  m_mainWidget->addLangButton->setEnabled(false);
  m_mainWidget->editLangButton->setEnabled(false);
  m_mainWidget->languageSelect->setEnabled(false);

  enableButtonApply(false);

  connect(m_mainWidget->boldButton, SIGNAL(clicked()), this, SLOT(slotBold()));
  connect(m_mainWidget->italicButton, SIGNAL(clicked()), this, SLOT(slotItalic()));
  connect(m_mainWidget->underlineButton, SIGNAL(clicked()), this, SLOT(slotUnderline()));
  connect(m_mainWidget->sizeInput, SIGNAL(valueChanged(int)), this, SLOT(slotSize(int)));
  connect(m_mainWidget->addLangButton, SIGNAL(clicked()), this, SLOT(slotAddLang()));
  connect(m_mainWidget->editLangButton, SIGNAL(clicked()), this, SLOT(slotEditLang()));

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
  QStringList text=DBHandler::instance(locateLocal("data", "kslovar/languages.ksl"))->processList("SELECT name FROM languages;");
  if(!text.isEmpty())
  {
    for(QStringList::iterator count=text.begin(); count!=text.end(); count++)
    {
      m_mainWidget->languageSelect->insertItem(*count);
    }
  }
}

void CreateDictionary::slotEditLang()
{
  if(m_mainWidget->languageSelect->currentItem() == 0)
  {
    return;
  }

  KSLanguages *widget=new KSLanguages(this, i18n("Edit language"), m_mainWidget->languageSelect->currentText(), locateLocal("data", "kslovar/languages.ksl"), m_mainWidget->languageSelect->currentItem());
  widget->show();
}

void CreateDictionary::slotAddLang()
{
  KSLanguages *widget=new KSLanguages(this, i18n("Add language"));
  widget->show();
}

void CreateDictionary::slotOk()
{
  if(!save())
  {
    return;
  }
  emit okClicked();
  accept();
}

void CreateDictionary::slotApply()
{
  if(!save())
  {
    return;
  }
  emit applyClicked();
}

bool CreateDictionary::save()
{
  m_mainWidget->mainEdit->setTextFormat(Qt::RichText);
  QString text="<h1>"+m_mainWidget->nameEdit->text()+"</h1>"+m_mainWidget->mainEdit->text();
  if(m_edit)
  {
    if(!DBHandler::instance(KSData::instance()->getDictionaryPath())->saveDictionary(text, false))
    {
      return false;
    }
  }
  else
  {
    QString path=KFileDialog::getSaveFileName("~", "*.ksd|KSlovar dictionary file", this);
    if(path.isEmpty())
    {
      return false;
    }
    QFile dictionary(path);
    if(dictionary.open(IO_ReadOnly))
    {
      if(KMessageBox::questionYesNo(this, i18n("Do you really want to overwrite?"), i18n("Overwrite?"), i18n("Overwrite"), i18n("Keep"))==4)
      {
        return save();
      }
      if(!dictionary.remove())
      {
        KMessageBox::error(this, i18n("File cannot be removed! Cannot save."));
        return false;
      }
    }
    if(!DBHandler::instance(path)->saveDictionary(text))
    {
      return false;
    }
    KSData::instance()->setDictionaryPath(path);
    m_edit=true;
  }
  return true;
}

CreateDictionary::~CreateDictionary()
{
}


#include "createdictionary.moc"
