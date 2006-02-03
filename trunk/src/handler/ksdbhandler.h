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

#ifndef KSDBHANDLER_H
#define KSDBHANDLER_H
#include <qobject.h>

class KProgressDialog;
class KProgress;
typedef struct sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

/**
* class to read and write dictionaries from SQLite databases
* @short Database handling class
* @author Gregor Kališnik <gregor@podnapisi.net>
*/

class KSDBHandler : public QObject
{
  Q_OBJECT
  public:

    /**
    * Class's constructor that opens a connection to the database
    * @param databasePath Path to the databases
    */
    KSDBHandler(const QString &databasePath);
    /**
    * Desctructor that closes the connection
    */
    bool saveDictionary(const QString &text, const QString &lang, bool create=true);
    bool saveWord(const QString &word, const QString &text, bool add, const QString &id);
    static KSDBHandler *instance(const QString &path);
    bool processQuery(const QString &rawQuery);
    QString processString(const QString &rawQuery, int columns=1);
    QStringList processList(const QString &rawQuery, int columns=1);
    int getId(const QString &search);

    static QString convertString(const QString &input);

    ~KSDBHandler();

  private:
    sqlite3 *m_db;
    sqlite3_stmt *m_rawOutput;
    static QString m_currentPath;
    static KSDBHandler *m_instance;

    /**
    * Query excetution method
    * @param sqlQuery Query to excetute
    * @retval output Returns the result of the SQL query (statement)
    */
    bool query(const QString &sqlQuery, sqlite3_stmt ** output);
    bool query(const QString &sqlQuery);
};

#endif
