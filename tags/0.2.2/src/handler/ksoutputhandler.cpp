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
#include "ksoutputhandler.h"

KSOutputHandler::KSOutputHandler(const QString &name, const QString &id, const QString &search)
  : QCustomEvent(LIST), m_name(name), m_id(id), m_search(search)
{
}

KSOutputHandler::KSOutputHandler(const QString &text)
  : QCustomEvent(TEXT), m_text(text)
{
}

QString KSOutputHandler::getName()
{
  return m_name;
}

QString KSOutputHandler::getId()
{
  return m_id;
}

QString KSOutputHandler::getSearch()
{
  return m_search;
}

QString KSOutputHandler::getText()
{
  return m_text;
}

KSOutputHandler::~KSOutputHandler()
{
}

