#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  //
  ui->doubleSpinBox->setValue(ui->olxPlot->RangeNm());
  ui->sbxDisplayPxKtsRatio->setValue(ui->olxPlot->DisplayPxKtsRatio());

  //
  mTimer = new QTimer(this);
  connect(mTimer,&QTimer::timeout,this,&MainWindow::OnTimerTick);
}

MainWindow::~MainWindow()
{
  delete mTimer;
  delete ui;
}

void MainWindow::OnTimerTick()
{
  ui->pgxFifoLoad->setValue(ui->olxPlot->FifoObelixVideoLoad());
  ui->pgxFifoTrackLoad->setValue(ui->olxPlot->FifoObelixTrackLoad());
  ui->tbxAverageTime->setText(QString("%1").arg(ui->olxPlot->ElapsedAverageTimeMs()));
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

void MainWindow::on_sbxDisplayPxKtsRatio_valueChanged(double arg1)
{
  ui->olxPlot->SetDisplayPxKtsRatio(arg1);
}

void MainWindow::on_pbxStart_clicked()
{
  ui->tbxIpVideo->setEnabled(false);
  ui->sbxPortVideo->setEnabled(false);
  ui->tbxIpTrack->setEnabled(false);
  ui->sbxPortTrack->setEnabled(false);
  ui->sbxTimer->setEnabled(false);

  //
  ui->olxPlot->SetUdpReaderVideoParameters(ui->tbxIpVideo->text(), ui->sbxPortVideo->value());
  ui->olxPlot->SetUdpReaderTrackParameters(ui->tbxIpTrack->text(), ui->sbxPortTrack->value());

  //
  ui->olxPlot->Start(ui->sbxTimer->value());
  mTimer->start(100);
}

void MainWindow::on_pbxStop_clicked()
{
  ui->olxPlot->Stop();

  //
  ui->tbxIpVideo->setEnabled(true);
  ui->sbxPortVideo->setEnabled(true);
  ui->tbxIpTrack->setEnabled(true);
  ui->sbxPortTrack->setEnabled(true);
  ui->sbxTimer->setEnabled(true);
}
