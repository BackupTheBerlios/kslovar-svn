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
 * @short Class for sending commands to threaded classes.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSQuery
{
  public:
    /**
     * Constructor for creating KSQuery.
     * @param command Type of te command.
     * @param query Query to execute. Can be only a parameter to the command. (Example: serch criteria)
     * @param reciever Pointer to the recieving class. (Example: For search is a KSListView)
     */
    KSQuery(int command = 0, const QString &query = 0, QObject *reciever = 0);

    /**
     * Setting the command type manualy.
     * @param command ID of the command.
     */
    void setCommand(int command);
    /**
     * Gets the command ID.
     *
     * @return ID of the command.
     */
    int getCommand();
    /**
     * Manualy sets the query.
     * @param query Query command.
     */
    void setQuery(const QString &query);
    /**
     * Gets the query.
     *
     * @return Query of the command.
     */
    QString getQuery();
    /**
     * Manualy sets the reciever.
     * @param reciever Pointer to the reciever.
     */
    void setReciever(QObject *reciever);
    /**
     * Gets the pointer to the reciever.
     *
     * @return Pointer to the reciever.
     */
    QObject *getReciever();

    /**
     * An empty destructor.
     */
    ~KSQuery();

  private:
    int m_command;
    QString m_query;
    QObject *m_reciever;

};

#endif
