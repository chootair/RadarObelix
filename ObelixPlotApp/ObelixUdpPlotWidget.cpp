#include "ObelixUdpPlotWidget.h"

ObelixUdpPlotWidget::ObelixUdpPlotWidget(QWidget *parent) : ObelixPlotWidget(parent)
{
  mObelixUdpReaderVideo = new ObelixUdpReader();
  mObelixUdpReaderVideo->SetFifoParameter(this->FifoObelixVideo().mFifoPtr,
                                          this->FifoObelixVideo().mFifoIndexPtr,
                                          this->FifoObelixVideo().mFifoSize,
                                          this->FifoObelixVideo().mMessageSize,
                                          this->FifoObelixVideo().mFifoLocker);




  mObelixUdpReaderTrack = new ObelixUdpReader();
  mObelixUdpReaderTrack->SetFifoParameter(this->FifoObelixTrack().mFifoPtr,
                                          this->FifoObelixTrack().mFifoIndexPtr,
                                          this->FifoObelixTrack().mFifoSize,
                                          this->FifoObelixTrack().mMessageSize,
                                          this->FifoObelixTrack().mFifoLocker);





  mTimer = new QTimer(this);
  connect(mTimer,&QTimer::timeout,this,&ObelixUdpPlotWidget::OnTimerTick);
}

ObelixUdpPlotWidget::~ObelixUdpPlotWidget()
{
  Stop();
  Sleep(200);
  delete mTimer;
  delete mObelixUdpReaderTrack;
  delete mObelixUdpReaderVideo;
}

void ObelixUdpPlotWidget::SetUdpReaderVideoParameters(QString pIp, uint pPort)
{
  mObelixUdpReaderVideo->SetConnexionParameters(pIp, pPort);
}

void ObelixUdpPlotWidget::SetUdpReaderTrackParameters(QString pIp, uint pPort)
{
  mObelixUdpReaderTrack->SetConnexionParameters(pIp, pPort);
}

void ObelixUdpPlotWidget::Start(int pRefershPeriodMsec)
{
  mObelixUdpReaderVideo->start();
  mObelixUdpReaderTrack->start();
  if (pRefershPeriodMsec > 0)
  {
    mTimer->start(pRefershPeriodMsec);
  }
}

void ObelixUdpPlotWidget::Stop()
{
  mObelixUdpReaderVideo->AskForStop();
  mObelixUdpReaderTrack->AskForStop();
  mTimer->stop();
}

void ObelixUdpPlotWidget::OnTimerTick()
{
  this->RefreshScope();
}
