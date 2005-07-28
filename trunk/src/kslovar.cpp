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

#include <kmainwindow.h>
#include <klocale.h>

#include <iostream>

using namespace std;

#define TOOLBAR_ID_HOME 0
#define TOOLBAR_ID_BACK 1
#define TOOLBAR_ID_FORWARD 2

KSlovar::KSlovar()
    : KMainWindow( 0, "KSlovar" )
{
  setCaption( "KSlovar" );
  
  
  KPopupMenu * filemenu = new KPopupMenu;
  filemenu->insertItem(KGlobal::iconLoader()->loadIcon("fileopen", KIcon::NoGroup), i18n( "&Open" ), this, SLOT( slotFileOpen() ) );
  filemenu->insertItem(KGlobal::iconLoader()->loadIcon("exit", KIcon::NoGroup), i18n( "&Quit" ), kapp, SLOT( quit() ) );
  
  KPopupMenu *help = helpMenu( );
  
  KMenuBar * menu = menuBar();
  menu->insertItem( i18n( "&File" ), filemenu );
  menu->insertItem( i18n( "&Help" ), help );
  
  KToolBar *toolbar = new KToolBar(this);
  toolbar->insertButton(BarIcon("back"), TOOLBAR_ID_BACK,
                        SIGNAL(clicked(int)),this,SLOT(slotPrevPhrase()),
                        FALSE, i18n("Go to previous phrase"));
  toolbar->insertButton(BarIcon("forward"), TOOLBAR_ID_FORWARD,
                        SIGNAL(clicked(int)),this,SLOT(slotNextPhrase()),
                        FALSE, i18n("Go to next phrase"));
  toolbar->insertButton(BarIcon("gohome"), TOOLBAR_ID_HOME,
                        SIGNAL(clicked(int)),this,SLOT(slotHome()),
                        FALSE, i18n("Go to first page"));
  
  addToolBar(toolbar);
  
  QHBox * horiz = new QHBox( this );
  
  split = new QSplitter( horiz );
  
  QVBox * vert = new QVBox( split );
  
  search = new KLineEdit( vert );
  list = new KListBox( vert );
  
  browser=new KHTMLPart( split );
  browser->setEncoding("utf-8", true);
  browser->begin();
  browser->write("<h1>Welcome message.</h1> Need to change it :P");
  browser->end();
  
  connect( browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this, SLOT( slotShowBrowser(const KURL &, const KParts::URLArgs &) ) );
  
  connect(list, SIGNAL( selected(int) ), this, SLOT( slotShowList() ) );
  connect(search, SIGNAL ( returnPressed(const QString&) ), this, SLOT( slotSearch() ) );
  
  
  setCentralWidget( horiz );
}

void KSlovar::slotFileOpen()
{
  QString path=KFileDialog::getOpenFileName(QString::null, "*", this);
  
  if( !path.isEmpty() )
  {
  
    dictionary = path;
    dictionary.append( ".db" );
    QFile file( path );
    if( file.open(IO_ReadOnly) )
    {
      QTextStream stream( &file );
      QString temp;
      if(stream.readLine() != "[dictionary]")
      {
        KMessageBox::error(this, i18n("This is not a dictionary!") );
        return;
      }
      list->clear();
      phrases=0;
      while( !stream.atEnd() )
      {
        temp=stream.readLine();
        phrases+=temp;
        list->insertItem( temp.remove( QRegExp ("/.+$") ) );
      }
    }
    else
    {
      KMessageBox::error(this, i18n("Couldn't read the dictionary!") );
    }
    file.close();
    dictionaryDB = new DBHandler(dictionary);
    
    toolBar()->setItemEnabled( TOOLBAR_ID_HOME, TRUE);
    slotHome();
  }
}

void KSlovar::slotShow()
{

  QString query1="SELECT text FROM dictionary WHERE id='";
  query1=query1.append(selectedPhrase);
  query1=query1.append("' LIMIT 1;");
  
  QString output=dictionaryDB->query(query1);
  
  browser->begin();
  browser->write(output);
  browser->end();
  
  if(!back.isEmpty())
  {
    toolBar()->setItemEnabled( TOOLBAR_ID_BACK, TRUE);
  }
}

void KSlovar::slotSearch()
{
  list->clear();
  
  for(QStringList::Iterator rezultat = phrases.begin(); rezultat != phrases.end(); rezultat++)
  {
    QString rez = *rezultat;
    if( ( rez.contains( search->text() ) ) )
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
  int& temp = back.first();
  itForward = forward.prepend( selectedPhrase.toInt() );
  selectedPhrase = selectedPhrase.setNum(temp);
  
  it = back.remove(it);
  
  if(back.isEmpty())
  {
    toolBar()->setItemEnabled( TOOLBAR_ID_BACK, FALSE);
  }
  toolBar()->setItemEnabled( TOOLBAR_ID_FORWARD, TRUE);
  
  slotShow();
}

void KSlovar::slotNextPhrase()
{
  int& temp = forward.first();
  
  addHistory();
  
  selectedPhrase = selectedPhrase.setNum(temp);
  
  itForward = forward.remove(itForward);
  
  if(forward.isEmpty())
  {
    toolBar()->setItemEnabled( TOOLBAR_ID_FORWARD, FALSE);
  }
  
  slotShow();
}

void KSlovar::slotHome()
{
  addHistory();
  
  selectedPhrase = selectedPhrase.setNum(0);
  
  slotShow();
}

void KSlovar::addHistory()
{
  if(!selectedPhrase.isEmpty())
  {
    it = back.prepend( selectedPhrase.toInt() );
  }
}

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
