#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  mUdpSocket = new QUdpSocket(this);
  mTimer = new QTimer(this);
  connect(mTimer,&QTimer::timeout,this,&MainWindow::OnTimerTick);


  mCount = 0;
  mMsgNb = 0;

  // Beam Definiton
  mBeam = nullptr;
  mBeamSize = 0;

  mNbLevel = 5;
   mNbCells =200;


  mBeamWidthDeg = 1;
  mRangeNm      = 100;


  ui->sbxNbCells->setValue(mNbCells);
  ui->sbxNbLevel->setValue(mNbLevel);

  ui->sbxRange->setValue(mRangeNm);
  ui->sbxBeamWidth->setValue(mBeamWidthDeg);

  mAzGapLvlRto = 2;
  mAzGapCorrectionRto = 1;
  mRgGapLvlRto = 100;
  mIntensity = 1;

  ui->sbxAzGapRto->setValue(mAzGapLvlRto);
  ui->sbxAzCorrectionGapRto->setValue(mAzGapCorrectionRto);
  ui->sbxRangeGapRto->setValue(mRgGapLvlRto);
  ui->sbxIntensty->setValue(mIntensity);


  mRadarMode = 1;
  ui->cbxVideoMode->setCurrentIndex(mRadarMode);


  GenerateBeam();

  mTrackTableSize = 25;
  mTrackTable = (T_Track*)calloc(mTrackTableSize, sizeof(T_Track));

  for (int i=0; i<mTrackTableSize;  i++)
  {
    mTrackTable[i].Azimuht = rand()%360;
    mTrackTable[i].Range   = rand()%100;
  }

}

MainWindow::~MainWindow()
{
  delete mUdpSocket;
  delete ui;
}

void MainWindow::GenerateBeam()
{

  if (mBeam)
  {
    free (mBeam);
  }

  mNbCells = ui->sbxNbCells->value();
  mNbLevel = ui->sbxNbLevel->value();
  mBeamWidthDeg= ui->sbxBeamWidth->value();

  //
  mBeamSize = (1<<mNbLevel) -1;

  //
  ui->tbxBeamSize->setText(QString("%1").arg(mBeamSize));

  //
  mBeam = (T_ObelixVideoMessage*)calloc(mBeamSize, sizeof(T_ObelixVideoMessage));
  //
  for (int lIdxBeam=0; lIdxBeam<mBeamSize; lIdxBeam++)
  {
    mBeam[lIdxBeam].StartRgNm = 0.1;
    mBeam[lIdxBeam].CellWidthNm   = (1-mBeam[lIdxBeam].StartRgNm)/mNbCells;
    mBeam[lIdxBeam].CellCount   = mNbCells/mNbLevel;
    mBeam[lIdxBeam].CellCount   = qMin(mBeam[lIdxBeam].CellCount, OBELIX_CELL_COUNT);
  }


}



void MainWindow::OnTimerTick()
{
  int lMsgSent = 0;
  int lMsgErr = 0;
  qint64 lWriteSize=0;

  // Mutliple beams
  for (int i=0; i<ui->sbxBeamRepeat->value(); i++)
  {
    // Build beams
    BuildBeam(mCount*mBeamWidthDeg,mBeamWidthDeg, 0,mRangeNm,mRadarMode);

    // Update counter
    mCount++;
    if (mCount*mBeamWidthDeg >= 360)
    {
      mCount = 0;
    }

    // Send beam packets
    for (int lIdxBeam=0; lIdxBeam<mBeamSize; lIdxBeam++)
    {
      lWriteSize = mUdpSocket->writeDatagram((char*)&(mBeam[lIdxBeam]),sizeof(T_ObelixVideoMessage),QHostAddress("192.12.12.12"),5147);
      if (lWriteSize != sizeof(T_ObelixVideoMessage))
      {
        lMsgErr++;
      }
      else
      {
        lMsgSent++;
      }
    }
  }


   T_ObelixTrackReportMessage lTrackReport;

   lTrackReport.Number=0;
   lTrackReport.TrackCount=qMin((int)OBELIX_TRACK_TABLE_COUNT, mTrackTableSize);

  // Track table
  for (int i=0; i<lTrackReport.TrackCount;  i++)
  {
   lTrackReport.TrackTbl[i].Id=i;
   lTrackReport.TrackTbl[i].Status=1;
   lTrackReport.TrackTbl[i].Spare=0;
   lTrackReport.TrackTbl[i].LatitudeDeg=0;
   lTrackReport.TrackTbl[i].LongitudeDeg=0;
   lTrackReport.TrackTbl[i].BearingDeg = mTrackTable[i].Azimuht;
   lTrackReport.TrackTbl[i].DistanceNm = mTrackTable[i].Range;
   lTrackReport.TrackTbl[i].CourseDeg=rand()%360;
   lTrackReport.TrackTbl[i].GroundSpeedKts=rand()%50;
   lTrackReport.TrackTbl[i].FlightLevel=0;
   sprintf(lTrackReport.TrackTbl[i].CallSing, "TRK %02i",i);

  }

  lWriteSize = mUdpSocket->writeDatagram((char*)&(lTrackReport),sizeof(T_ObelixTrackReportMessage),QHostAddress("192.12.12.12"),5148);


  // Update info
  ui->tbxMsgSent->setText(QString("%1").arg(lMsgSent));
  ui->tbxMsgErr->setText(QString("%1").arg(lMsgErr));
}

void MainWindow::BuildBeam(double pStartAzimutDeg,
                           double pAzimuthWidthDeg,
                           double pStartRangeNm,
                           double pRangeWidthNm,
                           int    pRadarMode)
{
  int lIdxBeam = 0;

  // Loop on beam levels
  for (int lIdxLvl=0; lIdxLvl<mNbLevel; lIdxLvl++)
  {
    // Number of sub-beam for this level
    int lLevelSubBeamCnt = 1<<lIdxLvl;

    // Sub-beam width
    double lSubBeamAzWidthDeg = pAzimuthWidthDeg/lLevelSubBeamCnt;
    double lSubBeamRgWidthNm  = pRangeWidthNm/mNbCells;

    // Loop on the sub-beams of the level
    for (int lIdxSubBeam=0; lIdxSubBeam<lLevelSubBeamCnt; lIdxSubBeam++)
    {
      // Message counter
      mBeam[lIdxBeam].Number = mMsgNb;
      mMsgNb++;

      // Sub-Beam start and stop azimut
      mBeam[lIdxBeam].StartAzDeg = pStartAzimutDeg + lIdxSubBeam*lSubBeamAzWidthDeg;
      mBeam[lIdxBeam].EndAzDeg   = mBeam[lIdxBeam].StartAzDeg + lSubBeamAzWidthDeg;

      // Sub-Beam start range
      mBeam[lIdxBeam].CellWidthNm = lSubBeamRgWidthNm;
      mBeam[lIdxBeam].StartRgNm   = pStartRangeNm + lIdxLvl * mBeam[lIdxBeam].CellWidthNm * mBeam[lIdxBeam].CellCount;

      // Sub-Beam cells value
      for (int lIdxCell=0; lIdxCell<mBeam[lIdxLvl].CellCount; lIdxCell++)
      {
        // Reset
        mBeam[lIdxBeam].CellValueTbl[lIdxCell]=0;

        // Cell range ratio
        double lCellRgRto = (mBeam[lIdxBeam].StartRgNm+(lIdxCell+0.5)*mBeam[lIdxBeam].CellWidthNm-pStartRangeNm)/pRangeWidthNm;

        // Track mode
        if (pRadarMode == 1)
        {
          // Loop on track table
          for (int lIdxTrack=0; lIdxTrack<mTrackTableSize;  lIdxTrack++)
          {
            // Valid track
            /// \todo Manage valid track

            // Track gap
            double lAzGapDeg = fabs((mBeam[lIdxBeam].StartAzDeg + 0.5 *lSubBeamAzWidthDeg) - mTrackTable[lIdxTrack].Azimuht);
            double lRgGapNm  = fabs(mBeam[lIdxBeam].StartRgNm+(lIdxCell+0.5)*mBeam[lIdxBeam].CellWidthNm - mTrackTable[lIdxTrack].Range);

            // Gap correction
            lAzGapDeg = lAzGapDeg*(1-(1-lCellRgRto)*mAzGapCorrectionRto);
            lRgGapNm  = lRgGapNm/pRangeWidthNm;

            // Intens
            double lAzLvl = qBound(0.0, 1 - mAzGapLvlRto * lAzGapDeg, 1.0);
            double lRgLvl = qBound(0.0, 1 - mRgGapLvlRto * lRgGapNm, 1.0);
            int    lLvl = 255 * mIntensity * lAzLvl * lRgLvl;

            //
            mBeam[lIdxBeam].CellValueTbl[lIdxCell]= qMin(255, mBeam[lIdxBeam].CellValueTbl[lIdxCell]+lLvl);
          }
        }
        // Meteo mode
        else if (pRadarMode == 2)
        {
          /// \todo Meteo mode
        }
        // Random mode
        else if (pRadarMode == 3)
        {
          mBeam[lIdxBeam].CellValueTbl[lIdxCell]=rand()%255;
        }
      }

      //
      lIdxBeam++;
    }
  }
}


void MainWindow::on_pbxRun_clicked(bool checked)
{
  ui->gbxBeam->setEnabled(!checked);

  if (checked)
  {
    ui->pbxRun->setText("Running");
    mTimer->start(ui->sbxTimer->value());
  }
  else
  {
    ui->pbxRun->setText("Run");
    mTimer->stop();
  }
}

void MainWindow::on_pbxGenerateBeam_clicked()
{
  GenerateBeam();
}

void MainWindow::on_pbxOneShot_clicked()
{
  OnTimerTick();
}

void MainWindow::on_cbxVideoMode_currentIndexChanged(int index)
{

}

void MainWindow::on_sbxAzGapRto_valueChanged(double arg1)
{
    mAzGapLvlRto = arg1;
}

void MainWindow::on_sbxAzCorrectionGapRto_valueChanged(double arg1)
{
  mAzGapCorrectionRto = arg1;
}

void MainWindow::on_sbxRangeGapRto_valueChanged(double arg1)
{
    mRgGapLvlRto = arg1;
}

void MainWindow::on_sbxIntensty_valueChanged(double arg1)
{
    mIntensity = arg1;
}

void MainWindow::on_sbxBeamWidth_valueChanged(double arg1)
{
    mBeamWidthDeg = arg1;
}

void MainWindow::on_cbxVideoMode_activated(int index)
{
    mRadarMode = index;
}

void MainWindow::on_sbxRange_valueChanged(double arg1)
{
    mRangeNm = arg1;
}
