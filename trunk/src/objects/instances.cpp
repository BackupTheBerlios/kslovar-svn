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
#include "instances.h"

#include "../kslovar.h"

#include <qobject.h>

KSlovar *Instances::m_mainInstance=0L;
QObject *Instances::m_configInstance=0L;

void Instances::setMainInstance(KSlovar *KSlovar)
{
  m_mainInstance=KSlovar;
}

void Instances::setConfigInstance(QObject *Config)
{
  m_configInstance=Config;
}

KSlovar *Instances::mainInstance()
{
  return m_mainInstance;
}

QObject *Instances::configInstance()
{
  return m_configInstance;
}


#include "instances.moc"
