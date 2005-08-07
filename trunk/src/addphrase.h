/***************************************************************************
 *   Copyright (C) 2005 by Gregor Kališnik   *
 *   gregor@podnapisi.net   *
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

#ifndef _ADDPHRASE_H_
#define _ADDPHRASE_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>

class KIconLoader;
class AddPhraseWdt;
class KSpell;
class KSlovar;

class AddPhrase : public KDialogBase
{
  Q_OBJECT
  public:
    AddPhrase(QWidget *parent, QString caption);
    void setWord(QString text, QString id);
    void setPath(QString filename);
    
  private slots:
    void slotAddExplanation();
    void slotRemoveExplanation();
    void slotAddWord();
    void slotRemoveWord();
    void slotBeginCheck();
    void slotCheck(KSpell *speller);
    void slotEndCheck(const QString& checked);
    
  protected slots:
    void slotOk();
    
  private:
    AddPhraseWdt *m_mainWidget;
    QString m_path;
    QStringList m_words;
    QString m_id;
    QString m_text;
    bool m_edit;
    
    void populateAvailableList();
    void populateAddPhraseDialog();
};

#endif
