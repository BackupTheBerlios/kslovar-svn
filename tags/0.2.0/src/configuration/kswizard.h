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
#ifndef KSWIZARD_H
#define KSWIZARD_H

#include "ui/kswizardwzt.h"

namespace KIO
{
  class Job;
}

/**
 * @short Class for First time wizard.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSWizard: public KSWizardWzt {
  Q_OBJECT
  public:
    /**
     * KSWizards default constructor.
     */
    KSWizard(QWidget *parent = 0, const char *name = 0);

  private slots:
    /**
     * Slot for initiating last processes.
     */
    void slotFinish();
    /**
     * Slot that is initiated when download is complete.
     * @param job Pointer to the job in question.
     */
    void slotCompletedDownload(KIO::Job *job);
    /**
     * Slot that prints the progress.
     * @param percent The percentage done.
     */
    void slotProgress(KIO::Job*, unsigned long percent);
};

#endif
