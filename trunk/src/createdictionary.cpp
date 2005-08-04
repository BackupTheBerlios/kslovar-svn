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
#include <qlabel.h>
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
#include <kaction.h>
#include <kshortcut.h>
#include <kactioncollection.h>

#include <kdebug.h>

CreateDictionary::CreateDictionary(QWidget *parent, QString caption, QString name, QString page)
  : KDialog(parent, caption)
{
  registerButtons();
  
  QVBoxLayout *all=new QVBoxLayout(this);
  
  registerToolbar();
  
  all->addWidget(m_toolbar);
  
  QVBoxLayout *nameLayout=new QVBoxLayout(this);
  QLabel *nameLabel=new QLabel(i18n("Name"), this);
  //nameLabel->setMaximumHeight(22);
  nameLayout->addWidget(nameLabel);
  m_nameEdit=new KLineEdit(this);
  nameLayout->addWidget(m_nameEdit);
  
  all->addLayout(nameLayout);
  
  QVBoxLayout *editLayout=new QVBoxLayout(this);
  QLabel *pageLabel=new QLabel(i18n("First page"), this);
  //pageLabel->setMaximumHeight(22);
  editLayout->addWidget(pageLabel);
  m_pageEdit=new KTextEdit(this);
  m_pageEdit->setAlignment(Qt::AlignJustify);
  editLayout->addWidget(m_pageEdit);
  
  all->addLayout(editLayout);
  
  if((!name.isEmpty()) && (!page.isEmpty()))
  {
    m_nameEdit->setText(name);
    m_pageEdit->setText(page);
    m_pageEdit->setTextFormat(PlainText);
  }

  connect(m_pageEdit, SIGNAL(textChanged()), this, SLOT(slotChanged()));
  connect(m_nameEdit, SIGNAL(textChanged(const QString &)), this, SLOT(slotChanged()));
}

void CreateDictionary::slotSaveAs()
{
  QString backup=m_path;
  m_pageEdit->setTextFormat(RichText);
  m_path=KFileDialog::getSaveFileName(QString::null, "*.db", this);
  if(m_path.isEmpty())
  {
    m_path=backup;
    return;
  }
  if(QFileInfo(m_path).exists())
  {
    int res = KMessageBox::warningContinueCancel(this, i18n("The file already exists. Overwrite?"), i18n("Create"));
    if (res==KMessageBox::Cancel)
    {
      return;
    }
    QFile::remove(m_path);
  }
  
  QString page="<h1>";
  page=page+m_nameEdit->text()+"</h1>"+m_pageEdit->text();
  
  DBHandler::Instance(m_path)->saveDictionary(page);
  m_save->setEnabled(false);
}

void CreateDictionary::slotUnder()
{
  if(m_pageEdit->underline())
  {
    m_pageEdit->setUnderline(false);
  }
  else
  {
    m_pageEdit->setUnderline(true);
  }
}

void CreateDictionary::slotItalic()
{
  if(m_pageEdit->italic())
  {
    m_pageEdit->setItalic(false);
  }
  else
  {
    m_pageEdit->setItalic(true);
  }
}

void CreateDictionary::slotBold()
{
  if(m_pageEdit->bold())
  {
    m_pageEdit->setBold(false);
  }
  else
  {
    m_pageEdit->setBold(true);
  }
}

void CreateDictionary::slotFontSize()
{
  KComboBox * selectedFontSize = m_toolbar->getCombo(3);
  QString size = selectedFontSize->currentText();
  
  m_pageEdit->setPointSize(size.toInt());
}

void CreateDictionary::slotSave()
{
  if(m_path.isEmpty())
  {
    slotSaveAs();
    return;
  }
  
  m_pageEdit->setTextFormat(RichText);
  QString page="<h1>";
  page=page+m_nameEdit->text()+"</h1>"+m_pageEdit->text();
  
  DBHandler::Instance(m_path)->saveDictionary(page, false);
  m_save->setEnabled(false);
}

void CreateDictionary::slotChanged()
{
  m_save->setEnabled(true);
}

void CreateDictionary::setPath(QString filename)
{
  m_path=filename;
}

void CreateDictionary::registerButtons()
{
  m_bold=new KAction(i18n("&Bold"), "text_bold", KShortcut(KKey("CTRL+b")), this, SLOT(slotBold()), this, "bold");
  m_italic=new KAction(i18n("&Italic"), "text_italic", KShortcut(KKey("CTRL+i")), this, SLOT(slotItalic()), this, "italic");
  m_under=new KAction(i18n("&Underline"), "text_under", KShortcut(KKey("CTRL+u")), this, SLOT(slotUnder()), this, "under");
  
  m_save=new KAction(i18n("&Save"), "filesave", KShortcut(KKey("CTRL+s")), this, SLOT(slotSave()), this, "save");
  m_saveAs=new KAction(i18n("Sa&ve As"), "filesaveas", KShortcut(KKey("CTRL+e")), this, SLOT(slotSaveAs()), this, "saveAs");
  m_close=new KAction(i18n("C&lose"), "fileclose", KShortcut(KKey("CTRL+l")), this, SLOT(close()), this, "close");
}

void CreateDictionary::registerToolbar()
{
  QStringList fontSize;
  fontSize << "8" << "10" << "12" << "14" << "16";
  
  m_toolbar = new KToolBar(this);
  m_save->plug(m_toolbar);
  m_saveAs->plug(m_toolbar);
  m_toolbar->insertSeparator();
  m_bold->plug(m_toolbar);
  m_italic->plug(m_toolbar);
  m_under->plug(m_toolbar);
  m_toolbar->insertCombo(fontSize, 3, true, SIGNAL(activated(int)), this, SLOT(slotFontSize()));
  m_toolbar->setCurrentComboItem(3, 2);
  m_toolbar->insertSeparator();
  m_save->setEnabled(false);
  
  m_close->plug(m_toolbar);
}

#include "createdictionary.moc"
