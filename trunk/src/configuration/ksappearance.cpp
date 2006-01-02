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

#include "../handler/ksxmlhandler.h"

#include <qregexp.h>
#include <qlayout.h>

#include <klistbox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kparts/browserextension.h>

KSAppearance::KSAppearance(QWidget *parent, const char *name)
  :KSAppearanceWdt(parent, name)
{
  styleList->addColumn("name");
  styleList->setFullWidth(true);
  populateStyleList();

  m_exampleDefault="<?xml version='1.0' encoding='UTF-8'?><phrase><word>Example</word><type>noun</type><explanations><explanation>An example style for the default dictionary.</explanation><example>You could use this style!</example></explanations><explanations><explanation>Example of another explanation.</explanation><example>The second example is strange.</example></explanations><other><seealso id='1'>Explanation</seealso></other><other><seealso id='2'>Type</seealso></other></phrase>";

  m_defaultStyleParser=new KSXMLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-default.xsl")));


  QVBoxLayout *defaultStyleLayout=new QVBoxLayout(defaultStyle);
  m_previewDefault=new KHTMLPart(defaultStyle);
  m_previewDefault->setEncoding("utf-8");
  m_previewDefault->begin();
  m_previewDefault->write(m_defaultStyleParser->parse(m_exampleDefault));
  m_previewDefault->end();
  defaultStyleLayout->addWidget(m_previewDefault->view());


  connect(styleList, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(selectStyle(QListViewItem*)));
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

void KSAppearance::selectStyle(QListViewItem *selected)
{
  m_defaultStyleParser=new KSXMLHandler(QString::fromUtf8(locate("appdata", "styles/"+selected->text(0)+"/"+selected->text(0)+"-default.xsl")));

  m_previewDefault->begin();
  m_previewDefault->write(m_defaultStyleParser->parse(m_exampleDefault));
  m_previewDefault->end();

  KSData::instance()->setStyle(selected->text(0));
  Configuration::setDictionaryStyle(selected->text(0));
  KSConfigDialog::instance()->manualUpdateButtons();
}



//#include "ksappearance.moc"
