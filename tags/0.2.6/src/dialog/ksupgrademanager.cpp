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
#include "ksupgrademanager.h"

#include "ui/ksupgrademanagerwdt.h"

#include "misc/widget/kslistview.h"

#include "kslovar.h"

#include <qfile.h>

#include <kpushbutton.h>
#include <klocale.h>
#include <kio/jobclasses.h>
#include <kio/job.h>
#include <kactivelabel.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kprogress.h>
#include <kdebug.h>

KSUpgradeManager::KSUpgradeManager(QWidget *parent, const QString &caption)
  : KDialogBase(parent, "KSDownloadManager", true, caption, Ok|Cancel), m_completedDownloads(0)
{
  KIO::CopyJob *copy;

  //Setting up the basics...
  setButtonText(ButtonCode(Ok), i18n("Upgrade"));
  setButtonText(ButtonCode(Cancel), i18n("Close"));
  enableButtonOK(false);

  m_mainWidget = new KSUpgradeManagerWdt(this);
  setMainWidget(m_mainWidget);
  m_mainWidget->downloadList->addColumn(i18n("File"));

  //Download versioning files...
  m_mainWidget->currentLabel->setText(i18n("Version files"));
  copy = KIO::copy(KURL("http://kslovar.berlios.de/version"), KURL("/tmp/version"), false);
  connect(copy, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckVersion()));
  connect(copy, SIGNAL(percent(KIO::Job*, unsigned long)), this, SLOT(slotProgress(KIO::Job*, unsigned long)));
  copy->setInteractive(false);
}

void KSUpgradeManager::slotCheckVersion()
{
  QString localVersion = "000", remoteVersion = "000";
  QString versionFile = locateLocal("appdata", "version");
  QString languageFile = locateLocal("appdata", "languages.ldft", false);

  QFile local(versionFile);
  if(local.exists())
  {
    local.open(IO_ReadOnly);
    QString input;
    local.readLine(input, 5);
    localVersion = input;
    local.close();
  }

  QFile remote("/tmp/version");
  if(remote.exists())
  {
    remote.open(IO_ReadOnly);
    QString input;
    remote.readLine(input, 5);
    remoteVersion = input;
    remote.close();
  }
  else
  {
    m_mainWidget->currentLabel->setText(i18n("Version file unavailable!"));
    return;
  }

  if(remoteVersion > localVersion)
  {
    new KListViewItem(m_mainWidget->downloadList, "Languages.ldft", languageFile, "http://kslovar.berlios.de/languages.ldft");
    enableButtonOK(true);
    m_mainWidget->currentLabel->setText(i18n("nothing"));
  }
  else
  {
    KSlovar::KSInstance()->loadLanguages();
    m_mainWidget->currentLabel->setText(i18n("Everything is up to date."));
  }
}

void KSUpgradeManager::slotCheckDownload(KIO::Job*, const QString &download, const QString &url)
{
  switch(m_downloadState)
  {
    case(REMOVE_BACKUP):
    {
      KIO::DeleteJob *temp = KIO::del(KURL(download+"~"), false, false);
      m_localFile = download;
      m_remoteFile = url;
      connect(temp, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckDownload(KIO::Job*)));
      temp->setInteractive(false);
      m_downloadState = BACKUP;
      break;
    }
    case(BACKUP):
    {
      KIO::CopyJob *temp = KIO::move(KURL(m_localFile), KURL(m_localFile+"~"), false);
      connect(temp, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckDownload(KIO::Job*)));
      temp->setInteractive(false);
      m_downloadState = DOWNLOAD;
      break;
    }
    case(DOWNLOAD):
    {
      KIO::CopyJob *temp = KIO::copy(KURL(m_remoteFile), KURL(m_localFile), false);
      connect(temp, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckDownload(KIO::Job*)));
      connect(temp, SIGNAL(percent(KIO::Job*, unsigned long)), this, SLOT(slotProgress(KIO::Job*, unsigned long)));
      temp->setInteractive(false);
      m_downloadState = COMPLETE;
      break;
    }
    case(DELETE_VERSION):
    {
      KIO::DeleteJob *temp = KIO::del(KURL(locateLocal("appdata", "version")), false, false);
      connect(temp, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckDownload(KIO::Job*)));
      temp->setInteractive(false);
      m_downloadState = MOVE_VERSION;
      break;
    }
    case(MOVE_VERSION):
    {
      KIO::CopyJob *temp = KIO::move(KURL("/tmp/version"), KURL(locateLocal("appdata", "version")), false);
      connect(temp, SIGNAL(result(KIO::Job*)), this, SLOT(slotCheckDownload(KIO::Job*)));
      temp->setInteractive(false);
      m_downloadState = -1;
      break;
    }
    case(COMPLETE):
    {
      m_mainWidget->downloadProgress->setProgress(0);
      m_mainWidget->currentLabel->setText(i18n("Update successful"));
      m_completedDownloads++;
      m_downloadState = DELETE_VERSION;
      slotCheckDownload(0);
      KSlovar::KSInstance()->loadLanguages();
      break;
    }
  }
}

void KSUpgradeManager::slotProgress(KIO::Job*, unsigned long percent)
{
  m_mainWidget->downloadProgress->setProgress(percent);
}

void KSUpgradeManager::slotOk()
{
  int numberDownload = 0;
  for(QListViewItem *count = m_mainWidget->downloadList->firstChild(); count;)
  {
    m_mainWidget->currentLabel->setText(count->text(0));
    m_downloadState = REMOVE_BACKUP;
    slotCheckDownload(0, count->text(1), count->text(2));
    numberDownload++;
    QListViewItem *tmp = count;
    count = count->nextSibling();
    delete tmp;
  }
  /*KIO::del(KURL(locateLocal("appdata", "version")), false, false)->setInteractive(false);
  KIO::move(KURL("/tmp/version"), KURL(locateLocal("appdata", "version")), false)->setInteractive(false);*/
  emit okClicked();
  //accept();
}

KSUpgradeManager::~KSUpgradeManager()
{
}


#include "ksupgrademanager.moc"
