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
#include "kswizard.h"

#include "configuration.h"

#include "kslovar.h"

#include "misc/ksdata.h"

#include "handler/ksdbhandler.h"

#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qfile.h>

#include <kprogress.h>
#include <kactivelabel.h>
#include <kio/job.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <klocale.h>

KSWizard::KSWizard(QWidget *parent, const char *name)
    :KSWizardWzt(parent, name)
{
  m_mouseNavigation->setChecked(Configuration::mouseNavigation());
  m_scrollBar->setChecked(Configuration::scrollBar());
  m_smoothScroll->setChecked(Configuration::smoothScroll());
  m_authorName->setText(Configuration::authorName());
  m_authorEmail->setText(Configuration::authorEmail());

  locateLocal("appdata", "");
  m_downloadProgress->setShown(false);
  m_downloadLabel->setShown(false);
  setFinishEnabled(finish, true);
  connect(finishButton(), SIGNAL(pressed()), this, SLOT(slotFinish()));
  if(QFile::exists(locateLocal("appdata", "languages.ldft", false)))
  {
    languages->setDisabled(true);
    m_remoteLanguages->setChecked(false);
  }
}

void KSWizard::slotFinish()
{
  Configuration::setMouseNavigation(m_mouseNavigation->isChecked());
  Configuration::setScrollBar(m_scrollBar->isChecked());
  Configuration::setSmoothScroll(m_smoothScroll->isChecked());
  Configuration::setAutoUpdateLanguage(m_upgradeManager->isChecked());
  Configuration::setAuthorName(m_authorName->text());
  Configuration::setAuthorEmail(m_authorEmail->text());
  Configuration::writeConfig();

  if(m_remoteLanguages->isChecked())
  {
    m_downloadProgress->setShown(true);
    m_downloadLabel->setShown(true);
    KIO::copy(KURL("http://kslovar.berlios.de/version"), KURL(locateLocal("appdata", "version", false)))->setInteractive(false);
    KIO::CopyJob *download = KIO::copy(KURL("http://kslovar.berlios.de/languages.ldft"), KURL(locateLocal("appdata", "languages.ldft", false)), false);
    connect(download, SIGNAL(result(KIO::Job*)), this, SLOT(slotCompletedDownload(KIO::Job*)));
    connect(download, SIGNAL(percent(KIO::Job*, unsigned long)), this, SLOT(slotProgress(KIO::Job*, unsigned long)));
    download->setInteractive(false);
  }

  if(m_localLanguages->isChecked())
  {
    m_downloadProgress->setShown(false);
    m_downloadLabel->setShown(false);
    KSDBHandler *languageHandler = new KSDBHandler(locateLocal("appdata", "languages.ldft"));
    if(!languageHandler->processQuery("BEGIN TRANSACTION; Create table language ( id_lang INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT , name Varchar(20) ); Create table type ( id_type INTEGER NOT NULL PRIMARY KEY, id_lang INTEGER NOT NULL , name Varchar(20) ); Create table conversion_table ( id_conversion INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT , fromc Char(1) , toc Char(1) , UNIQUE (fromc) ); Create table head ( version Varchar(20) , revision INTEGER , modified INTEGER ); Create table authors ( id_author INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT , name Varchar(20) , email Varchar(20) ); Create table modified ( id_author INTEGER NOT NULL , id_type INTEGER , id_lang INTEGER NOT NULL , last_modified INTEGER , Primary Key (id_author,id_lang) ); COMMIT; INSERT INTO head (version, revision, modified) VALUES ('0.1-rc3', '0', '0');"))
    {
      KMessageBox::error(this, i18n("Could not create languages.ldft!"));
      delete languageHandler;
      languageHandler = 0;
      return;
    }
    delete languageHandler;
    languageHandler = 0;
    close();
  }
}

void KSWizard::slotCompletedDownload(KIO::Job *job)
{
  if(job->error())
  {
    job->showErrorDialog(this);
    return;
  }
  KSlovar::KSInstance()->loadLanguages();
  close();
}

void KSWizard::slotProgress(KIO::Job*, unsigned long percent)
{
  m_downloadProgress->setProgress(percent);
}

#include "kswizard.moc"
