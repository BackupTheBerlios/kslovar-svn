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
#include <klocale.h>

KSWizard::KSWizard(QWidget *parent, const char *name)
    :KSWizardWzt(parent, name)
{
  m_mouseNavigation->setChecked(Configuration::mouseNavigation());
  m_scrollBar->setChecked(Configuration::scrollBar());
  m_smoothScroll->setChecked(Configuration::smoothScroll());
  locateLocal("appdata", "");
  m_downloadProgress->setShown(false);
  m_downloadLabel->setShown(false);
  setFinishEnabled(finish, true);
  connect(finishButton(), SIGNAL(pressed()), this, SLOT(slotFinish()));
  if(QFile::exists(locateLocal("appdata", "languages.ksl", false)))
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
  Configuration::writeConfig();

  if(m_remoteLanguages->isChecked())
  {
    m_downloadProgress->setShown(true);
    m_downloadLabel->setShown(true);
    KIO::copy(KURL("http://kslovar.berlios.de/version"), KURL(locateLocal("appdata", "version", false)))->setInteractive(false);
    KIO::CopyJob *download = KIO::copy(KURL("http://kslovar.berlios.de/languages.ksl"), KURL(locateLocal("appdata", "languages.ksl", false)), false);
    connect(download, SIGNAL(result(KIO::Job*)), this, SLOT(slotCompletedDownload(KIO::Job*)));
    connect(download, SIGNAL(percent(KIO::Job*, unsigned long)), this, SLOT(slotProgress(KIO::Job*, unsigned long)));
    download->setInteractive(false);
  }

  if(m_localLanguages->isChecked())
  {
    m_downloadProgress->setShown(false);
    m_downloadLabel->setShown(false);
    if(!KSData::instance()->getLanguageHandler()->processQuery("BEGIN TRANSACTION; CREATE TABLE language ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT ); CREATE TABLE type ( id INTEGER PRIMARY KEY AUTOINCREMENT , name TEXT , id_lang INTEGER ); CREATE TABLE conversion_table ( id INTEGER PRIMARY KEY AUTOINCREMENT , fromc TEXT UNIQUE , toc TEXT ); COMMIT;"))
    {
      KMessageBox::error(this, i18n("Could not create languages.ksl!"));
      return;
    }
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
