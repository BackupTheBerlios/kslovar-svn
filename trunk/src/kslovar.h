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
class KAction;

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
    
private slots:
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
  QString m_path;
  /**
  * Currently selected phrase.
  */
  QString m_selectedPhrase;
  /**
  * History managment variables
  */
  QValueList<int> m_backHistory;
  QValueList<int> m_forwardHistory;
  QValueList<int>::iterator m_it;
  QValueList<int>::iterator m_itForward;
  
  
  KLineEdit *m_search;
  KListBox *m_list;
  KHTMLPart *m_browser;
  QSplitter *m_split;
  KProgressDialog *m_progress;
  KProgress *m_progressBar;
  CreateDictionary *m_dictionarydlg;
  
  KAction *m_newDictionary;
  KAction *m_openDictionary;
  KAction *m_editDictionary;
  KAction *m_quit;
  KAction *m_back;
  KAction *m_forward;
  KAction *m_home;
  
  /**
   * Updating history
   */
  void addHistory(bool deleteForward=true);
  
  
  void registerButtons();
  void addMenu();
  void addToolbar();
  void disableNavButtons();
  
};

#endif // _KSLOVAR_H_
