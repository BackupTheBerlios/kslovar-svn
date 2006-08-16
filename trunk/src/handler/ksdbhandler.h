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
#include <qthread.h>

typedef struct sqlite3;

typedef QMap<QString, QString> KSResult;

class QMutex;
class KSlovar;

/**
 * Class to read and write dictionaries using SQLite API
 * @short Database handling class
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSDBHandler : public QThread
{
  public:
    /**
     * Class's constructor that opens a connection to the database.
     * @param databasePath Path to the databases.
     */
    KSDBHandler(const QString &databasePath);

    /**
     * Desctructor that closes the connection. (non threaded mode)
     */
    ~KSDBHandler();

    /**
     * Method to save a dictionary. It can create or modify a dictionary.
     * @param text The Introduction part of the dictionary.
     * @param lang Language of the dictionary.
     * @param type Type of a dictionary (default or transitional).
     * @param create If the dictionary should be created. Default is true.
     */
    bool saveDictionary(const QString &text, const QString &lang, const QString &type, bool create=true);
    /**
     * Method used to save a word with explanations and everything.
     * @param word Name of the word.
     * @param text The data stored inside a XML formatted string.
     * @param add True for adding a new word and false for modifying an existing word.
     * @param id Used only when modifying a word. ID of the word you wish to modify.
     * */
    bool saveWord(const QString &word, const QString &text, bool add, const QString &id);
    /**
     * Method for executing non SELECT SQL queries.
     * @param rawQuery The query to execute.
     */
    bool processQuery(const QString &rawQuery);
    /**
     * Method for executing SELECT queries and that will return just one row of results. (Mainly used for reading just one word)
     * @param rawQuery SELECT Query to execute.
     *
     * @return Return the result in a KSResult, which is a QMap<QString, QString>.
     */
    KSResult processString(const QString &rawQuery);
    /**
     * Like processString, but for more rows.
     * @param rawQuery SELECT Query to execute.
     *
     * @return Return the result in a QValueList<KSResult>. KSResult is a QMap<QString, QString>
     */
    QValueList<KSResult> processList(const QString &rawQuery);
    /**
     * Method for converting name into search string. (Changing non-ASCII letters into ASCII letters using conversion table in KSData)
     * @param input String that we want to strip off all non-ASCII letters.
     *
     * @return Converted string.
     */
    static QString convertString(const QString &input);
    /**
     * Method for retrieving an ID of a row, that was last added. (Just a wrapper for sqlite3_last_insert_rowid() function)
     *
     * @return Return ID of last added row.
     */
    int getId();
    /**
     * Method for retrieving an ID of a word with a name. (Some kind of a wrapper for processString() method)
     * @param name Name of the word
     *
     * @return Returns the ID.
     * @return @c -1 No word with the specified name found.
     */
    int getId(const QString& name);
    /**
     * Method that syncs parts of speech in languages.ldft and in the opened dictionary.
     */
    void syncTypes();
    /**
     * Tells the threaded part of KSDBHandler, that it needs to stop. The thread will before stopping, unlock and clear all the variables.
     */
    void stopThread();

  protected:
    /**
     * Method to run in thread.
     *
     * @see Qt documentation
     */
    virtual void run();

  private:
    sqlite3 *m_db;
    QValueList<KSResult> m_result;
    bool m_stop;
    KSlovar *m_reciever;
    QMutex m_locker;
    int m_packets;

    /**
     * Query execution method
     * @param sqlQuery Query to execute.
     */
    bool query(const QString &sqlQuery);
    /**
     * Results processing method.
     */
    static int processQuery(void *primaryArgument, int numberColumns, char **columnValues, char **columnNames);
    /**
     * Special threaded method for obtaining phrases list.
     */
    static int processList(void *primaryArgument, int numberColumns, char **columnValues, char **columnNames);

};

#endif
