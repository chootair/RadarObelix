#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  mObelixVideoReader = new ObelixUdpReader();
  mObelixVideoReader->SetConnexionParameters("192.12.12.12", 5147);
  mObelixVideoReader->SetFifoParameter(ui->olxPlot->FifoObelixVideo().mFifoPtr,
                                       ui->olxPlot->FifoObelixVideo().mFifoIndexPtr,
                                       ui->olxPlot->FifoObelixVideo().mFifoSize,
                                       ui->olxPlot->FifoObelixVideo().mMessageSize,
                                       ui->olxPlot->FifoObelixVideo().mFifoLocker);




  mObelixTrackReader = new ObelixUdpReader();
  mObelixTrackReader->SetConnexionParameters("192.12.12.12", 5148);
  mObelixTrackReader->SetFifoParameter(ui->olxPlot->FifoObelixTrack().mFifoPtr,
                                       ui->olxPlot->FifoObelixTrack().mFifoIndexPtr,
                                       ui->olxPlot->FifoObelixTrack().mFifoSize,
                                       ui->olxPlot->FifoObelixTrack().mMessageSize,
                                       ui->olxPlot->FifoObelixTrack().mFifoLocker);



  ui->doubleSpinBox->setValue(ui->olxPlot->RangeNm());

  mTimer = new QTimer(this);
  connect(mTimer,&QTimer::timeout,this,&MainWindow::OnTimerTick);
}

MainWindow::~MainWindow()
{
  mObelixVideoReader->AskForStop();
  mObelixTrackReader->AskForStop();
  Sleep(200);
  delete mTimer;
  delete mObelixTrackReader;
  delete mObelixVideoReader;
  delete ui;
}


void MainWindow::OnTimerTick()
{
  ui->olxPlot->RefreshScope();
  ui->pgxFifoLoad->setValue(ui->olxPlot->FifoObelixVideoLoad());
  ui->pgxFifoTrackLoad->setValue(ui->olxPlot->FifoObelixTrackLoad());
  ui->tbxAverageTime->setText(QString("%1").arg(ui->olxPlot->ElapsedAverageTimeMs()));
  //qDebug("%i %i",ui->olxPlot->FifoLoad(), ui->olxPlot->ElapsedAverageTimeMs());
}

void MainWindow::on_pbxRun_clicked(bool checked)
{
  //
  if (true == checked)
  {
    mObelixVideoReader->start();
    mObelixTrackReader->start();
    mTimer->start(ui->sbxTimer->value());
  }
  else
  {
    mObelixTrackReader->AskForStop();
    mObelixVideoReader->AskForStop();
    mTimer->stop();
  }
}

void MainWindow::on_pbxClearScope_clicked()
{
  ui->olxPlot->ClearScope();
}

void MainWindow::on_sbxPresistance_valueChanged(double arg1)
{
  ui->olxPlot->SetPresistenceRatio(arg1);
}

void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
  ui->olxPlot->SetRangeNm(arg1);
}
