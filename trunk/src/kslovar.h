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


#ifndef _KSLOVAR_H_
#define _KSLOVAR_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <kurl.h>
#include <kparts/browserextension.h>
#include <qstringlist.h>
 
class KLineEdit;
class KListBox;
class KHTMLPart;
class QSplitter;
class DBHandler;
class KProgressDialog;
class KProgress;
class CreateDictionary;

/**
 * @short Application Main Window
 * @author Gregor Kališnik <gregor@podnapisi.net>
 * @version 0.0.5
 */
class KSlovar : public KMainWindow
{
    Q_OBJECT
public:  
    /**
     * Default Constructor
     */
    KSlovar();

    /**
     * Default Destructor
     */
    virtual ~KSlovar();
    
    /**
     * Index of phrases.
     */
    QStringList phrases;
    
public slots:
  /**
  * A slot that opens a dictionary from disk.
  */
  void slotFileOpen();
  /**
  * Slot that searches for a phrase in SQLite database.
  */
  void slotShow();
  /**
  * Method that search for a given keyphrase and print result on the list.
  */
  void slotSearch(const QString &text);
  /**
  * Method for showing a phrase using the list.
  */
  void slotShowList();
  /**
  * Method for showing a phrase using the browser.
  */
  void slotShowBrowser(const KURL &url, const KParts::URLArgs &);
  /**
  * Method for looking previously selected phrases.
  */
  void slotPrevPhrase();
  /**
  * @see KSlovar#slotPrevPhrase().
  */
  void slotNextPhrase();
  /**
  * Method to go show first page.
  */
  void slotHome();
  void slotNewDictionary();
  void slotEditDictionary();
    
private:
  /**
  * Path to the dictionary's database file.
  */
  QString path;
  /**
  * Currently selected phrase.
  */
  QString selectedPhrase;
  /**
  * History managment variables
  */
  QValueList<int> back;
  QValueList<int> forward;
  QValueList<int>::iterator it;
  QValueList<int>::iterator itForward;
  
  
  KLineEdit *search;
  KListBox *list;
  KHTMLPart *browser;
  QSplitter *split;
  DBHandler *dictionaryDB;
  KProgressDialog * progress;
  KProgress * progressBar;
  CreateDictionary * dictionarydlg;
  
  /**
   * Updating history
   */
  void addHistory(bool deleteForward=TRUE);
  
};

#endif // _KSLOVAR_H_
