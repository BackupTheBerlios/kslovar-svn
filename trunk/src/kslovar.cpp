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
#include "objects/configuration.h"

#include "widgets/createdictionary.h"
#include "widgets/addphrase.h"

#include "ui/appearancewdt.h"

#include "dbhandler.h"
#include "ksxmlhandler.h"
#include "objects/ksdata.h"

#include "objects/kslistview.h"
#include "objects/kslistviewitem.h"
#include "objects/kslistviewsearchline.h"
#include "objects/instances.h"

#include <qvbox.h>
#include <qhbox.h>

#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kapp.h>
#include <klineedit.h>
#include <khtml_part.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kparts/browserextension.h>
#include <kshortcut.h>
#include <kaction.h>
#include <khtmlview.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>


KSlovar *KSlovar::m_instance=0L;

KSlovar::KSlovar()
    : KMainWindow( 0, "KSlovar" )
{
  Instances::setMainInstance(this);
  m_configDialog=new KConfigDialog(this, "settings", Configuration::self());
  Instances::setConfigInstance(m_configDialog);
  XMLParser=new KSXMLHandler(QString::fromUtf8(locate("appdata", "styles/default.xsl")));
  loadLanguages();

  m_welcomeMessage=i18n("<h1>Welcome to KSlovarju</h1> This needs to be changed :).");

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

  setCentralWidget( horiz );
}

void KSlovar::slotFileOpen()
{
  processFileOpen(KFileDialog::getOpenFileName("~", "*.ksd|KSlovar dictionary file", this));
}

void KSlovar::showDictionary()
{/*
  m_selected=false;

  QString output=DBHandler::Instance(m_path)->readText(m_selectedPhrase);
  m_currentText=output;

  m_browser->begin();
  m_browser->write(output);
  m_browser->end();

  if(!m_backHistory.isEmpty())
  {
    m_back->setEnabled(true);
  }
 */

  m_history=false;
  m_browser->begin();
  //m_browser->write(DBHandler::Instance(m_path)->readText(m_selectedPhrase));
  //m_browser->write(DBHandler::instance(m_path)->processString("SELECT text FROM dictionary WHERE id='"+m_selectedPhrase+"' LIMIT 1;"));
  m_browser->write(XMLParser->parse(DBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+m_selectedPhrase+"' LIMIT 1;")));
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
{/*ULD
  if(m_selected==true)
  {
    return;
  }
  addHistory();

  QString result;
//  QStringList list1 = m_phrases.grep( m_list->currentText() );

/*  for(QStringList::Iterator phrase = list1.begin(); phrase != list1.end(); phrase++)
  {
    QString search=result = *phrase;
    if(search.remove(QRegExp("/.+$")).compare(m_list->currentText())==0)
    {
      break;
    }
  }
  /////////  *
  //QString text=result;
  //QString backup=text;

  /*text.remove(QRegExp (".+/"));
  if(text!="0")
  {
    m_selectedPhrase=text;
    m_userDictionary=true;
  }
  else
  {*
  m_selectedPhrase=result.remove(QRegExp("/\\D.+$")).remove(QRegExp("^\\w+/"));
  //m_userDictionary=false;
  //}
  showDictionary();
  */

  m_editPhrase->setEnabled(true);

  if(m_history==false)
  {
    addHistory();
  }

  m_selectedPhrase=static_cast<KSListViewItem*> (selected)->getId();
  showDictionary();
}

void KSlovar::slotShowBrowser(const KURL &url, const KParts::URLArgs &)
{
  //addHistory();

  //int temp = url.host().toInt();
  //QString temp1;

  //m_selectedPhrase=url.host();

  //m_selected=true;
//  m_list->setSelected(m_selectedPhrase.toInt(), true);

  /*for(QStringList::Iterator phrase = phrases.begin(); phrase != phrases.end(); phrase++)
  {
    QString search=temp1 = *phrase;
    if(search.remove(QRegExp("/\\D.+$")).remove(QRegExp("^\\w+/")).toInt()==temp)
    {
      temp1.remove(QRegExp (".+/"));
      if(temp1!="0")
      {
        m_selectedPhrase=temp1;
        m_userDictionary=true;
      }
      else
      {
        m_selectedPhrase.setNum(temp);
        m_userDictionary=false;
      }
      break;
    }
}*/

  //showDictionary();

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
  //bool temp1=m_backHistory.first().user();
  //m_itForward = m_forwardHistory.prepend( history(m_selectedPhrase.toInt(), m_userDictionary) );
  m_itForward = m_forwardHistory.prepend( m_selectedPhrase.toInt() );
  //m_selectedPhrase = m_selectedPhrase.setNum(temp);
  //m_userDictionary=temp1;

//  m_selected=true;
//  m_list->setSelected(m_selectedPhrase.toInt(), true);

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

  //showDictionary();
}

void KSlovar::slotNextPhrase()
{
  int temp = m_forwardHistory.first();
  //bool temp1= m_forwardHistory.first().user();

  addHistory(false);

  //m_selectedPhrase = m_selectedPhrase.setNum(temp);
  //m_userDictionary=temp1;

  //m_selected=true;
//  m_list->setSelected(m_selectedPhrase.toInt(), true);

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

  //showDictionary();
}

void KSlovar::slotHome()
{
  if(m_selectedPhrase=="0")
  {
    return;
  }

//  m_list->setSelected(m_selectedPhrase.toInt(), false);

  addHistory();

  m_selectedPhrase.setNum(0);
  m_list->clearSelection();
  m_editPhrase->setEnabled(false);
  //m_userDictionary=false;

  showDictionary();
}

void KSlovar::addHistory(bool deleteForward)
{
  if(!m_selectedPhrase.isEmpty())
  {
    //m_it = m_backHistory.prepend( history(m_selectedPhrase.toInt(), m_userDictionary) );
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
  m_dictionarydlg = new CreateDictionary(this);
  m_dictionarydlg->resize(700, 700);
  m_dictionarydlg->show();
}

void KSlovar::slotEditDictionary()
{
  //QString text=DBHandler::Instance(m_path)->readText(QString("0"));
  QString text=DBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='0' LIMIT 1;");
  QString name=text;

  text.remove(QRegExp("<h1>.+</h1>"));
  name.remove(text).remove("<h1>").remove("</h1>");

  m_dictionarydlg = new CreateDictionary(this, "Edit Dictionary", name, text, true);
//  m_dictionarydlg->setPath(m_path);
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

  m_find=KStdAction::find(this, SLOT(slotFind()), actionCollection());
  m_findNext=KStdAction::findNext(this, SLOT(slotFindNext()), actionCollection());
  m_print=KStdAction::print(this, SLOT(slotPrint()), actionCollection());
  m_selectAll=KStdAction::selectAll(this, SLOT(slotSelectAll()), actionCollection());

  m_addPhrase=new KAction(i18n("&Add phrase"), "filenew", KShortcut(KKey("CTRL+a")), this, SLOT(slotAddPhrase()), actionCollection(), "addPhrase");
  m_editPhrase=new KAction(i18n("Edi&t phrase"), "edit", KShortcut(KKey("CTRL+t")), this, SLOT(slotEditPhrase()), actionCollection(), "editPhrase");
  m_removePhrase=new KAction(i18n("&Remove phrase"), "editdelete", KShortcut(KKey("CTRL+r")), this, SLOT(slotRemovePhrase()), actionCollection(), "removePhrase");

  m_config=KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
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

  KPopupMenu *gomenu=new KPopupMenu;
  m_back->plug(gomenu);
  m_forward->plug(gomenu);
  m_home->plug(gomenu);

  KPopupMenu *setmenu=new KPopupMenu;
  m_config->plug(setmenu);

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
  m_phrases.clear();
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
//  quit();
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
  m_phrasedlg=new AddPhrase(this, "Add word");
  //m_phrasedlg->setPath(m_path);
  m_phrasedlg->resize(700, 700);
  m_phrasedlg->show();
}

void KSlovar::slotEditPhrase()
{
  if(!m_editPhrase->isEnabled())
  {
    return;
  }
  //QString output=DBHandler::Instance(m_path)->readText(m_selectedPhrase);
  QString output=DBHandler::instance(KSData::instance()->getDictionaryPath())->processString("SELECT text FROM dictionary WHERE id='"+m_selectedPhrase+"' LIMIT 1;");
  m_phrasedlg=new AddPhrase(this, "Edit word");
  m_phrasedlg->setWord(output, m_selectedPhrase);
  //m_phrasedlg->setPath(m_path);
  m_phrasedlg->resize(700, 700);
  m_phrasedlg->show();
}

void KSlovar::slotRemovePhrase()
{
}

QStringList KSlovar::getPhrases()
{
  return m_phrases;
}

void KSlovar::slotConfigure()
{
  m_configDialog->addPage(new AppearanceWdt(0), i18n("Appearance"), "looknfeel");

  //connect( m_configDialog, SIGNAL(settingsChanged()), this, SLOT(slotUpdateConfiguration()) );

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
  if( !fileName.isEmpty() )
  {

    /*progress = new KProgressDialog(this, "Progress", i18n("Reading index"), i18n("Please wait..."));
    progress->setAutoClose(true);
    progress->showCancelButton(false);
    progress->setMinimumDuration(0);
    progressBar = progress->progressBar();*/

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

    //int steps=0;
    //int step=0;

    //m_phrases = DBHandler::Instance(m_path)->readIndex(0L);
    m_phrases=DBHandler::instance(fileName)->processList("SELECT name, id, search FROM phrases ORDER BY search ASC;", 3);
    //progressBar->setTotalSteps(steps);


    for(QStringList::iterator phrase = m_phrases.begin(); phrase != m_phrases.end(); phrase++)
    {
      //step++;
      QString word = *phrase;
      QString id= *phrase;
      QString search= *phrase;
      new KSListViewItem(m_list, word.remove(QRegExp("/.+$")), id.remove(QRegExp("[^/\\d]+")).remove(QRegExp("^\\d+")).remove(QRegExp("\\d+$")).remove(QRegExp("/+")), search.remove(QRegExp("^.+/")));
      //progressBar->setProgress(step);
    }
    //progressBar->setProgress(steps);

    m_home->setEnabled(true);
    //m_back->setEnabled(false);
    //m_forward->setEnabled(false);
    m_editDictionary->setEnabled(true);
    m_close->setEnabled(true);
    m_addPhrase->setEnabled(true);
    //m_editPhrase->setEnabled(true);

    slotHome();
    m_backHistory.clear();
    KSData::instance()->setDictionaryPath(fileName);
    loadPartOfSpeech(DBHandler::instance(fileName)->processString("SELECT lang FROM head;").toInt());
  }
}

void KSlovar::loadLanguages()
{
  QStringList input=DBHandler::instance(QString::fromUtf8(locate("appdata", "languages.ksl")))->processList("SELECT id, name FROM language;", 2);
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
  else
  {
    KMessageBox::error(this, i18n("Cannot load languages!"));
  }
}

void KSlovar::loadPartOfSpeech(int id)
{
  KSData::instance()->clearPartOfSpeech();
  KSData::instance()->setLanguage(id);
  QStringList input=DBHandler::instance(QString::fromUtf8(locate("appdata", "languages.ksl")))->processList("SELECT id, name FROM type WHERE id_lang='"+QString::number(id)+"';", 2);
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
  else
  {
    KMessageBox::error(this, i18n("Cannot load parts of speech!"));
  }
}

void KSlovar::refresh()
{
  showDictionary();
}

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
