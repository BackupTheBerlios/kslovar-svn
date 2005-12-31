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
#include "apperancesrc.h"

#include "../objects/kslistview.h"
#include "../objects/kslistviewitem.h"
#include "../objects/configuration.h"
#include "../objects/instances.h"
#include "../objects/ksconfigdialog.h"
#include "../objects/ksdata.h"

#include "../ksxmlhandler.h"

#include <qregexp.h>
#include <qlayout.h>

#include <klistbox.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kparts/browserextension.h>

ApperanceSrc::ApperanceSrc(QWidget *parent, const char *name)
  :AppearanceWdt(parent, name)
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

void ApperanceSrc::populateStyleList()
{
  QStringList styles=KGlobal::dirs()->findAllResources("appdata", QString::fromLatin1("styles/*/*-default.xsl"));
  for(QStringList::iterator count=styles.begin();count!=styles.end();count++)
  {
    KSListViewItem *temp=new KSListViewItem(styleList, (*count).remove(QRegExp(".+/")).remove(QRegExp("-.+")));
    if((*count)==Configuration::dictionaryStyle())
    {
      styleList->setSelected(temp, true);
    }
  }
  styleList->sort();
}

void ApperanceSrc::selectStyle(QListViewItem *selected)
{
  delete m_defaultStyleParser;
  m_defaultStyleParser=new KSXMLHandler(QString::fromUtf8(locate("appdata", "styles/"+selected->text(0)+"/"+selected->text(0)+"-default.xsl")));

  m_previewDefault->begin();
  m_previewDefault->write(m_defaultStyleParser->parse(m_exampleDefault));
  m_previewDefault->end();

  KSData::instance()->setStyle(selected->text(0));
  Configuration::setDictionaryStyle(selected->text(0));
  kdDebug() << Configuration::dictionaryStyle() << endl;
  Instances::configInstance()->manualUpdateButtons();
}



#include "apperancesrc.moc"
