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


#ifndef KSLOVAR_H_
#define KSLOVAR_H_

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
class KSSearchLine;
class KSListViewItem;
class KSConfigDialog;
class QPopupmenu;
//class KStatusBar;
class KProgress;
class QEvent;


class KSXSLHandler;


/**
 * @short KSlovar's Main Window.
 * @author Gregor Kališnik <gregor@podnapisi.net>
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
     * Method that returns an instance to KSlovar. Useful if you want to use it's methods :) (In case taht KSlovar doesn't exists, doesn't crate it though...)
     *
     * @return KSlovar instance. (a pointer :))
     */
    static KSlovar *KSInstance();

    /**
     * A public method for opening a dictionary. Used for calling it from outside KSlovar class.
     * @param fileName The path to the dictionary, that we want to open.
     */
    void openFile(const QString &fileName);
    /**
     * Wrapper for running showDictionary() method.
     */
    void refresh();
    /**
     * Method used for loading and stoing languages from languages.ksl to KSData. Used from outside in case we change the langage database. (KSLanguage)
     */
    void loadLanguages();
    /**
     * Method that reads parts of speech from languages.ksl and stores them into KSData.
     * @param id ID of language.
     */
    void loadPartOfSpeech(int id);

    /**
     * Default Destructor
     */
    virtual ~KSlovar();

  protected:
    /**
   * Redefined eventFilter() for reading output from threaded classes. (KSDBHandler for now) For more info, look into the Qt documentation!
     */
    bool eventFilter(QObject *object, QEvent *event);
    /**
     * Redefined customEvent() method for executing commands. What to execute depends on the type of recieved package. Also used for use of threaded classes. (KSDBHandler) For more info, read Qt documentation!
     */
    void customEvent(QCustomEvent *package);

  private slots:
    /**
     * A slot that opens a dictionary from disk.
     */
    void slotFileOpen();
    /**
     * Method for showing a phrase using the list.
     * @param selected Pointer to the selected item in the list.
     */
    void slotShowList(QListViewItem *selected);
    /**
     * Method for showing a phrase using the browser. This method is redefined, see KDE documentation for more info.
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
     * Method to show first/intoduction page.
     */
    void slotHome();
    /**
     * Slot that inicialises and shows KSDictionary.
     */
    void slotNewDictionary();
    /**
     * @see KSlovar#slotNewDictionary()
     */
    void slotEditDictionary();
    /**
     * Slot for closing an opened dictionary.
     */
    void slotClose();
    /**
     * Slot for quiting KSlovar.
     */
    void slotQuit();
    /**
     * Wrapper slot.
     */
    void slotFind();
    /**
     * @see KSlovar#slotFind()
     */
    void slotFindNext();
    /**
     * Wrapper slot.
     */
    void slotPrint();
    /**
     * Wrapper slot.
     */
    void slotSelectAll();
    /**
     * Slot that inicialises and shows KSPhrase.
     */
    void slotAddPhrase();
    /**
     * @see KSlovar#slotAddPhrase()
     */
    void slotEditPhrase();
    /**
     * Slot for removing a phrase.
     */
    void slotRemovePhrase();
    /**
     * Slot that shows the Configuration panel.
     */
    void slotConfigure();
    /**
     * Empty method :).
     */
    void slotUpdateConfiguration();
    /**
     * Slot used for showing context menu (right click on the mouse :P)
     */
    void showPopup(KListView*, QListViewItem*, const QPoint &p);
    /**
     * Slot for inicialising and showing KSConversion.
     */
    void slotConversionTable();
    /**
     * Slot that inicialises and shows KSUpgradeManager.
     */
    void slotDownloadLanguage();
    /**
     * Slot that toggles betwen literal and non-literal search.
     */
    void slotToggleLiteral();
    /**
     * Slot that toggles betwen normal and back search.
     */
    void slotToggleBack();
    /**
     * Slot that inicialises and shows KSWizard.
     */
    void slotFirstRunWizard();
    /**
     * Slot that counts how many words has been found. Used for the neat ;) status bar.
     * @param found If the word has been found.
     * @param completed If the search has been completed. (KSDBHandler produces no more result)
     */
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
    //KStatusBar *m_statusBar;
    KProgress *m_progress;
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
     * Method to add a visited word into history.
     */
    void addHistory(bool deleteForward=true);
     /**
     * Slot that searches for a phrase in SQLite database and displays it.
     */
    void showDictionary();

    /**
     * Methos declares KAction buttons.
     */
    void registerButtons();
    /**
     * Method that declares menus.
     */
    void addMenu();
    /**
     * Methos that adds menus into toolbar.
     */
    void addToolbar();
    /**
     * Method that disables navigation buttons.
     */
    void disableNavButtons();
    /**
     * Method that really opens a dictionary. (all others are just wrappers)
     * @param fileName Path to the dictionary.
     */
    void processFileOpen(const QString &fileName);

};

#endif // _KSLOVAR_H_
