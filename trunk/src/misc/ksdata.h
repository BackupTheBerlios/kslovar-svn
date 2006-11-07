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
#ifndef KSDATA_H
#define KSDATA_H

#include <qobject.h>
#include <qvaluevector.h>
#include <qvariant.h>

/**
 * @short Class that has dictionary's basic data.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSDictionaryData
{
  public:
     /**
     * A constructor.
     */
    KSDictionaryData()
    {
    }

    /**
     * A constructor.
     * @param name Name of the dictionary.
     * @param description Description of the dictionary.
     * @param writable Is the dictionary writable or not.
     * @param type Type of the dictionary.
     */
    KSDictionaryData(const QString &name, const QString &description, bool writable, const QString &type = "Default")
      : m_name(name),
        m_description(description),
        m_type(type),
        m_writable(writable)
    {
    }

    /**
     * Method for setting a value.
     * @param name Type of the value.
     * @param value Value to set to.
     */
    void setValue(const QString &name, const QVariant &value)
    {
      if (name == "name")
        m_name = value.toString();
      else if (name == "description")
        m_description = value.toString();
      else if (name == "type")
        m_type = value.toString();
      else if (name == "writable")
        m_writable = value.toBool();
      else
        qDebug("Value type doesn't exists.");
    }

    /**
     * Method for retrieving data.
     * @param name Type of the value.
     * @param defaultValue The default value, if doesn't exists allready.
     *
     * @return Returns the data.
     * @return @c QString() Data doesn't exists or the type is doesn't exists.
     */
    QVariant getValue(const QString &name, const QVariant &defaultValue)
    {
      Q_UNUSED(defaultValue)
      if (name == "name") {
        return m_name;
      } else if (name == "description") {
        return m_description;
      } else if (name == "type") {
        return m_type;
      } else if (name == "writable") {
        return m_writable;
      } else {
        return QVariant();
        qDebug("Value type doesn't exists.");
      }
    }

  private:
    QString m_name;
    QString m_description;
    QString m_type;
    bool m_writable;
};

/**
 * @short Structure for easy handling elements.
 * For working with all the emenets that have a name and id. For example... languages.
 */
typedef struct
{
  QString name;
  int id;
}KSElement;

/**
 * @short Structure for easier work with phrases.
 */
typedef struct
{
  int id;
  QString name;
  QString search;
}KSPhrases;

class KSListView;
class KSListViewItem;
class KSDBHandler;


/**
 * @short Class for storing data.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSData : public QObject
{
  Q_OBJECT
  public:
    /**
     * A constructor for KSData.
     */
    KSData();

    /**
     * Method for getting an instance to KSData. If KSData, doesn't exists, it creates it.
     *
     * @return Pointer to KSData instance.
     */
    static KSData *instance();

    /**
     * Stores the dictionary path.
     * @param path Path to the dictionary.
     */
    void setDictionaryPath(const QString &path);
    /**
     * Gives the path to the dictionary.
     *
     * @return Path to the dictionary in a QString.
     */
    QString getDictionaryPath();

    /**
     * Stores a pointer to the KSDBHandler, that handles the dictionary.
     * @param dictionaryHandler A pointer to the KSDBHandler. If set to 0, deletes the current dictionary handler.
     */
    void setDictionary(KSDBHandler *dictionaryHandler = 0);
    /**
     * Gives the pointer to KSDBHandler, that handles with the dictionary.
     *
     * @return Pointer to the KSDBHandler.
     */
    KSDBHandler *getDictionary();

    /**
     * Stores the ID of current used language.
     * @param id ID of the language.
     */
    void setLanguage(int id);
    /**
     * Gives the language ID.
     *
     * @return Language ID.
     */
    int getLanguage();
    /**
     * Stores the pointer to the KSDBHandler, that handles with the languages.ksl.
     * @param languageHandler Pointer to the KSDBHandler.
     */
    void setLanguageHandler(KSDBHandler *languageHandler);
    /**
     * Gives a pointer to the KSDBHandler, that handles with the language.ksl.
     *
     * @return Pointer to the KSDBHandler.
     */
    KSDBHandler *getLanguageHandler();

    /**
     * Sets the type of dictionary.
     * @param id Type ID. (0 for default, 1 for transitional)
     */
    void setType(int id);
    /**
     * Gives the type of dictionary.
     *
     * @return @c 0 It's a default dictionary.
     * @return @c 1 It's a transitional dictionary.
     */
    int getType();

    /**
     * Adds a language to the list.
     * @param name Name of the language.
     * @param id ID of the language.
     */
    void addLanguage(const QString &name, int id);
    /**
     * Gives a list of languages. (Names only)
     *
     * @return A QStringList with languages names.
     */
    QStringList getLanguagesNames();
    /**
     * Gives a language ID with the specified name.
     * @param name Name of the language.
     *
     * @return ID of the language.
     * @return @c -1 There is no language with the specified name.
     */
    int getLanguageId(const QString &name);
    /**
     * Gives a name of the language with the specified ID.
     * @param id ID of the language.
     *
     * @return Name of the language.
     */
    QString getLanguageName(int id);

    /**
     * Adds a part of speech.
     * @param name Name of part of speech.
     * @param id ID of part of speech.
     */
    void addPartOfSpeech(const QString &name, int id);
    /**
     * Gives the list of parts of speech's names.
     *
     * @return A QStringList with names.
     */
    QStringList getPartOfSpeech();
    /**
     * Gives a name of part of speech, that has the specified ID.
     * @param id ID of part of speech.
     *
     * @return Name of part of speech.
     */
    QString getPartOfSpeechName(int id);
    /**
     * Gives an ID of part of speech, that has the specified ID.
     * @param name Name of part of speech.
     *
     * @return Returns the ID.
     */
    int getPartOfSpeechId(const QString &name);
    /**
     * Method for retrieving parts of speech with their ID.
     *
     * @return List of KSElements.
     */
    QValueList<KSElement> getPartOfSpeechList();
    /**
     * Deletes all parts of speech.
     */
    void clearPartOfSpeech();

    /**
     * Sets the main list (the one in KSlovar main Window).
     * @param list Pointer to the KSListView.
     */
    void setMainList(KSListView *list);
    /**
     * Gets a pointer to the KSListView.
     *
     * @return Pointer to the KSListView in KSlovar's main window.
     */
    KSListView *getMainList();

    /**
     * Adds a conversion.
     * @param from From character.
     * @param to To character.
     */
    void addConversion(const QChar &from, const QChar &to);
    /**
     * Gets the whole conversion table.
     *
     * @return A QMap<QChar, QChar> with the conversion table. Key is from character.
     */
    QMap<QChar, QChar> getConvertTable();

    /**
     * Sets the used style.
     * @param selectedStyle Name of the style.
     */
    void setStyle(const QString &selectedStyle);
    /**
     * Gets the used style.
     *
     * @return Name of the style.
     */
    QString getStyle();

    /**
     * Sets the literal search on or off.
     * @param searchType State of the literal search.
     */
    void setLiteralSearch(bool searchType);
    /**
     * Gets the state of literal search.
     *
     * @return @c true Literal search is on.
     * @return @c false Literal search is off.
     */
    bool literalSearch();

    /**
     * Set the back search on or off.
     * @param searchType The state of back search.
     */
    void setBackSearch(bool searchType);
    /**
     * Gets the state of the back search.
     *
     * @return @c true Back search is on.
     * @return @c false Back search is off.
     */
    bool backSearch();

    /**
     * Adds a dictionary to the Dictionary vector.
     * @param entry The data to include into the vector.
     */
    void addDictionaryData(const KSDictionaryData &entry);

    /**
     * Adds a dictionary to the Dictionary vector.
     * @param name Name of the dictionary.
     * @param description Description of the dictionary.
     * @param type Type of the dictionary.
     */
    void addDictionaryData(const QString &name, const QString &description, bool writable, const QString &type);

    /**
     * Gets the dictionary data from ID.
     * @param id ID of the dictionary data.
     *
     * @return Returns KSDictionaryData.
     */
    KSDictionaryData getDictionary(int id);

    /**
     * An empty destructor.
     */
    ~KSData();

  private:
    static KSData *m_instance;

    QString m_dictionaryPath;
    KSDBHandler *m_dictionaryHandler;
    int m_languageId;
    KSDBHandler *m_languageHandler;
    int m_typeId;
    QValueList<KSElement> m_languages;
    QValueList<KSElement> m_partOfSpeech;
    KSListView *m_mainList;
    QString m_selectedStyle;
    QMap<QChar, QChar> m_convertTable;
    QValueVector<KSDictionaryData> m_dictionaryVector;
    bool m_literalSearch;
    bool m_backSearch;

};

#endif
