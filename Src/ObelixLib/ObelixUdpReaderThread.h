#ifndef OBELIXUDPREADERTHREAD_H
#define OBELIXUDPREADERTHREAD_H

// Qt
#include <QThread>
#include <QReadWriteLock>
#include <QUdpSocket>

//
#include "Obelix.h"

class ObelixUdpReaderThread : public QThread
{
  Q_OBJECT

public:
  ObelixUdpReaderThread(QObject *parent = nullptr);
  ~ObelixUdpReaderThread();
  void SetConnexionParameters(QString pIp, uint pPort);
  void SetFifoParameter(char*           pMessageFifo,
                        uint*           pFifoIndex,
                        uint            pFifoSize,
                        uint            pMessageSize,
                        QReadWriteLock* pRwlocker);

  void AskForStop();
private:
  void run();

private:
  bool mRun;
  //
QString mIp;
uint mPort;
  uint16_t mLastNb;
  //
  uint*           mFifoIndex;
  uint            mFifoSize;
  uint            mMessageSize;
  char*           mMessageFifo;
  QReadWriteLock* mRwlocker;
};

#endif // OBELIXUDPREADERTHREAD_H
