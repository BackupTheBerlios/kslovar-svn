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
#include "ksquery.h"

KSQuery::KSQuery(int command, const QString &query, QObject *reciever)
  : m_command(command), m_query(query), m_reciever(reciever)
{
}

void KSQuery::setCommand(int command)
{
  m_command = command;
}

int KSQuery::getCommand()
{
  return m_command;
}

void KSQuery::setQuery(const QString &query)
{
  m_query = query;
}

QString KSQuery::getQuery()
{
  return m_query;
}

void KSQuery::setReciever(QObject *reciever)
{
  m_reciever = reciever;
}

QObject *KSQuery::getReciever()
{
  return m_reciever;
}

KSQuery::~KSQuery()
{
}

