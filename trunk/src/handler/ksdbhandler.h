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

#include <qthread.h>
#include <qvaluelist.h>
#include <qobject.h>

class KSListView;
class QMutex;
class KSQuery;
class QStringList;
typedef struct sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;

/**
* class to read and write dictionaries using SQLite API
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
     * Method to save a dictionary. It can create or modify a dictionary.
     * @param text The Introduction part of the dictionary.
     * @param lang Language of the dictionary.
     * @param type Type of a dictionary (default or transitional).
     * @param create If the dictionary should be created. Default is true.
     *
     * @return @c true Dictionary was successfully saved
     * @return @c false Dictionary wasn't saved successfully
     */
    bool saveDictionary(const QString &text, const QString &lang, const QString &type, bool create=true);
    /**
     * Method used to save a word with explanations and everything.
     * @param word Name of the word.
     * @param text The data stored inside a XML formatted string.
     * @param add True for adding a new word and false for modifying an existing word.
     * @param id Used only when modifying a word. ID of the word you wish to modify.
     *
     * @return @c true Word was saved successfully.
     * @return @c false Word wasn't saved successfully.
     * */
    bool saveWord(const QString &word, const QString &text, bool add, const QString &id);
    /**
     * Method for executing non SELECT SQL queries.
     * @param rawQuery The query to execute.
     *
     * @return @c true Query was successfull.
     * @return @c false Query wasn't successfull.
     */
    bool processQuery(const QString &rawQuery);
    /**
     * Method for executing SELECT queries and that will return just one row of results. (Mainly used for reading just one word)
     * @param rawQuery SELECT Query to execute.
     * @param columns Number of columns that we want to read.
     *
     * @return Return the result in a QString.
     * @return @c QString::null There is no result.
     */
    QString processString(const QString &rawQuery, int columns=1);
    /**
     * Like processString, but for more rows.
     * @param rawQuery SELECT Query to execute.
     * @param columns Number of columns that we want to read.
     *
     * @return Return the result in a QStringList. (If empty, returns empty QStringList)
     */
    QStringList processList(const QString &rawQuery, int columns=1);
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
     * Method for converting name into search string. (Changing non-ASCII letters into ASCII letters using conversion table in KSData)
     * @param input String that we want to strip off all non-ASCII letters.
     *
     * @return Converted string.
     */
    static QString convertString(const QString &input);

    /**
     * Method for adding a command into the queue. Used in the threaded KSDBHandler only.
     * @param query The command we use to execute.
     */
    void addQueue(KSQuery query);
    /**
     * Method for shutting down the thread. It doesn't work like QThread::terminate() method, thread is stopped peacefully :). For threaded KSDBHandler only!
     */
    void terminate();
    /**
     * Method for stopping the current command. (For now stops only the search in progress)
     */
    void skip();
    /**
     * Method is used to know if the thread is in process of stopping the current command.
     *
     * @return @c true The KSDBhandler's thread is stopping a command.
     * @return @c false The KSDBHandler's thread is stopping no one ;).
     */
    bool isSkiped();

    /**
     * Desctructor that closes the connection. (non threaded mode)
     */
    ~KSDBHandler();

  protected:
    /**
     * Method that defines what the thread should do. (Just Another Loop :D)
     */
    virtual void run();

  private:
    sqlite3 *m_db;
    sqlite3_stmt *m_rawOutput;
    static QString m_currentPath;

    QValueList<KSQuery> m_commandQueue;
    QMutex locker;
    QObject *m_reciever;
    bool m_terminate;
    bool m_skip;
    int m_try;

    /**
     * Query excetution method, that returns results. (SELECT queries)
     * @param sqlQuery Query to excetute.
     * @retval output Returns the result of the SQL query. (statement)
     *
     * @return @c true Query was successfull.
     * @return @c false Query failed.
     */
    bool query(const QString &sqlQuery, sqlite3_stmt **output);
    /**
     * Query execution method, that doesn't return results. (non-SELECT queries)
     * @param sqlQuery Query to execute.
     *
     * @return @c true Query was successfull.
     * @return @c false Query failed.
     */
    bool query(const QString &sqlQuery);
    /**
     * Method for searching for a word.
     * @param criteria The search criteria.
     */
    void search(const QString &criteria);
};

#endif
