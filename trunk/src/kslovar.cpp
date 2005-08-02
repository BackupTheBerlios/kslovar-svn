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
  
  m_split = new QSplitter( horiz );
  
  QVBox * vert = new QVBox( m_split );
  vert->setMaximumWidth(200);
  
  m_search = new KLineEdit( vert );
  m_list = new KListBox( vert );
  
  m_browser=new KHTMLPart( m_split );
  m_browser->setEncoding("utf-8", true);
  m_browser->begin();
  m_browser->write("<h1>Welcome message.</h1> Need to change it :P");
  m_browser->end();
  
  connect( m_browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this, SLOT( slotShowBrowser(const KURL &, const KParts::URLArgs &) ) );
  
  connect(m_list, SIGNAL( selected(int) ), this, SLOT( slotShowList() ) );
  connect(m_search, SIGNAL ( textChanged ( const QString & ) ), this, SLOT( slotSearch( const QString & ) ) );
  
  
  setCentralWidget( horiz );
}

void KSlovar::slotFileOpen()
{
  m_path=KFileDialog::getOpenFileName(QString::null, "*.db", this);
  
  if( !m_path.isEmpty() )
  {
    
    /*progress = new KProgressDialog(this, "Progress", i18n("Reading index"), i18n("Please wait..."));
    progress->setAutoClose(true);
    progress->showCancelButton(false);
    progress->setMinimumDuration(0);
    progressBar = progress->progressBar();*/
  
    QFile file( m_path );
    if( file.open(IO_ReadOnly) )
    {
      QString temp;
      file.readLine(temp, 50);
      if( !temp.startsWith("SQLite format 3") )
      {
        KMessageBox::error(this, i18n("This is not a valid dictionary!") );
        return;
      }
      m_list->clear();
    }
    else
    {
      KMessageBox::error(this, i18n("Couldn't read the dictionary!") );
      return;
    }
    file.close();
    
    int steps=0;
    int step=0;
    
    phrases = DBHandler::Instance(m_path)->readIndex(&steps);
    //progressBar->setTotalSteps(steps);
    
    
    for(QStringList::Iterator phrase = phrases.begin(); phrase != phrases.end(); phrase++)
    {
      step++;
      QString rez = *phrase;
      m_list->insertItem(rez.remove( QRegExp ("/.+$") ));
      //progressBar->setProgress(step);
    }
    //progressBar->setProgress(steps);

    m_home->setEnabled(true);
    m_back->setEnabled(false);
    m_forward->setEnabled(false);
    m_editDictionary->setEnabled(true);
    slotHome();
    m_backHistory.clear();
  }
}

void KSlovar::slotShow()
{

  QString output=DBHandler::Instance(m_path)->readText(m_selectedPhrase);
  
  m_browser->begin();
  m_browser->write(output);
  m_browser->end();
  
  if(!m_backHistory.isEmpty())
  {
    m_back->setEnabled(true);
  }
}

void KSlovar::slotSearch(const QString &text)
{
  m_list->clear();
  
  for(QStringList::Iterator rezultat = phrases.begin(); rezultat != phrases.end(); rezultat++)
  {
    QString rez = *rezultat;
    if( ( rez.contains( text ) ) )
    {
      m_list->insertItem(rez.remove( QRegExp ("/.+$") ));
    }
  }
}

void KSlovar::slotShowList()
{
  addHistory();
  
  QStringList list1 = phrases.grep( m_list->currentText() );
  
  QString text=list1.join(0L).remove( QRegExp ("/\\D.+$") );
  m_selectedPhrase = text.remove( QRegExp ("^.+\\D/") );
  
  slotShow();
}

void KSlovar::slotShowBrowser(const KURL &url, const KParts::URLArgs &)
{
  addHistory();
  
  m_selectedPhrase = url.host();
  
  slotShow();
}

void KSlovar::slotPrevPhrase()
{
  int& temp = m_backHistory.first();
  m_itForward = m_forwardHistory.prepend( m_selectedPhrase.toInt() );
  m_selectedPhrase = m_selectedPhrase.setNum(temp);
  
  m_it = m_backHistory.remove(m_it);
  
  if(m_backHistory.isEmpty())
  {
    m_back->setEnabled(false);
  }
  m_forward->setEnabled(true);
  
  slotShow();
}

void KSlovar::slotNextPhrase()
{
  int& temp = m_forwardHistory.first();
  
  addHistory(FALSE);
  
  m_selectedPhrase = m_selectedPhrase.setNum(temp);
  
  m_itForward = m_forwardHistory.remove(m_itForward);
  
  if(m_forwardHistory.isEmpty())
  {
    m_forward->setEnabled(false);
  }
  
  slotShow();
}

void KSlovar::slotHome()
{
  addHistory();
  
  m_selectedPhrase = m_selectedPhrase.setNum(0);
  
  slotShow();
}

void KSlovar::addHistory(bool deleteForward)
{
  if(!m_selectedPhrase.isEmpty())
  {
    m_it = m_backHistory.prepend( m_selectedPhrase.toInt() );
  }
  
  if(deleteForward)
  {
    m_forwardHistory.clear();
    m_forward->setEnabled(false);
  }
}

void KSlovar::slotNewDictionary()
{
  m_dictionarydlg = new CreateDictionary();
  m_dictionarydlg->show();
  m_dictionarydlg->resize(700, 700);
}

void KSlovar::slotEditDictionary()
{
  QString text=DBHandler::Instance(m_path)->readText(QString("0"));
  QString name=text;
  
  text.remove(QRegExp("<h1>.+</h1>"));
  name.remove(text).remove("<h1>").remove("</h1>");
  
  m_dictionarydlg = new CreateDictionary(name, text);
  m_dictionarydlg->setPath(m_path);
  m_dictionarydlg->show();
  m_dictionarydlg->resize(700, 700);
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
}

void KSlovar::addMenu()
{
  KPopupMenu * filemenu = new KPopupMenu;
  m_newDictionary->plug(filemenu);
  m_openDictionary->plug(filemenu);
  filemenu->insertSeparator();
  m_quit->plug(filemenu);
  
  KPopupMenu *editmenu=new KPopupMenu;
  m_editDictionary->plug(editmenu);
  
  KPopupMenu *help = helpMenu( );
  
  KMenuBar * menu = menuBar();
  menu->insertItem( i18n( "&File" ), filemenu );
  menu->insertItem(i18n("&Edit"), editmenu);
  menu->insertItem( i18n( "&Help" ), help );
}

void KSlovar::addToolbar()
{
  KToolBar *toolbar = new KToolBar(this);
  m_back->plug(toolbar);
  m_forward->plug(toolbar);
  m_home->plug(toolbar);
  
  addToolBar(toolbar);
}

void KSlovar::disableNavButtons()
{
  m_back->setEnabled(false);
  m_forward->setEnabled(false);
  m_home->setEnabled(false);
  m_editDictionary->setEnabled(false);
}

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
