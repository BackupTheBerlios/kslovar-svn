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

#ifndef _CREATEDICTIONARY_H_
#define _CREATEDICTIONARY_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kdialog.h>

class KToolBar;
class KLineEdit;
class KTextEdit;
class DBHandler;

class CreateDictionary : public KDialog
{
  Q_OBJECT

  public:
    CreateDictionary(QString name=NULL, QString page=NULL);
    
    QString path;
    DBHandler *dictionaryDB;
    
  public slots:
    virtual void slotUnder();
    virtual void slotItalic();
    virtual void slotBold();
    virtual void slotSaveAs();
    virtual void slotFontSize();
    virtual void slotSave();
    virtual void slotChanged();
    
  private:
    KToolBar *toolbar;
    KLineEdit *nameEdit;
    KTextEdit *pageEdit;
    QString fontSize;
    
};

#endif
