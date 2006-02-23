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
#ifndef KSQUERY_H
#define KSQUERY_H


#define SEARCH 0
#define STRING 1

#include <qstring.h>

class QObject;

/**
	@author Gregor Kališnik <gregor@podnapisi.net>
*/
class KSQuery
{
public:
    KSQuery(int command = 0, const QString &query = 0, QObject *reciever = 0);

    void setCommand(int command);
    int getCommand();
    void setQuery(const QString &query);
    QString getQuery();
    void setReciever(QObject *reciever);
    QObject *getReciever();

    ~KSQuery();

  private:
    int m_command;
    QString m_query;
    QObject *m_reciever;

};

#endif
