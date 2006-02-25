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


#ifndef _KSLOVAR_H_
#define _KSLOVAR_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>
#include <kurl.h>
#include <kparts/browserextension.h>

class KListBox;
class KHTMLPart;
class QSplitter;
class KSDBHandler;
class KSDictionary;
class KAction;
class KToggleAction;
class KSPhrase;
class KListView;
class QListViewItem;
class KSListView;
// class KSListViewSearchLine;
class KSSearchLine;
class KSListViewItem;
class KSConfigDialog;
class QPopupmenu;
class KStatusBar;
class QEvent;


class KSXSLHandler;


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

    static KSlovar *KSInstance();

    void openFile(QString fileName);
    void refresh();
    void loadLanguages();
    void loadPartOfSpeech(int id);

    /**
     * Default Destructor
     */
    virtual ~KSlovar();

  protected:
    bool eventFilter(QObject *object, QEvent *event);
    void customEvent(QCustomEvent *package);

private slots:
  /**
  * A slot that opens a dictionary from disk.
  */
  void slotFileOpen();
  /**
  * Method for showing a phrase using the list.
  */
  void slotShowList(QListViewItem *selected);
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
  void slotClose();
  void slotQuit();
  void slotFind();
  void slotFindNext();
  void slotPrint();
  void slotSelectAll();
  void slotAddPhrase();
  void slotEditPhrase();
  void slotRemovePhrase();
  void slotConfigure();
  void slotUpdateConfiguration();
  void showPopup(KListView*, QListViewItem*, const QPoint &p);
  void slotConversionTable();
  void slotDownloadLanguage();
  void slotToggleLiteral();
  void slotToggleBack();
  void slotFirstRunWizard();
  void slotCountPackages(bool found, bool completed);

private:
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


//   KSListViewSearchLine *m_search;
  KSSearchLine *m_search;
  KSListView *m_list;
  KHTMLPart *m_browser;
  QSplitter *m_split;
  KSDictionary *m_dictionarydlg;
  KSPhrase *m_phrasedlg;
  QString m_welcomeMessage;
  bool m_selected;
  bool m_history;
  KSConfigDialog *m_configDialog;
  QPopupMenu *m_listPopup;
  KStatusBar *m_statusBar;
  unsigned int m_recievedPackages;

  static KSlovar *m_instance;

  KAction *m_newDictionary;
  KAction *m_openDictionary;
  KAction *m_editDictionary;
  KAction *m_quit;
  KAction *m_back;
  KAction *m_forward;
  KAction *m_home;
  KAction *m_close;
  KAction *m_find;
  KAction *m_findNext;
  KToggleAction *m_literalSearch;
  KToggleAction *m_backSearch;
  KAction *m_print;
  KAction *m_selectAll;
  KAction *m_addPhrase;
  KAction *m_editPhrase;
  KAction *m_removePhrase;
  KAction *m_config;
  KAction *m_conversion;
  KAction *m_update;
  KAction *m_wizard;

  KSXSLHandler *XMLParser;

  /**
   * Updating history
   */
  void addHistory(bool deleteForward=true);
  /**
   * Slot that searches for a phrase in SQLite database and displays it.
   */
  void showDictionary();

  void registerButtons();
  void addMenu();
  void addToolbar();
  void disableNavButtons();
  void processFileOpen(QString fileName);

};

#endif // _KSLOVAR_H_
