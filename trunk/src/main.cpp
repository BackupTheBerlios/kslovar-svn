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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("A Dictionary");

static const char version[] = "0.0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kslovar", I18N_NOOP("KSlovar"), version, description,
		     KAboutData::License_GPL, "(C) 2005 Gregor Kališnik", 0, 0, "gregor@podnapisi.net");
    about.addAuthor( "Gregor Kališnik", 0, "gregor@podnapisi.net" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    KSlovar *mainWin = 0;

    if (app.isRestored())
    {
        RESTORE(KSlovar);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here

        mainWin = new KSlovar();
        app.setMainWidget( mainWin );
        mainWin->show();
        mainWin->resize(1000, 800);

        args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

