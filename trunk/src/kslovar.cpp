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
#include <qsqldatabase.h>
#include <iostream>

using namespace std;

#include <kmainwindow.h>
#include <klocale.h>

KSlovar::KSlovar()
    : KMainWindow( 0, "KSlovar" )
{
  setCaption( "KSlovar" );
  
  
  KPopupMenu * filemenu = new KPopupMenu;
  filemenu->insertItem(KGlobal::iconLoader()->loadIcon("fileopen", KIcon::NoGroup), i18n( "&Open" ), this, SLOT( slotFileOpen() ) );
  filemenu->insertItem(KGlobal::iconLoader()->loadIcon("exit", KIcon::NoGroup), i18n( "&Quit" ), kapp, SLOT( quit() ) );
  
  KPopupMenu *help = helpMenu( );
  
  KMenuBar * meni = menuBar();
  meni->insertItem( i18n( "&File" ), filemenu );
  meni->insertItem( i18n( "&Help" ), help );
  
  QHBox * horiz = new QHBox( this );
  
  split = new QSplitter( horiz );
  split->setOpaqueResize();
  
  QVBox * vert = new QVBox( split );
  
  search = new KLineEdit( vert );
  list = new KListBox( vert );
  
  browser=new KHTMLPart( split );
  browser->setEncoding("utf-8", true);
  browser->begin();
  browser->write("<h1>Welcome message.</h1> Need to change it :P");
  browser->end();
  
//  connect( browser->browserExtension(), SIGNAL( openURLRequest( const KURL &, const KParts::URLArgs & ) ), this, SLOT( openURLRequest(const KURL &, const KParts::URLArgs & ) ) );
  
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
  }
}

void KSlovar::slotShow()
{
  QSqlDatabase *defaultDB = QSqlDatabase::addDatabase( "QSQLITE" );
  defaultDB->setDatabaseName( dictionary );
  
  QString query="SELECT text FROM dictionary WHERE id='";
  query=query.append(selectedDictionary);
  query=query.append("' LIMIT 1;");
  
  defaultDB->open();
  
  QString error=defaultDB->lastError().text();
  /*if(  )
  {*/
  QSqlQuery q(query, defaultDB);
  if( q.next() )
  {
    
    browser->begin();
    browser->write( q.value(0).toString() );
    browser->setEncoding("utf-8", true);
    browser->end();
  }
  //}
}

/*void KSlovar::slotShow()
{
  QDomDocument d;
  QFile file( dictionary );
  if ( !file.open( IO_ReadOnly ) )
  {
    KMessageBox::error(this, i18n("Couldn't read the dictionary!") );
    return;
  }
  if ( !d.setContent( &file ) )
  {
    file.close();
    KMessageBox::error(this, i18n("This is not a dictionary!") );
    return;
  }
  file.close();
  
  QDomElement dElem = d.documentElement();
  
  for(QDomNode n = dElem.firstChild(); !n.isNull(); n = n.nextSibling())
  {
    if( n.isElement() )
    {
      QDomElement e = n.toElement();
      if( ( e.tagName() == "index" ) && ( e.text() == selectedDictionary ) )
      {
        n = n.nextSibling();
        e = n.toElement();
        
        QString text = e.text();
        
        for(n = e.firstChild(); !n.isNull(); n = n.nextSibling())
        {
          if( n.isElement() )
          {
            e = n.toElement();
            QString formated = "<";
            formated = formated.append( e.tagName() );
            formated = formated.append(">");
            formated = formated.append( e.text() );
            formated = formated.append("</");
            formated = formated.append( e.tagName() );
            formated = formated.append(">");
            text = text.replace(e.text(), formated);
          }
        }
        
        browser->begin();
        browser->write( text );
        browser->end();
        
        break;
      }
    }
  }
  
  
}*/

void KSlovar::slotSearch()
{
  list->clear();
  //QStringList rezultati = phrases.grep( search->text() );
  
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
  
  QStringList list1 = phrases.grep( list->currentText() );
  
  QString text=list1.join(0L).remove( QRegExp ("/\\D.+$") );
  selectedDictionary = text.remove( QRegExp ("^.+\\D/") );
  
  slotShow();
}
///@karkol... To sploh ne dela... :P
/*void KSlovar::openURLRequest(const KURL &url, const KParts::URLArgs & )
{
  selectedDictionary = url.url();
  
  slotShow();
}*/

KSlovar::~KSlovar()
{
}

#include "kslovar.moc"
