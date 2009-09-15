#include "linboInfoBrowserImpl.hh"
#include <qapplication.h>
#include <QtGui>
#include <q3textstream.h>

linboInfoBrowserImpl::linboInfoBrowserImpl(QWidget* parent ) : linboDialog()
{
   Ui_linboInfoBrowser::setupUi((QDialog*)this);

   myProcess = new Q3Process( this );
   connect( this->saveButton, SIGNAL(clicked()), this, SLOT(postcmd()));

   connect( myProcess, SIGNAL(readyReadStdout()),
            this, SLOT(readFromStdout()) );

   connect( myProcess, SIGNAL(readyReadStderr()),
            this, SLOT(readFromStderr()) );  
}

linboInfoBrowserImpl::~linboInfoBrowserImpl()
{
  delete myProcess;
} 

void linboInfoBrowserImpl::setTextBrowser( Q3TextBrowser* newBrowser )
{
  Console = newBrowser;
}

void linboInfoBrowserImpl::setMainApp( QWidget* newMainApp ) {
  if ( newMainApp ) {
    myMainApp = newMainApp;
  }
}


void linboInfoBrowserImpl::precmd() {
  app = static_cast<linboGUIImpl*>( myMainApp );
  
  if( app ) {
    if ( app->isRoot() ) {
      saveButton->setText("Speichern");
      saveButton->setEnabled( true );
      editor->setReadOnly( false );
      // connect( this->saveButton, SIGNAL(clicked()), this, SLOT(postcmd()));
    } else {
      saveButton->setText("Schliessen");
      saveButton->setEnabled( true );
      editor->setReadOnly( true );
      // connect( this->saveButton, SIGNAL(clicked()), this, SLOT(close()));
    }

    myProcess->clearArguments();
    myProcess->setArguments( myLoadCommand );

#ifdef DEBUG
    Console->append(QString("linboInfoBrowserImpl: myLoadCommand"));
    QStringList list = myProcess->arguments();
    QStringList::Iterator it = list.begin();
    while( it != list.end() ) {
      Console->append( *it );
      ++it;
    }
    Console->append(QString("*****"));
#endif

    if( myProcess->start() ) {
      while( myProcess->isRunning() ) {
        usleep( 1000 );
      }
    } else {
      Console->append("myLoadCommand didn't start");
    }

    file = new QFile( filepath );
    // read content
    if( !file->open( QIODevice::ReadOnly ) ) {
      Console->append("Keine passende Beschreibung im Cache.");
    } 
    else {
      Q3TextStream ts( file );
      editor->setText( ts.read() );
      file->close();
    }
  }
  
}


void linboInfoBrowserImpl::postcmd() {
  
  if( app ) {
    if ( app->isRoot() ) {

      if ( !file->open( QIODevice::WriteOnly ) ) {
        Console->append("Fehler beim Speichern der Beschreibung.");
      } 
      else {
        Q3TextStream ts( file );
        ts << editor->text();
        file->flush();
        file->close();

        myProcess->clearArguments();
        myProcess->setArguments( mySaveCommand ); 

#ifdef DEBUG
        Console->append(QString("linboInfoBrowserImpl: mySaveCommand"));
        QStringList list = myProcess->arguments();
        QStringList::Iterator it = list.begin();
      
        while( it != list.end() ) {
          Console->append( *it );
          ++it;
        }
        Console->append(QString("*****"));
#endif

        if( myProcess->start() ) {
          while( myProcess->isRunning() ) {
            usleep( 1000 );
          }
        } else {
          Console->append("mySaveCommand didn't start");
        }
      
        myProcess->clearArguments();
        myProcess->setArguments( myUploadCommand );

#ifdef DEBUG
        Console->append(QString("linboInfoBrowserImpl: myUploadCommand"));
        list = myProcess->arguments();
        it = list.begin();
      
        while( it != list.end() ) {
          Console->append( *it );
          ++it;
        }
        Console->append(QString("*****"));
#endif

        if( myProcess->start() ) {
          while( myProcess->isRunning() ) {
            usleep( 1000 );
          }
        } else {
          Console->append("myUploadCommand didn't start");
        }


      }
    }
    this->close();
  }
  
}

void linboInfoBrowserImpl::setCommand( const QStringList& newArguments ) {
  myUploadCommand = newArguments;
}

void linboInfoBrowserImpl::setUploadCommand( const QStringList& newUploadCommand ) {
  myUploadCommand = newUploadCommand;
}

void linboInfoBrowserImpl::setLoadCommand( const QStringList& newLoadCommand ) {
  myLoadCommand = newLoadCommand;
}

void linboInfoBrowserImpl::setSaveCommand( const QStringList& newSaveCommand ) {
  mySaveCommand = newSaveCommand;
}

QStringList linboInfoBrowserImpl::getCommand() {
  // not needed here
  return myUploadCommand;
}

void linboInfoBrowserImpl::setFilePath( const QString& newFilepath ) {
  filepath = newFilepath;
}

void linboInfoBrowserImpl::readFromStdout()
{
  while( myProcess->canReadLineStdout() )
    {
      line = myProcess->readLineStdout();
      Console->append( line );
    } 
}

void linboInfoBrowserImpl::readFromStderr()
{
  while( myProcess->canReadLineStderr() )
    {
      line = myProcess->readLineStderr();
      line.prepend( "<FONT COLOR=red>" );
      line.append( "</FONT>" );
      Console->append( line );
    } 

}