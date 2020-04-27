#include "ObelixUdpReaderThread.h"

ObelixUdpReaderThread::ObelixUdpReaderThread(QObject *parent) :
  QThread(parent)
{
  mRun      = false;
  mLastNb   = 0;
  //
  mIp = "";
  mPort = 0;
}

ObelixUdpReaderThread::~ObelixUdpReaderThread()
{
}

void ObelixUdpReaderThread::SetConnexionParameters(QString pIp, uint pPort)
{
  mIp = pIp;
  mPort = pPort;
}

void ObelixUdpReaderThread::SetFifoParameter(char *pMessageFifo,
                                       uint *pFifoIndex,
                                       uint pFifoSize,
                                       uint pMessageSize,
                                       QReadWriteLock *pRwlocker)
{
  mMessageSize = pMessageSize;
  //
  mFifoIndex = pFifoIndex;
  mFifoSize  = pFifoSize;
  //
  mMessageFifo = pMessageFifo;
  mRwlocker    = pRwlocker;
}



void ObelixUdpReaderThread::AskForStop()
{
  mRun = false;
}

void ObelixUdpReaderThread::run()
{
  qint64 lReadLoopCnt = 0;
  qint64 lReadSize = 0;
  QUdpSocket*  lRxSocket = new QUdpSocket();


  char* lDummyMessagePtr = (char*)calloc(1,mMessageSize);




  // Control flag
  mRun = true;

  // Bind port
  if (lRxSocket->bind(QHostAddress(mIp), mPort) != true)
  {
    qDebug("Bind failed on : %s", lRxSocket->errorString().toStdString().c_str());
  }

  //lRxSocket->setReadBufferSize(200*mFifoSize*sizeof(T_ObelixVideoMessage));
  //qDebug("Read socket buffer size=%i", lRxSocket->readBufferSize());

  lRxSocket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 2*mFifoSize*mMessageSize);

  // Thread loop
  while (mRun)
  {
    // Wait for data
    if (true == lRxSocket->waitForReadyRead(50))
    {
      //
      lReadLoopCnt = 0;

      // Lock FIFO
      mRwlocker->lockForWrite();

      // Read loop
      while (lRxSocket->hasPendingDatagrams())
      {
        //
        lReadLoopCnt++;

        // Slot available
        if (*mFifoIndex < mFifoSize)
        {
          // Read
          lReadSize = lRxSocket->readDatagram(mMessageFifo + ((*mFifoIndex) * mMessageSize), mMessageSize);

          // Read Ok -> FIFO Next
          if (lReadSize == mMessageSize)
          {
            /// \todo Manage a message log flag at this level ?

            // Update FIFO index
            *mFifoIndex = *mFifoIndex + 1;
          }
          else
          {
            qDebug("Read failed %i",lReadSize);
          }
        }
        else
        {
          /// \todo Manage case FIFO full
          qDebug("FIFO full");

          // Read to dummy
          lReadSize = lRxSocket->readDatagram(lDummyMessagePtr, mMessageSize);
        }
      }

      // Unlock FIFO
      mRwlocker->unlock();

      //
      //qDebug("Read x%i datagrams",lReadLoopCnt);
    }
  }

  delete lRxSocket;
}
