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
#ifndef CREATEDICTIONARY_H
#define CREATEDICTIONARY_H

#include <kdialogbase.h>

class KSDictionaryWdt;

/**
 * @short Class that handles of making and editing dictionaries.
 * @author Gregor Kališnik
 */
class KSDictionary : public KDialogBase
{
  Q_OBJECT
  public:
    /**
     * Constructor that makes KSDictionary.
     * @param parent Parent of the dialog.
     * @param caption Caption of the dialog. Default is Create dictionary.
     * @param nameDictionary Name of the dictionary. (If we are editing it)
     * @param text Introduction of the dictionary.
     * @param edit Editing a dictionary. Default is false.
     */
    KSDictionary(QWidget *parent = 0, const char *caption="Create dictionary", const QString &nameDictionary=0, const QString &text=0, bool edit=false);

    /**
     * Method for populating KComboBox with languages.
     */
    void populateLanguages();

    /**
     * An ordinary destructor;
     */
    ~KSDictionary();

  private slots:
    /**
     * Slot for handling bold button.
     */
    void slotBold();
    /**
     * Slot for handling italic button.
     */
    void slotItalic();
    /**
     * Slot for handling underline button.
     */
    void slotUnderline();
    /**
     * Slot for defining new size.
     * @param newSize New size of the selected text.
     */
    void slotSize(int newSize);
    /**
     * Slot that initiates and shows KSLanguage.
     */
    void slotAddLang();
    /**
     * @see KSDictionary#slotAddLang()
     */
    void slotEditLang();

  private:
    /**
     * Method that saves the dictionary into KSD.
     */
    bool save();

    KSDictionaryWdt *m_mainWidget;
    QString m_name;
    QString m_text;
    bool m_edit;

  protected slots:
    /**
     * Slot for handling ok button.
     */
    virtual void slotOk();
    /**
     * Slot for handling apply button.
     */
    virtual void slotApply();

};

#endif
