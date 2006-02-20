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
#ifndef KSUPGRADEMANAGER_H
#define KSUPGRADEMANAGER_H

#include <kdialogbase.h>

#define REMOVE_BACKUP 0
#define BACKUP 1
#define DOWNLOAD 2
#define DELETE_VERSION 3
#define MOVE_VERSION 4
#define COMPLETE 5

class KSUpgradeManagerWdt;
class KURL;
namespace KIO
{
  class Job;
};

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSUpgradeManager : public KDialogBase
{
  Q_OBJECT
  public:
    KSUpgradeManager(QWidget *parent = 0, const QString &name = QString::null);

    ~KSUpgradeManager();

  protected slots:
    void slotOk();

  private slots:
    void slotCheckDownload(KIO::Job*, const QString &download = QString::null, const QString &url = QString::null);
    void slotCheckVersion();
    void slotProgress(KIO::Job*, unsigned long percent);

  private:
    KSUpgradeManagerWdt *m_mainWidget;
    short m_downloadState;
    int m_completedDownloads;
    QString m_localFile;
    QString m_remoteFile;


};

#endif
