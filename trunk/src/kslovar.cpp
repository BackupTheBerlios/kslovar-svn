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


#include "kslovar.h"
#include "dbhandler.h"
#include "createdictionary.h"

#include <kpopupmenu.h>
#include <kmenubar.h>
#include <kapp.h>
#include <qvbox.h>
#include <qhbox.h>
#include <klineedit.h>
#include <klistbox.h>
#include <khtml_part.h>
#include <kurl.h>
#include <qsplitter.h>
#include <kfiledialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <kiconloader.h>
#include <qstringlist.h>
#include <kmessagebox.h>
#include <kparts/browserextension.h>
#include <kprogress.h>
#include <kshortcut.h>
#include <kaction.h>

#include <kdebug.h>

#include <kmainwindow.h>
#include <klocale.h>

KSlovar::KSlovar()
    : KMainWindow( 0, "KSlovar" )
{
  setCaption( "KSlovar" );
  
  registerButtons();
  addMenu();
  addToolbar();
  disableNavButtons();
  
  QHBox * horiz = new QHBox( this );
  
  split = new QSplitter( horiz );
  
  QVBox * vert = new QVBox( split );
  vert->setMaximumWidth(200);
  
  search = new KLineEdit( vert );
  list = new KListBox( vert );
  
  browser=new KHTMLPart( split );
  browser->setEncoding("utf-8", true);
  browser->begin();
  browser->write("<h1>Welcome message.</h1> Need to change it :P");
  browser->end();
  
  connect( browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this, SLOT( slotShowBrowser(const KURL &, const KParts::URLArgs &) ) );
  
  connect(list, SIGNAL( selected(int) ), this, SLOT( slotShowList() ) );
  connect(search, SIGNAL ( textChanged ( const QString & ) ), this, SLOT( slotSearch( const QString & ) ) );
  
  
  setCentralWidget( horiz );
}

void KSlovar::slotFileOpen()
{
  path=KFileDialog::getOpenFileName(QString::null, "*.db", this);
  
  if( !path.isEmpty() )
  {
    
    /*progress = new KProgressDialog(this, "Progress", i18n("Reading index"), i18n("Please wait..."));
    progress->setAutoClose(true);
    progress->showCancelButton(false);
    progress->setMinimumDuration(0);
    progressBar = progress->progressBar();*/
  
    /*dictionary = path;
    dictionary.append( ".db" );*/
    QFile file( path );
    if( file.open(IO_ReadOnly) )
    {
      QString temp;
      file.readLine(temp, 50);
      if( !temp.startsWith("SQLite format 3") )
      {
        KMessageBox::error(this, i18n("This is not a valid dictionary!") );
        return;
      }
      list->clear();
    }
    else
    {
      KMessageBox::error(this, i18n("Couldn't read the dictionary!") );
      return;
    }
    file.close();
    dictionaryDB = new DBHandler(path);
    
    int steps=0;
    int step=0;
    
    phrases = dictionaryDB->readIndex(&steps);
    //progressBar->setTotalSteps(steps);
    
    
    for(QStringList::Iterator phrase = phrases.begin(); phrase != phrases.end(); phrase++)
    {
      step++;
      QString rez = *phrase;
      list->insertItem(rez.remove( QRegExp ("/.+$") ));
      //progressBar->setProgress(step);
    }
    //progressBar->setProgress(steps);

    home->setEnabled(true);
    back->setEnabled(false);
    forward->setEnabled(false);
    editDictionary->setEnabled(true);
    slotHome();
    backHistory.clear();
  }
}

void KSlovar::slotShow()
{

  QString output=dictionaryDB->readText(selectedPhrase);
  
  browser->begin();
  browser->write(output);
  browser->end();
  
  if(!backHistory.isEmpty())
  {
    //toolBar()->setItemEnabled( TOOLBAR_ID_BACK, TRUE);
    back->setEnabled(true);
  }
}

void KSlovar::slotSearch(const QString &text)
{
  list->clear();
  
  for(QStringList::Iterator rezultat = phrases.begin(); rezultat != phrases.end(); rezultat++)
  {
    QString rez = *rezultat;
    if( ( rez.contains( text ) ) )
    {
      list->insertItem(rez.remove( QRegExp ("/.+$") ));
    }
  }
}

void KSlovar::slotShowList()
{
  addHistory();
  
  QStringList list1 = phrases.grep( list->currentText() );
  
  QString text=list1.join(0L).remove( QRegExp ("/\\D.+$") );
  selectedPhrase = text.remove( QRegExp ("^.+\\D/") );
  
  slotShow();
}

void KSlovar::slotShowBrowser(const KURL &url, const KParts::URLArgs &)
{
  addHistory();
  
  selectedPhrase = url.host();
  
  slotShow();
}

void KSlovar::slotPrevPhrase()
{
  int& temp = backHistory.first();
  itForward = forwardHistory.prepend( selectedPhrase.toInt() );
  selectedPhrase = selectedPhrase.setNum(temp);
  
  it = backHistory.remove(it);
  
  if(backHistory.isEmpty())
  {
    //toolBar()->setItemEnabled( TOOLBAR_ID_BACK, FALSE);
    back->setEnabled(false);
  }
  //toolBar()->setItemEnabled( TOOLBAR_ID_FORWARD, TRUE);
  forward->setEnabled(true);
  
  slotShow();
}

void KSlovar::slotNextPhrase()
{
  int& temp = forwardHistory.first();
  
  addHistory(FALSE);
  
  selectedPhrase = selectedPhrase.setNum(temp);
  
  itForward = forwardHistory.remove(itForward);
  
  if(forwardHistory.isEmpty())
  {
    //toolBar()->setItemEnabled( TOOLBAR_ID_FORWARD, FALSE);
    forward->setEnabled(false);
  }
  
  slotShow();
}

void KSlovar::slotHome()
{
  addHistory();
  
  selectedPhrase = selectedPhrase.setNum(0);
  
  slotShow();
}

void KSlovar::addHistory(bool deleteForward)
{
  if(!selectedPhrase.isEmpty())
  {
    it = backHistory.prepend( selectedPhrase.toInt() );
  }
  
  if(deleteForward)
  {
    forwardHistory.clear();
    //toolBar()->setItemEnabled( TOOLBAR_ID_FORWARD, FALSE);
    forward->setEnabled(false);
  }
}

void KSlovar::slotNewDictionary()
{
  dictionarydlg = new CreateDictionary();
  dictionarydlg->show();
  dictionarydlg->resize(700, 700);
}

void KSlovar::slotEditDictionary()
{
  QString text=dictionaryDB->readText(QString("0"));
  QString name=text;
  
  text.remove(QRegExp("<h1>.+</h1>"));
  name.remove(text).remove("<h1>").remove("</h1>");
  
  dictionarydlg = new CreateDictionary(name, text);
  dictionarydlg->path=path;
  dictionarydlg->dictionaryDB=dictionaryDB;
  dictionarydlg->show();
  dictionarydlg->resize(700, 700);
}

void KSlovar::registerButtons()
{
  newDictionary = new KAction(i18n("&New dictionary"), "filenew", KShortcut(KKey("CTRL+n")), this, SLOT(slotNewDictionary()), actionCollection(), "newDictionary");
  openDictionary = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  quit = KStdAction::quit(kapp, SLOT(quit()), actionCollection());
  
  back=KStdAction::back(this, SLOT(slotPrevPhrase()), actionCollection());
  forward=KStdAction::forward(this, SLOT(slotNextPhrase()), actionCollection());
  home=KStdAction::home(this, SLOT(slotHome()), actionCollection());
  
  editDictionary = new KAction(i18n("&Edit dictionary"), "edit", KShortcut(KKey("CTRL+e")), this, SLOT(slotEditDictionary()), actionCollection(), "editDictionary");
}

void KSlovar::addMenu()
{
  KPopupMenu * filemenu = new KPopupMenu;
  newDictionary->plug(filemenu);
  openDictionary->plug(filemenu);
  filemenu->insertSeparator();
  quit->plug(filemenu);
  
  KPopupMenu *editmenu=new KPopupMenu;
  editDictionary->plug(editmenu);
  
  KPopupMenu *help = helpMenu( );
  
  KMenuBar * menu = menuBar();
  menu->insertItem( i18n( "&File" ), filemenu );
  menu->insertItem(i18n("&Edit"), editmenu);
  menu->insertItem( i18n( "&Help" ), help );
}

void KSlovar::addToolbar()
{
  KToolBar *toolbar = new KToolBar(this);
  back->plug(toolbar);
  forward->plug(toolbar);
  home->plug(toolbar);
  
  addToolBar(toolbar);
}

void KSlovar::disableNavButtons()
{
  back->setEnabled(false);
  forward->setEnabled(false);
  home->setEnabled(false);
  editDictionary->setEnabled(false);
}

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
