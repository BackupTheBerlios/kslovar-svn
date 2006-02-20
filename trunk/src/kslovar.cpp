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

#include "kslovar.h"
#include "configuration/configuration.h"

#include "dialog/ksdictionary.h"
#include "dialog/ksphrase.h"
#include "dialog/ksconversion.h"
#include "dialog/ksupgrademanager.h"

#include "configuration/ksappearance.h"
#include "configuration/ui/ksbehaviourwdt.h"
#include "configuration/ksconfigdialog.h"

#include "handler/ksdbhandler.h"
#include "handler/ksxslhandler.h"

#include "misc/ksdata.h"

#include "misc/widget/kslistview.h"
#include "misc/widget/kslistviewitem.h"
#include "misc/widget/kslistviewsearchline.h"


#include <qvbox.h>
#include <qhbox.h>
#include <qpopupmenu.h>
#include <qfile.h>

#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kapp.h>
#include <khtml_part.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kparts/browserextension.h>
#include <kshortcut.h>
#include <kaction.h>
#include <kactionclasses.h>
#include <khtmlview.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcmenumngr.h>
#include <kio/netaccess.h>
#include <kio/job.h>


KSlovar *KSlovar::m_instance=0L;

KSlovar::KSlovar()
  : KMainWindow( 0, "KSlovar" ), m_dictionarydlg(0)
{
  KSlovar::m_instance=this;
  m_configDialog=new KSConfigDialog(this, "settings", Configuration::self());
  //XMLParser=new KSXSLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-default.xsl")));

  if(Configuration::autoUpdateLanguage())
  {
    slotDownloadLanguage();
  }
  else
  {
    loadLanguages();
  }

  m_welcomeMessage=i18n("<h1>Welcome to KSlovar</h1> This needs to be changed :).");

  registerButtons();
  addMenu();
  addToolbar();

  QHBox * horiz = new QHBox( this );

  m_split = new QSplitter( horiz );

  QVBox * vert = new QVBox( m_split );
  vert->setMaximumWidth(200);
  vert->setMinimumWidth(200);

  m_search = new KSListViewSearchLine( vert );
  m_list = new KSListView( vert );
  m_list->addColumn("name");
  m_list->setColumnWidth(0, 193);
  m_list->setFullWidth(true);
  m_search->setListView(m_list);

  m_browser=new KHTMLPart( m_split );
  m_browser->setEncoding("utf-8", true);
  slotClose();

  connect( m_browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this, SLOT( slotShowBrowser(const KURL &, const KParts::URLArgs &) ) );
  connect(m_list, SIGNAL( selectionChanged(QListViewItem *)), this, SLOT( slotShowList(QListViewItem *) ) );
  connect(m_list, SIGNAL(doubleClicked( QListViewItem *)), this, SLOT(slotEditPhrase()));
  connect(kapp, SIGNAL(shutDown()), this, SLOT(slotClose()));
  connect(m_list, SIGNAL(contextMenu(KListView*, QListViewItem*, const QPoint&)), this, SLOT(showPopup( KListView*, QListViewItem*, const QPoint& )));

  setCentralWidget( horiz );
}

void KSlovar::slotFileOpen()
{
  processFileOpen(KFileDialog::getOpenFileName(":dictionary", "*.ksd|KSlovar dictionary file", this));
}

void KSlovar::showDictionary()
{
  m_history=false;
  m_browser->begin();
  m_browser->write(XMLParser->parse(KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+m_selectedPhrase+"' LIMIT 1;")));
  m_browser->end();

  if(!m_backHistory.isEmpty())
  {
    m_back->setEnabled(true);
  }
  else
  {
    m_back->setEnabled(false);
  }
}

void KSlovar::slotShowList(QListViewItem *selected)
{
  m_editPhrase->setEnabled(true);
  m_removePhrase->setEnabled(true);

  if(m_history==false)
  {
    addHistory();
  }

  m_selectedPhrase=static_cast<KSListViewItem*> (selected)->getId();
  showDictionary();
}

void KSlovar::slotShowBrowser(const KURL &url, const KParts::URLArgs &)
{
  QListViewItem *current=m_list->firstChild();
  while(current)
  {
    if(static_cast<KSListViewItem*> (current)->getId()==url.host())
    {
      m_list->setSelected(current, true);
      break;
    }
    current=current->nextSibling();
  }
}

void KSlovar::slotPrevPhrase()
{
  int temp = m_backHistory.first();
  m_itForward = m_forwardHistory.prepend( m_selectedPhrase.toInt() );

  m_it = m_backHistory.remove(m_it);

  if(m_backHistory.isEmpty())
  {
    m_back->setEnabled(false);
  }
  m_forward->setEnabled(true);

  m_history=true;

  if(temp==0)
  {
    m_list->clearSelection();
    m_editPhrase->setEnabled(false);
    m_selectedPhrase.setNum(0);
    showDictionary();
  }
  else
  {
    QListViewItem *current=m_list->firstChild();
    while(current)
    {
      if(static_cast<KSListViewItem*> (current)->getId().toInt()==temp)
      {
        m_list->setSelected(current, true);
        break;
      }
      current=current->nextSibling();
    }
  }
}

void KSlovar::slotNextPhrase()
{
  int temp = m_forwardHistory.first();

  addHistory(false);

  m_itForward = m_forwardHistory.remove(m_itForward);

  if(m_forwardHistory.isEmpty())
  {
    m_forward->setEnabled(false);
  }

  m_history=true;

  QListViewItem *current=m_list->firstChild();
  while(current)
  {
    if(static_cast<KSListViewItem*> (current)->getId().toInt()==temp)
    {
      m_list->setSelected(current, true);
      break;
    }
    current=current->nextSibling();
  }
}

void KSlovar::slotHome()
{
  if(m_selectedPhrase=="0")
  {
    return;
  }

  addHistory();

  m_selectedPhrase.setNum(0);
  m_list->clearSelection();
  m_editPhrase->setEnabled(false);

  showDictionary();
}

void KSlovar::addHistory(bool deleteForward)
{
  if(!m_selectedPhrase.isEmpty())
  {
    m_it = m_backHistory.prepend(m_selectedPhrase.toInt());
  }

  if(deleteForward)
  {
    m_forwardHistory.clear();
    m_forward->setEnabled(false);
  }
}

void KSlovar::slotNewDictionary()
{
  m_dictionarydlg = new KSDictionary(this);
  m_dictionarydlg->resize(700, 700);
  m_dictionarydlg->show();
}

void KSlovar::slotEditDictionary()
{
  QString text=KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='0' LIMIT 1;");
  QString name=text;

  text.remove(QRegExp("<h1>.+</h1>"));
  name.remove(text).remove("<h1>").remove("</h1>");

  m_dictionarydlg = new KSDictionary(this, "Edit Dictionary", name, text, true);
  m_dictionarydlg->resize(700, 700);
  m_dictionarydlg->show();
}

void KSlovar::registerButtons()
{
  m_newDictionary = new KAction(i18n("&New dictionary"), "filenew", KShortcut(KKey("CTRL+n")), this, SLOT(slotNewDictionary()), actionCollection(), "newDictionary");
  m_openDictionary = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  m_quit = KStdAction::quit(kapp, SLOT(quit()), actionCollection());

  m_back=KStdAction::back(this, SLOT(slotPrevPhrase()), actionCollection());
  m_forward=KStdAction::forward(this, SLOT(slotNextPhrase()), actionCollection());
  m_home=KStdAction::home(this, SLOT(slotHome()), actionCollection());

  m_editDictionary = new KAction(i18n("&Edit dictionary"), "edit", KShortcut(KKey("CTRL+e")), this, SLOT(slotEditDictionary()), actionCollection(), "editDictionary");
  m_close=KStdAction::close(this, SLOT(slotClose()), actionCollection());

  m_find = KStdAction::find(this, SLOT(slotFind()), actionCollection());
  m_findNext = KStdAction::findNext(this, SLOT(slotFindNext()), actionCollection());
  m_literalSearch = new KToggleAction(i18n("&Literal search"), "filter", KShortcut(KKey("CTRL+l")), this, SLOT(slotToggleLiteral()), actionCollection(), "literalSearch");

  m_print=KStdAction::print(this, SLOT(slotPrint()), actionCollection());
  m_selectAll=KStdAction::selectAll(this, SLOT(slotSelectAll()), actionCollection());

  m_addPhrase=new KAction(i18n("&Add phrase"), "filenew", KShortcut(KKey("CTRL+a")), this, SLOT(slotAddPhrase()), actionCollection(), "addPhrase");
  m_editPhrase=new KAction(i18n("Edi&t phrase"), "edit", KShortcut(KKey("CTRL+t")), this, SLOT(slotEditPhrase()), actionCollection(), "editPhrase");
  m_removePhrase=new KAction(i18n("&Remove phrase"), "editdelete", KShortcut(KKey("Delete")), this, SLOT(slotRemovePhrase()), actionCollection(), "removePhrase");

  m_listPopup=new QPopupMenu;
  m_addPhrase->plug(m_listPopup);
  m_editPhrase->plug(m_listPopup);
  m_removePhrase->plug(m_listPopup);

  m_config = KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
  m_conversion = new KAction(i18n("Edit &conversion table"), "conversion", this, SLOT(slotConversionTable()), actionCollection(), "editConversion");
  m_update = new KAction(i18n("Update &languages"), "ktalkd", KShortcut(KKey("")), this, SLOT(slotDownloadLanguage()), actionCollection(), "updateLanguages");
}

void KSlovar::addMenu()
{
  KPopupMenu * filemenu = new KPopupMenu;
  m_newDictionary->plug(filemenu);
  m_openDictionary->plug(filemenu);
  filemenu->insertSeparator();
  m_print->plug(filemenu);
  filemenu->insertSeparator();
  m_close->plug(filemenu);
  m_quit->plug(filemenu);

  KPopupMenu *editmenu=new KPopupMenu;
  m_editDictionary->plug(editmenu);
  m_selectAll->plug(editmenu);
  editmenu->insertSeparator();
  m_addPhrase->plug(editmenu);
  m_editPhrase->plug(editmenu);
  m_removePhrase->plug(editmenu);
  editmenu->insertSeparator();
  m_find->plug(editmenu);
  m_findNext->plug(editmenu);
  m_literalSearch->plug(editmenu);

  KPopupMenu *gomenu=new KPopupMenu;
  m_back->plug(gomenu);
  m_forward->plug(gomenu);
  m_home->plug(gomenu);

  KPopupMenu *setmenu=new KPopupMenu;
  m_config->plug(setmenu);
  m_conversion->plug(setmenu);
  m_update->plug(setmenu);

  KPopupMenu *help = helpMenu( );

  KMenuBar * menu = menuBar();
  menu->insertItem( i18n( "&File" ), filemenu );
  menu->insertItem(i18n("&Edit"), editmenu);
  menu->insertItem(i18n("&Go"), gomenu);
  menu->insertItem(i18n("&Settings"), setmenu);
  menu->insertItem( i18n( "&Help" ), help );
}

void KSlovar::addToolbar()
{
  KToolBar *toolbar = new KToolBar(this);
  m_literalSearch->plug(toolbar);
  m_back->plug(toolbar);
  m_forward->plug(toolbar);
  m_home->plug(toolbar);

  KToolBar *utilityToolbar=new KToolBar(this);
  m_print->plug(utilityToolbar);
  m_find->plug(utilityToolbar);

  addToolBar(toolbar);
  addToolBar(utilityToolbar);
}

void KSlovar::disableNavButtons()
{
  m_back->setEnabled(false);
  m_forward->setEnabled(false);
  m_home->setEnabled(false);
  m_editDictionary->setEnabled(false);
  m_close->setEnabled(false);
  m_addPhrase->setEnabled(false);
  m_editPhrase->setEnabled(false);
  m_removePhrase->setEnabled(false);
}

void KSlovar::slotClose()
{
  disableNavButtons();
  KSData::instance()->clearPhrases();
  m_backHistory.clear();
  m_forwardHistory.clear();
  m_selectedPhrase="";
  m_browser->begin();
  m_browser->write(m_welcomeMessage);
  m_browser->end();
  m_list->clear();
}

void KSlovar::slotQuit()
{
  slotClose();
}

void KSlovar::slotFind()
{
  m_browser->findText();
}

void KSlovar::slotFindNext()
{
  m_browser->findTextNext();
}

void KSlovar::slotPrint()
{
  m_browser->view()->print();
}

void KSlovar::slotSelectAll()
{
  m_browser->selectAll();
}

void KSlovar::slotAddPhrase()
{
  m_phrasedlg=new KSPhrase(this, "Add word");
  m_phrasedlg->resize(700, 700);
  m_phrasedlg->show();
}

void KSlovar::slotEditPhrase()
{
  if(!m_editPhrase->isEnabled())
  {
    return;
  }
  QString output=KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+m_selectedPhrase+"' LIMIT 1;");
  m_phrasedlg=new KSPhrase(this, "Edit word");
  m_phrasedlg->setWord(output, m_selectedPhrase);
  m_phrasedlg->resize(700, 700);
  m_phrasedlg->show();
}

void KSlovar::slotRemovePhrase()
{
  if(KMessageBox::questionYesNo(this, i18n("Are you really sure you want to delete this word?"), i18n("Confirm deletion"), i18n("Delete"))==KMessageBox::No)
  {
    return;
  }
  KSListViewItem *temp=static_cast<KSListViewItem*> (m_list->selectedItem());
  KSDBHandler::instance(KSData::instance()->getDictionaryPath())->processQuery("DELETE FROM phrases WHERE id='"+temp->getId()+"'; DELETE FROM dictionary WHERE id='"+temp->getId()+"';");
  delete temp;
  m_editPhrase->setEnabled(false);
  m_removePhrase->setEnabled(false);
}

void KSlovar::slotConversionTable()
{
  KSConversion *temp = new KSConversion(this, "Edit Conversion table");
  temp->resize(300, 300);
  temp->show();
}

void KSlovar::slotConfigure()
{
  m_configDialog->addPage(new KSAppearance(0), i18n("Appearance"), "looknfeel");
  m_configDialog->addPage(new KSBehaviourWdt(0), i18n("Behaviour"), "package_settings");

  m_configDialog->resize(700, 700);
  m_configDialog->show();
}

void KSlovar::slotUpdateConfiguration()
{

}

void KSlovar::openFile(QString fileName)
{
  KSData::instance()->setDictionaryPath(fileName);
  processFileOpen(fileName);
}

void KSlovar::processFileOpen(QString fileName)
{
  QStringList temp;
  if( !fileName.isEmpty() )
  {
    QFile file( fileName );
    if( file.open(IO_ReadOnly) )
    {
      QString temp;
      file.readLine(temp, 50);
      if( !temp.startsWith("SQLite format 3") )
      {
        KMessageBox::error(this, i18n("This is not a valid dictionary!") );
        return;
      }
      slotClose();
    }
    else
    {
      KMessageBox::error(this, i18n("Couldn't read the dictionary!") );
      return;
    }
    file.close();
    m_search->setListView(0);

    temp=KSDBHandler::instance(fileName)->processList("SELECT name, id, search FROM phrases ORDER BY search ASC;", 3);

    for(QStringList::iterator phrase = temp.begin(); phrase != temp.end(); phrase++)
    {
      QString word = *phrase;
      QString id= *phrase;
      QString search= *phrase;
      new KSListViewItem(m_list, word.remove(QRegExp("/.+$")), id.remove(QRegExp("[^/\\d]+")).remove(QRegExp("^\\d+")).remove(QRegExp("\\d+$")).remove(QRegExp("/+")), search.remove(QRegExp("^.+/")));
      KSData::instance()->addPhrase(id.toInt(), word, search);
    }

    m_home->setEnabled(true);
    m_editDictionary->setEnabled(true);
    m_close->setEnabled(true);
    m_addPhrase->setEnabled(true);

    slotHome();
    m_backHistory.clear();
    KSData::instance()->setDictionaryPath(fileName);
    loadPartOfSpeech(KSDBHandler::instance(fileName)->processString("SELECT lang FROM head;").toInt());
    int type = KSDBHandler::instance(fileName)->processString("SELECT type FROM head;").toInt();
    KSData::instance()->setType(type);
    if(!type)
    {
      XMLParser=new KSXSLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-default.xsl")));
    }
    else
    {
      XMLParser=new KSXSLHandler(QString::fromUtf8(locate("appdata", "styles/"+Configuration::dictionaryStyle()+"/"+Configuration::dictionaryStyle()+"-transitional.xsl")));
    }

    m_search->setListView(m_list);
    m_search->updateSearch();
  }
}

void KSlovar::loadLanguages()
{
  if(!QFile::exists(locateLocal("appdata", "languages.ksl", false))) //Check if Languages.ksl exists. If not, run the upgrade manager to download it.
  {
    if(QFile::exists(locateLocal("appdata", "languages.ksl", false)+"~")) //Check if Languages.ksl exists. If not, run the upgrade manager to download it.
    {
      KIO::move(KURL(locateLocal("appdata", "languages.ksl", false)+"~"), KURL(locateLocal("appdata", "languages.ksl", false)), false)->setInteractive(false);
      KIO::del(KURL(locateLocal("appdata", "version", false)), false, false)->setInteractive(false);
      KMessageBox::information(this, i18n("Could not find languages.ksl. But found it's backup and using it."));
    }
    else
    {
      KIO::del(KURL(locateLocal("appdata", "version", false)), false, false)->setInteractive(false);
      KMessageBox::error(this, i18n("Could not find languages.ksl. Run Upgrade manager to download it.\n\nIf you do not have any internet connection, you can download it from http://kslovar.berlios.de/languages.ksl and put it into ~/.kde/share/apps/kslovar/."));
      return;
    }
  }
  QStringList input=KSDBHandler::instance(QString::fromUtf8(locateLocal("appdata", "languages.ksl", false)))->processList("SELECT id, name FROM language;", 2);
  if(!input.isEmpty())
  {
    QString id, name;
    for(QStringList::iterator count=input.begin();count!=input.end();count++)
    {
      id=*count;
      name=*count;
      KSData::instance()->addLanguage(name.remove(QRegExp("^.+/")), id.remove(QRegExp("/.+$")).toInt());
    }
  }

  if(m_dictionarydlg)
  {
    m_dictionarydlg->populateLanguages();
  }

  input.clear();
  input=KSDBHandler::instance(QString::fromUtf8(locateLocal("appdata", "languages.ksl", false)))->processList("SELECT fromc, toc FROM conversion_table;", 2);
  if(!input.isEmpty())
  {
    QChar from, to;
    for(QStringList::iterator count=input.begin();count!=input.end();count++)
    {
      from=(*count).at(0);
      to=(*count).at(2);
      KSData::instance()->addConversion(from, to);
    }
  }
}

void KSlovar::loadPartOfSpeech(int id)
{
  KSData::instance()->clearPartOfSpeech();
  KSData::instance()->setLanguage(id);
  QStringList input=KSDBHandler::instance(QString::fromUtf8(locate("appdata", "languages.ksl", false)))->processList("SELECT id, name FROM type WHERE id_lang='"+QString::number(id)+"';", 2);
  if(!input.isEmpty())
  {
    QString id, name;
    for(QStringList::iterator count=input.begin();count!=input.end();count++)
    {
      id=*count;
      name=*count;
      KSData::instance()->addPartOfSpeech(name.remove(QRegExp("^.+/")), id.remove(QRegExp("/.+$")).toInt());
    }
  }
}

void KSlovar::refresh()
{
  showDictionary();
}

void KSlovar::showPopup(KListView*, QListViewItem*, const QPoint &p)
{
  m_listPopup->popup(p);
}

KSlovar *KSlovar::KSInstance()
{
  if(!m_instance)
  {
    return 0l;
  }
  return m_instance;
}

void KSlovar::slotDownloadLanguage()
{/*
  QString localVersion = "000", remoteVersion = "000";
  QString versionFile = locateLocal("appdata", "version");
  QString languageFile = locateLocal("appdata", "languages.ksl", false);

  QFile local(versionFile);
  if(local.exists())
  {
    local.open(IO_ReadOnly);
    QString input;
    local.readLine(input, 5);
    localVersion = input;
  }

  KIO::NetAccess::download("ftp://ftp.berlios.de/pub/kslovar/version", versionFile, this);
  QFile remote(versionFile);
  if(remote.exists())
  {
    remote.open(IO_ReadOnly);
    QString input;
    remote.readLine(input, 5);
    remoteVersion = input;
  }

  if(remoteVersion > localVersion)
  {
    KIO::NetAccess::download("ftp://ftp.berlios.de/pub/kslovar/languages.ksl", languageFile, this);
    loadLanguages();
}*/
  KSUpgradeManager *upgradeManager = new KSUpgradeManager(this, i18n("Upgrade"));
  upgradeManager->show();
}

void KSlovar::slotToggleLiteral()
{
  KSData::instance()->setLiteralSearch(m_literalSearch->isChecked());
}

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
