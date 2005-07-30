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

#include <ktoolbar.h>
#include <klocale.h>
#include <kiconloader.h>
#include <ktextedit.h>
#include <kcombobox.h>
#include <kmessagebox.h>

#include <kdebug.h>

CreateDictionary::CreateDictionary(QWidget *parent, const char *name)
    :KCreateDictionary(parent, name)
{
  QStringList fonts;
  fonts << "8" << "10" << "12" << "14" << "16";
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
  toolbar->insertCombo(fonts, 3, true, SIGNAL(activated(int)), this, SLOT(slotFont()));
  addToolBar(toolbar);
  toolbar->setCurrentComboItem(3, 2);
  //pageEdit->setCheckSpellingEnabled(true);
  pageEdit->setAlignment(Qt::AlignJustify);
}

void CreateDictionary::slotCreate()
{
  pageEdit->setTextFormat(RichText);
  
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

void CreateDictionary::slotFont()
{
  KComboBox * selectedFont = toolbar->getCombo(3);
  QString size = selectedFont->currentText();
  
  pageEdit->setPointSize(size.toInt());
}


#include "createdictionary.moc"
