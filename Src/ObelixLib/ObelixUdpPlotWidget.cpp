#include "ObelixUdpPlotWidget.h"

ObelixUdpPlotWidget::ObelixUdpPlotWidget(QWidget *parent) : ObelixPlotWidget(parent)
{
  mObelixUdpReaderVideo = new ObelixUdpReaderThread();
  mObelixUdpReaderVideo->SetFifoParameter(this->FifoObelixVideo().mFifoPtr,
                                          this->FifoObelixVideo().mFifoIndexPtr,
                                          this->FifoObelixVideo().mFifoSize,
                                          this->FifoObelixVideo().mMessageSize,
                                          this->FifoObelixVideo().mFifoLocker);




  mObelixUdpReaderTrack = new ObelixUdpReaderThread();
  mObelixUdpReaderTrack->SetFifoParameter(this->FifoObelixTrack().mFifoPtr,
                                          this->FifoObelixTrack().mFifoIndexPtr,
                                          this->FifoObelixTrack().mFifoSize,
                                          this->FifoObelixTrack().mMessageSize,
                                          this->FifoObelixTrack().mFifoLocker);

  mObelixUdpReaderMap = new ObelixUdpReaderThread();
  mObelixUdpReaderMap->SetFifoParameter(this->FifoObelixMap().mFifoPtr,
                                        this->FifoObelixMap().mFifoIndexPtr,
                                        this->FifoObelixMap().mFifoSize,
                                        this->FifoObelixMap().mMessageSize,
                                        this->FifoObelixMap().mFifoLocker);


  mTimer = new QTimer(this);
  connect(mTimer,&QTimer::timeout,this,&ObelixUdpPlotWidget::OnTimerTick);
}

ObelixUdpPlotWidget::~ObelixUdpPlotWidget()
{
  Stop();
  //Sleep(200);
  delete mTimer;
  delete mObelixUdpReaderTrack;
  delete mObelixUdpReaderVideo;
  delete mObelixUdpReaderMap;
}

void ObelixUdpPlotWidget::SetUdpReaderVideoParameters(QString pIp, uint pPort)
{
  mObelixUdpReaderVideo->SetConnexionParameters(pIp, pPort);
}

void ObelixUdpPlotWidget::SetUdpReaderTrackParameters(QString pIp, uint pPort)
{
  mObelixUdpReaderTrack->SetConnexionParameters(pIp, pPort);
}

void ObelixUdpPlotWidget::SetUdpReaderMapParameters(QString pIp, uint pPort)
{
  mObelixUdpReaderMap->SetConnexionParameters(pIp, pPort);
}

void ObelixUdpPlotWidget::Start(int pRefershPeriodMsec)
{
  mObelixUdpReaderVideo->start();
  mObelixUdpReaderTrack->start();
  mObelixUdpReaderMap->start();
  if (pRefershPeriodMsec > 0)
  {
    mTimer->start(pRefershPeriodMsec);
  }
}

void ObelixUdpPlotWidget::Stop()
{
  mTimer->stop();
  //
  mObelixUdpReaderVideo->AskForStop();
  mObelixUdpReaderTrack->AskForStop();
  mObelixUdpReaderMap->AskForStop();
  //
  Sleep(200);
  //
  mObelixUdpReaderVideo->wait();
  mObelixUdpReaderTrack->wait();
  mObelixUdpReaderMap->wait();
}

void ObelixUdpPlotWidget::OnTimerTick()
{
  this->RefreshScope();
}
