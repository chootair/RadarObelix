#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <cstdint>

#include <QMainWindow>
#include <QTimer>
#include <QUdpSocket>


#include "Obelix.h"


typedef struct _T_Track
{
  double Azimuht;
  double Range;
  double Course;
  double Speed;
}T_Track;




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void OnTimerTick();

  void on_pbxRun_clicked(bool checked);

  void on_pbxGenerateBeam_clicked();

  void on_pbxOneShot_clicked();

  void on_cbxVideoMode_currentIndexChanged(int index);

  void on_sbxAzGapRto_valueChanged(double arg1);

  void on_sbxAzCorrectionGapRto_valueChanged(double arg1);

  void on_sbxRangeGapRto_valueChanged(double arg1);

  void on_sbxIntensty_valueChanged(double arg1);

  void on_sbxBeamWidth_valueChanged(double arg1);

  void on_cbxVideoMode_activated(int index);

  void on_sbxRange_valueChanged(double arg1);

private:
  void BuildBeam(double pStartAzimutDeg, double pAzimuthWidthDeg, double pStartRangeNm, double pRangeWidthNm, int pRadarMode);
  void GenerateBeam();

private:
  Ui::MainWindow *ui;

  QTimer* mTimer;
  QUdpSocket* mUdpSocket;

    int mCount;

    uint16_t mMsgNb;


  int mNbLevel;
  double mBeamWidthDeg;
  double mRangeNm;
  int mNbCells;
  T_ObelixVideoMessage* mBeam;
  int              mBeamSize;


  int mRadarMode;

  double mAzGapCorrectionRto;
  double mAzGapLvlRto;
  double mRgGapLvlRto;
  double mIntensity;



  T_Track* mTrackTable;
  int      mTrackTableSize;
};

#endif // MAINWINDOW_H
