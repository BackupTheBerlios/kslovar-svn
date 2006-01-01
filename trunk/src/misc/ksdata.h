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
}KSPhrase;


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

    void setLanguage(int id);
    int getLanguage();

    void addLanguage(QString name, int id);
    QStringList getLanguagesNames();
    QString getLanguageId(QString name);

    void addPartOfSpeech(QString name, int id);
    QStringList getPartOfSpeech();
    QString getPartOfSpeechName(int id);
    int getPartOfSpeechId(QString name);
    void clearPartOfSpeech();

    void addPhrase(int id, QString name, QString search);
    QValueList<KSPhrase> getPhrases();
    void clearPhrases();

    void setStyle(QString selectedStyle);
    QString getStyle();

    ~KSData();

  private:
    static KSData *m_instance;

    QString m_dictionaryPath;
    int m_languageId;
    QValueList<KSElement> m_languages;
    QValueList<KSElement> m_partOfSpeech;
    QValueList<KSPhrase> m_phrases;
    QString m_selectedStyle;

};

#endif
