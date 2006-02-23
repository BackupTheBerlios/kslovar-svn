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
#ifndef KSOUTPUTHANDLER_H
#define KSOUTPUTHANDLER_H

#include <qevent.h>

#define LIST 1000
#define CLEAR 1001
#define NORESULT 1002
#define TEXT 1003

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
*/
class KSOutputHandler : public QCustomEvent
{
public:
    KSOutputHandler(const QString &name, const QString &id, const QString &search);
    KSOutputHandler(const QString &text);

    QString getName();
    QString getId();
    QString getSearch();

    QString getText();

    ~KSOutputHandler();

  private:
    QString m_name, m_id, m_search;
    QString m_text;

};

#endif
