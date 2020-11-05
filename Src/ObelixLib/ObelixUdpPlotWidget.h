#ifndef OBELIXUDPPLOTWIDGET_H
#define OBELIXUDPPLOTWIDGET_H

#include <QTimer>

#include "ObelixPlotWidget.h"
#include "ObelixUdpReaderThread.h"

class OBELIXLIBSHARED_EXPORT ObelixUdpPlotWidget : public ObelixPlotWidget
{
  Q_OBJECT
public:
  ObelixUdpPlotWidget(QWidget *parent = nullptr);
  ~ObelixUdpPlotWidget();
  void SetUdpReaderVideoParameters(QString pIp, uint pPort);
  void SetUdpReaderTrackParameters(QString pIp, uint pPort);
  void Start(int pRefershPeriodMsec = 50);
  void Stop();


private slots:
  void OnTimerTick();

private:
  QTimer* mTimer;
  //
  ObelixUdpReaderThread* mObelixUdpReaderVideo;
  ObelixUdpReaderThread* mObelixUdpReaderTrack;
};

#endif // OBELIXUDPPLOTWIDGET_H
