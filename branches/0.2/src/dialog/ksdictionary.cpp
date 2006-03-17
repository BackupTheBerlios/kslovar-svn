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
#include "ksdictionary.h"

#include "kslanguage.h"

#include "../kslovar.h"

#include "../handler/ksdbhandler.h"

#include "../misc/ksdata.h"

#include "ui/ksdictionarywdt.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kcombobox.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <knuminput.h>

KSDictionary::KSDictionary(QWidget *parent, const char *caption, const QString &nameDictionary, const QString &text, bool edit)
  : KDialogBase(parent, "KSDictionary", true, caption), m_name(nameDictionary), m_text(text), m_edit(edit)
{
  KIconLoader *icons=new KIconLoader();
  m_mainWidget=new KSDictionaryWdt(this);

  if(m_name)
  {
    m_mainWidget->nameEdit->setText(m_name);
    m_mainWidget->mainEdit->setText(m_text);
  }

  populateLanguages();

  m_mainWidget->typeSelect->clear();
  m_mainWidget->typeSelect->insertItem(i18n("Default"));
  m_mainWidget->typeSelect->insertItem(i18n("Transitional"));
  m_mainWidget->typeSelect->setCurrentItem(KSData::instance()->getType());

  m_mainWidget->boldButton->setIconSet(icons->loadIconSet("text_bold", KIcon::Toolbar));
  m_mainWidget->italicButton->setIconSet(icons->loadIconSet("text_italic", KIcon::Toolbar));
  m_mainWidget->underlineButton->setIconSet(icons->loadIconSet("text_under", KIcon::Toolbar));

  enableButtonApply(false);

  connect(m_mainWidget->boldButton, SIGNAL(clicked()), this, SLOT(slotBold()));
  connect(m_mainWidget->italicButton, SIGNAL(clicked()), this, SLOT(slotItalic()));
  connect(m_mainWidget->underlineButton, SIGNAL(clicked()), this, SLOT(slotUnderline()));
  connect(m_mainWidget->sizeInput, SIGNAL(valueChanged(int)), this, SLOT(slotSize(int)));
  connect(m_mainWidget->addLangButton, SIGNAL(clicked()), this, SLOT(slotAddLang()));
  connect(m_mainWidget->editLangButton, SIGNAL(clicked()), this, SLOT(slotEditLang()));

  setMainWidget(m_mainWidget);
}

void KSDictionary::slotBold()
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

void KSDictionary::slotItalic()
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

void KSDictionary::slotUnderline()
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

void KSDictionary::slotSize(int newSize)
{
  m_mainWidget->mainEdit->setPointSize(newSize);
}

void KSDictionary::populateLanguages()
{
  m_mainWidget->languageSelect->clear();
  QStringList languages=KSData::instance()->getLanguagesNames();
  if(!languages.isEmpty())
  {
    for(QStringList::iterator count=languages.begin(); count!=languages.end(); count++)
    {
      m_mainWidget->languageSelect->insertItem(*count);
    }
  }
}

void KSDictionary::slotEditLang()
{
  KSLanguage *widget=new KSLanguage(this, i18n("Edit language"), m_mainWidget->languageSelect->currentText(), m_mainWidget->languageSelect->currentItem());
  widget->show();
}

void KSDictionary::slotAddLang()
{
  KSLanguage *widget=new KSLanguage(this, i18n("Add language"));
  widget->show();
}

void KSDictionary::slotOk()
{
  if(!save())
  {
    return;
  }
  emit okClicked();
  accept();
}

void KSDictionary::slotApply()
{
  if(!save())
  {
    KMessageBox::error(this, i18n("Cannot save dictionary!"));
    return;
  }
  emit applyClicked();
}

bool KSDictionary::save()
{
  if(m_mainWidget->nameEdit->text().isEmpty() || m_mainWidget->mainEdit->text().isEmpty())
  {
    KMessageBox::error(this, i18n("Fill in all the fields."));
    return false;
  }
  m_mainWidget->mainEdit->setTextFormat(Qt::RichText);
  QString text="<h1>"+m_mainWidget->nameEdit->text()+"</h1>"+m_mainWidget->mainEdit->text();
  QString id=KSData::instance()->getLanguageId(m_mainWidget->languageSelect->currentText());
  QString lang=id;

  QString type;
  if(m_mainWidget->typeSelect->currentText() == i18n("Transitional"))
  {
    type.setNum(1);
  }
  else
  {
    type.setNum(0);
  }

  if(m_edit)
  {
    if(!KSData::instance()->getDictionary()->saveDictionary(text, lang, type, false))
    {
      return false;
    }
    KSData::instance()->setType(type.toInt());
    KSData::instance()->setLanguage(lang.toInt());
  }
  else
  {
    QString path=KFileDialog::getSaveFileName("~/"+m_mainWidget->nameEdit->text(), "*.ksd|KSlovar dictionary file", this);
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
    KSDBHandler *temp = new KSDBHandler(path);
    if(!temp->saveDictionary(text, lang, type))
    {
      return false;
    }
    delete temp;
    m_edit=true;
    KSlovar::KSInstance()->openFile(path);
  }
  return true;
}

KSDictionary::~KSDictionary()
{
}


#include "ksdictionary.moc"
