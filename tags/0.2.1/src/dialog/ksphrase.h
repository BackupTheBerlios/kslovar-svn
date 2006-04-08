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

#ifndef _ADDPHRASE_H_
#define _ADDPHRASE_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialogbase.h>

class KIconLoader;
class KSPhraseWdt;
class KSpell;
class KSlovar;
class KSXMLHandler;

/**
 * @short Class for handling words.
 * @author Gregor Kališnik <gregor@podnapisi.net>
 */
class KSPhrase : public KDialogBase
{
  Q_OBJECT
  public:
    /**
     * Constructor that creates KSPhrase.
     * @param parent Parent of the dialog.
     * @param caption Caption of the dialog.
     */
    KSPhrase(QWidget *parent, const QString &caption);
    /**
     * Method for setting data for a word. (For editing purposes only!) Includes XML parsing. (Using KSXMLHandler of course)
     * @param text String in XML format.
     * @param id Word's ID.
     */
    void setWord(QString text, const QString &id);

  private slots:
    /**
     * Slot for handling add button.
     */
    void slotAddExplanation();
    /**
     * Slot for handling remove button.
     */
    void slotRemoveExplanation();
    /**
     * Slot for handling add button. (That right arrow)
     */
    void slotAddSynonym();
    /**
     * Slot for handling remove button. (That left arrow)
     */
    void slotRemoveSynonym();
    /**
     * @see KSPhrase#slotAddSynonym()
     */
    void slotAddAntonym();
    /**
     * @see KSPhrase#slotRemoveSynonym()
     */
    void slotRemoveAntonym();
    /**
     * @see KSPhrase#slotAddSynonym()
     */
    void slotAddFamily();
    /**
     * @see KSPhrase#slotRemoveSynonym()
     */
    void slotRemoveFamily();
    /**
     * Slot for handling spellcheck button.
     */
    void slotBeginCheck();
    /**
     * Slot that does the checking.
     * @param speller Pointer to the spellchecker.
     */
    void slotCheck(KSpell *speller);
    /**
     * Slot for replacing old words with new, fixed.
     */
    void slotEndCheck(const QString& checked);
    /**
     * Slot that enables apply button.
     */
    void slotModified();

  protected slots:
    /**
     * Slot for handling ok button.
     */
    void slotOk();
    /**
     * Slot for handling apply button.
     */
    void slotApply();

  private:
    KSPhraseWdt *m_mainWidget;
    QStringList m_words;
    QString m_id;
    QString m_text;
    QString m_word;
    bool m_edit;
    bool m_modified;
    bool m_create;
    KSXMLHandler *m_XMLHandler;
    QStringList m_deletedSynonyms;
    QStringList m_deletedAntonyms;
    QStringList m_deletedFamily;

    /**
     * Method for populating KComboBox with parts of speech from KSData.
     */
    void populatePartsOfSpeech();
    /**
     * Initialise the basic widgets for KSPhrase.
     */
    void initialize();
    /**
     * Connect all the slots with their signals.
     */
    void connectSlots();
    /**
     * Saves the phrase into KSD.
     */
    void save();
};

#endif
