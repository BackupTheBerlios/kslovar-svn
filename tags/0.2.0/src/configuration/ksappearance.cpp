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
#include "ksappearance.h"

#include "configuration.h"
#include "ksconfigdialog.h"

#include "../misc/widget/kslistview.h"
#include "../misc/widget/kslistviewitem.h"

#include "../misc/ksdata.h"

#include "../handler/ksxslhandler.h"

#include <qregexp.h>
#include <qlayout.h>
#include <qfileinfo.h>

#include <klistbox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kparts/browserextension.h>
#include <kinputdialog.h>
#include <kio/job.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <klocale.h>
#include <krun.h>

KSAppearance::KSAppearance(QWidget *parent, const char *name)
  :KSAppearanceWdt(parent, name)
{
  styleList->addColumn("name");
  styleList->setFullWidth(true);
  populateStyleList();

  m_exampleDefault = "<?xml version='1.0' encoding='UTF-8'?><phrase><word>Example</word><type>noun</type><explanations><explanation>An example style for the default dictionary.</explanation><example>You could use this style!</example></explanations><explanations><explanation>Example of another explanation.</explanation><example>The second example is strange.</example></explanations><other><synonym id='1'>Explanation</synonym><antonym id='3'>Something?</antonym></other><other><synonym id='2'>Type</synonym></other></phrase>";
  m_exampleTransitional = "<?xml version='1.0' encoding='UTF-8'?><!DOCTYPE default><phrase><word>Test</word><explanations><explanation>Type an explanation.</explanation><example>Type an example.</example><explanation2>Type an explanation.</explanation2><example2>Type an example.</example2></explanations></phrase>";

  m_defaultStyleParser = new KSXSLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-default.xsl")));

  QVBoxLayout *defaultStyleLayout=new QVBoxLayout(defaultStyle);
  m_previewDefault=new KHTMLPart(defaultStyle);
  m_previewDefault->setEncoding("utf-8");
  m_previewDefault->begin();
  m_previewDefault->write(m_defaultStyleParser->parse(m_exampleDefault));
  m_previewDefault->end();
  defaultStyleLayout->addWidget(m_previewDefault->view());

  m_transitionalStyleParser = new KSXSLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-transitional.xsl")));

  QVBoxLayout *transitionalStyleLayout=new QVBoxLayout(transitionalStyle);
  m_previewTransitional=new KHTMLPart(transitionalStyle);
  m_previewTransitional->setEncoding("utf-8");
  m_previewTransitional->begin();
  m_previewTransitional->write(m_transitionalStyleParser->parse(m_exampleTransitional));
  m_previewTransitional->end();
  transitionalStyleLayout->addWidget(m_previewTransitional->view());


  connect(styleList, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(slotSelectStyle(QListViewItem*)));
  connect(copyButton, SIGNAL(clicked()), this, SLOT(slotCopyStyle()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(slotDeleteStyle()));
  connect(editButton, SIGNAL(clicked()), this, SLOT(slotEditStyle()));
}

void KSAppearance::populateStyleList()
{
  bool skip=false;
  QStringList styles=KGlobal::dirs()->findAllResources("appdata", QString::fromLatin1("styles/*/*-default.xsl"));
  QStringList added;
  for(QStringList::iterator count=styles.begin();count!=styles.end();count++)
  {
    (*count).remove(QRegExp(".+/")).remove(QRegExp("-.+"));
    for(QStringList::iterator count2=added.begin();count2!=added.end();count2++)
    {
      (*count2).remove(QRegExp(".+/")).remove(QRegExp("-.+"));
      if(*count==*count2)
      {
        skip=true;
        break;
      }
    }
    if(skip)
    {
      skip=false;
      continue;
    }
    added << *count;
    KSListViewItem *temp=new KSListViewItem(styleList, *count);
    if((*count)==Configuration::dictionaryStyle())
    {
      styleList->setSelected(temp, true);
    }
  }
  styleList->sort();
}

void KSAppearance::slotSelectStyle(QListViewItem *selected)
{
  m_defaultStyleParser->setXSL(QString::fromUtf8(locate("appdata", "styles/"+selected->text(0)+"/"+selected->text(0)+"-default.xsl")), selected->text(0));

  m_previewDefault->begin();
  m_previewDefault->write(m_defaultStyleParser->parse(m_exampleDefault));
  m_previewDefault->end();

  m_transitionalStyleParser->setXSL(QString::fromUtf8(locate("appdata", "styles/"+selected->text(0)+"/"+selected->text(0)+"-transitional.xsl")), selected->text(0));

  m_previewTransitional->begin();
  m_previewTransitional->write(m_transitionalStyleParser->parse(m_exampleTransitional));
  m_previewTransitional->end();

  KSData::instance()->setStyle(selected->text(0));
  Configuration::setDictionaryStyle(selected->text(0));
  KSConfigDialog::instance()->manualUpdateButtons();
}

void KSAppearance::slotCopyStyle()
{
  QString newName = KInputDialog::getText(i18n("New style name"), i18n("Name of the new style: "));
  if(newName.isEmpty())
  {
    KMessageBox::error(this, i18n("You haven't entered a name for the copy!"));
    return;
  }
  KIO::copy(locate("appdata", "styles/"+styleList->selectedItem()->text(0)+"/"+styleList->selectedItem()->text(0)+"-default.xsl"), locateLocal("appdata", "styles/"+newName+"/"+newName+"-default.xsl"), false);
  KIO::copy(locate("appdata", "styles/"+styleList->selectedItem()->text(0)+"/"+styleList->selectedItem()->text(0)+"-transitional.xsl"), locateLocal("appdata", "styles/"+newName+"/"+newName+"-transitional.xsl"), false);

  new KSListViewItem(styleList, newName);
}

void KSAppearance::slotDeleteStyle()
{
  QString selectedStyle = styleList->selectedItem()->text(0);
  QString selectedStyleDir = locate("appdata", "styles/"+selectedStyle+"/");

  if(!QFileInfo(selectedStyleDir+selectedStyle+"-default.xsl").isWritable() || !QFileInfo(selectedStyleDir+selectedStyle+"-transitional.xsl").isWritable())
  {
    KMessageBox::error(this, i18n("Cannot delete selected style!"));
    return;
  }
  KIO::del(selectedStyleDir, false, false);

  delete styleList->selectedItem();
}

void KSAppearance::slotEditStyle()
{
  QString selectedStyle = styleList->selectedItem()->text(0);
  QString selectedStyleDir = locate("appdata", "styles/"+selectedStyle+"/");

  if(!QFileInfo(selectedStyleDir+selectedStyle+"-default.xsl").isWritable() || !QFileInfo(selectedStyleDir+selectedStyle+"-transitional.xsl").isWritable())
  {
    KMessageBox::error(this, i18n("Cannot edit selected style!"));
    return;
  }

  KRun::runURL(selectedStyleDir+selectedStyle+"-default.xsl", "text/plain");
  KRun::runURL(selectedStyleDir+selectedStyle+"-transitional.xsl", "text/plain");
}

#include "ksappearance.moc"
