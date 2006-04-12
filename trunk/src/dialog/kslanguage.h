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
#ifndef KSLANGUAGE_H
#define KSLANGUAGE_H

#include <kdialogbase.h>

class KSLanguageWdt;

/**
 * @short Class for handling with languages.
 * @author Gregor Kališnik
 */
class KSLanguage : public KDialogBase
{
  Q_OBJECT
  public:
    /**
     * Constructor that creates the KSLanguage
     * @param parent Parent of the dialog.
     * @param caption Caption for KSLanguage.
     * @param language Name of the language.
     * @param langid ID of the language.
     * @param name Name of the class.
     */
    KSLanguage(QWidget *parent=0, const char *caption=0, const QString &language=0, const char *name=0);

    /**
     * Just an empty destructor.
     */
    ~KSLanguage();

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
    KSLanguageWdt *m_mainWidget;
    QString m_path;
    int m_id;
    bool m_edit;

    /**
     * Method that populates KSListView with parts of speech.
     */
    void populateTypeList();
    /**
     * Method for saving the language.
     */
    bool save();

  private slots:
    /**
     * Slot that handles add button.
     */
    void slotAddType();
    /**
     * Slot that handles delete button.
     */
    void slotDeleteType();
    /**
     * Slots that toggles apply button. (enabled, disabled)
     */
    void slotEnableApply();

};

#endif
