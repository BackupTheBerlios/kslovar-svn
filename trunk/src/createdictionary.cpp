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

#include <kdialog.h>
#include <klocale.h>
#include <qlayout.h>
#include <kactivelabel.h>
#include <ktoolbar.h>
#include <klineedit.h>
#include <ktextedit.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <kcombobox.h>

CreateDictionary::CreateDictionary()
  : KDialog(0, "CreateDictionary")
{
  setCaption(i18n("Create dictionary"));
  
  QStringList fontSize;
  fontSize << "8" << "10" << "12" << "14" << "16";
  
  QVBoxLayout *all=new QVBoxLayout(this);
  
  toolbar = new KToolBar(this);
  toolbar->insertButton(BarIcon("text_bold"), 0,
                        SIGNAL(clicked(int)),this,SLOT(slotBold()),
                        TRUE, i18n("Go to previous phrase"));
  toolbar->insertButton(BarIcon("text_italic"), 1,
                        SIGNAL(clicked(int)),this,SLOT(slotItalic()),
                        TRUE, i18n("Go to next phrase"));
  toolbar->insertButton(BarIcon("text_under"), 2,
                        SIGNAL(clicked(int)),this,SLOT(slotUnder()),
                        TRUE, i18n("Go to first page"));
  toolbar->insertSeparator();
  toolbar->insertCombo(fontSize, 3, true, SIGNAL(activated(int)), this, SLOT(slotFontSize()));
  toolbar->setCurrentComboItem(3, 2);
  
  all->addWidget(toolbar);
  
  QVBoxLayout *name=new QVBoxLayout(this);
  KActiveLabel *nameLabel=new KActiveLabel(i18n("Name"), this);
  nameLabel->setMaximumHeight(22);
  name->addWidget(nameLabel);
  nameEdit=new KLineEdit(this);
  name->addWidget(nameEdit);
  
  all->addLayout(name);
  
  QVBoxLayout *edit=new QVBoxLayout(this);
  KActiveLabel *pageLabel=new KActiveLabel(i18n("First page"), this);
  pageLabel->setMaximumHeight(22);
  edit->addWidget(pageLabel);
  pageEdit=new KTextEdit(this);
  pageEdit->setAlignment(Qt::AlignJustify);
  edit->addWidget(pageEdit);
  
  all->addLayout(edit);
  
  QHBoxLayout *buttons=new QHBoxLayout(this);
  KPushButton *createButton=new KPushButton(i18n("Create"), this);
  buttons->addWidget(createButton);
  buttons->addStretch();
  KPushButton *cancelButton=new KPushButton(i18n("Cancel"), this);
  buttons->addWidget(cancelButton);
  
  all->addLayout(buttons);
  
  connect(createButton, SIGNAL(clicked()), this, SLOT(slotCreate()));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

void CreateDictionary::slotCreate()
{
  pageEdit->setTextFormat(RichText);
  QString path=KFileDialog::getSaveFileName(QString::null, "*.db", this);
  if(path.isEmpty())
  {
    return;
  }
  if(QFileInfo(path).exists())
  {
    int res = KMessageBox::warningContinueCancel(this, i18n("The file already exists. Overwrite?"), i18n("Create"));
    if (res==KMessageBox::Cancel)
    {
      return;
    }
    QFile::remove(path);
  }
  dictionaryDB=new DBHandler(path);
  
  QString page="<h1>";
  page=page+nameEdit->text()+"</h1>"+pageEdit->text();
  
  dictionaryDB->createDictionary(page);
  KMessageBox::information(this, i18n("Dictionary was created successfully"), i18n("Information"));
  close();
}

void CreateDictionary::slotUnder()
{
  if(pageEdit->underline())
  {
    pageEdit->setUnderline(false);
  }
  else
  {
    pageEdit->setUnderline(true);
  }
}

void CreateDictionary::slotItalic()
{
  if(pageEdit->italic())
  {
    pageEdit->setItalic(false);
  }
  else
  {
    pageEdit->setItalic(true);
  }
}

void CreateDictionary::slotBold()
{
  if(pageEdit->bold())
  {
    pageEdit->setBold(false);
  }
  else
  {
    pageEdit->setBold(true);
  }
}

void CreateDictionary::slotFontSize()
{
  KComboBox * selectedFontSize = toolbar->getCombo(3);
  QString size = selectedFontSize->currentText();
  
  pageEdit->setPointSize(size.toInt());
}

#include "createdictionary.moc"
