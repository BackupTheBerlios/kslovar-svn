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

/**
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
typedef struct
{
  QString name;
  int id;
}KSElement;

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
	@author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSData : public QObject
{
  Q_OBJECT
  public:
    KSData();

    static KSData *instance();

    void setDictionaryPath(QString path);
    QString getDictionaryPath();

    void setDictionary(KSDBHandler *dictionaryHandler);
    KSDBHandler *getDictionary();

    void setLanguage(int id);
    int getLanguage();
    void setLanguageHandler(KSDBHandler *languageHandler);
    KSDBHandler *getLanguageHandler();

    void setType(int id);
    int getType();

    void addLanguage(QString name, int id);
    QStringList getLanguagesNames();
    QString getLanguageId(QString name);

    void addPartOfSpeech(QString name, int id);
    QStringList getPartOfSpeech();
    QString getPartOfSpeechName(int id);
    int getPartOfSpeechId(QString name);
    void clearPartOfSpeech();

    void addPhrase(int id, const QString &name, const QString &search);
    QValueList<KSPhrases> getPhrases();
    void clearPhrases();

    void setMainList(KSListView *list);
    KSListView *getMainList();

    void addConversion(const QChar &from, const QChar &to);
    QMap<QChar, QChar> getConvertTable();

    void setStyle(QString selectedStyle);
    QString getStyle();

    void setLiteralSearch(bool searchType);
    bool literalSearch();

    void setBackSearch(bool searchType);
    bool backSearch();

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
    QValueList<KSPhrases> m_phrases;
    KSListView *m_mainList;
    QString m_selectedStyle;
    QMap<QChar, QChar> m_convertTable;
    bool m_literalSearch;
    bool m_backSearch;

};

#endif
