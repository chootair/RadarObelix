#include "MainWindow.h"
#include "ui_MainWindow.h"



std::string gCallSingList[10] = { "Maverick",
                                  "Iceman"  ,
                                  "Goose"   ,
                                  "Pappy"   ,
                                  "Guido"   ,
                                  "Snoopy"  ,
                                  "Flash"   ,
                                  "Dino"    ,
                                  "Viper"   ,
                                  "Gadget"};



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  qDebug("MainWindows is executed in thread : %X", QThread::currentThreadId());

  ui->setupUi(this);
  mObelixSimThread = new ObelixUdpSimThread();

  // SIM
  ui->sbxAnntenaSped->setValue(mObelixSimThread->AntennaRpm());
  ui->sbxSimTimer->setValue(mObelixSimThread->TimerPeridod());
  ui->sbxBeamWidth->setValue(mObelixSimThread->BeamWidth());
  ui->sbxRange->setValue(mObelixSimThread->Range());
  ui->sbxIntensty->setValue(ui->olxPlot->VideoIntensity());
  ui->sbxSimNoise->setValue(mObelixSimThread->VideoNoise());
  ui->sbxPlatformHeading->setValue(mObelixSimThread->PlatformHeading());
  ui->sbxAzGapRto->setValue(mObelixSimThread->VideoAzimutGapLevelRatio());
  ui->sbxAzCorrectionGapRto->setValue(mObelixSimThread->VideoAzimutGapCorrectionRatio());
  ui->sbxRangeGapRto->setValue(mObelixSimThread->VideoRangeGapLevelRatio());

  ui->ckxSectorScan->setChecked(mObelixSimThread->SectorScan());
  ui->ckxSectorScanPlatform->setChecked(mObelixSimThread->SectorScanPlatformRef());
  ui->sbxSectorScanWitdh->setValue(mObelixSimThread->SectorScanWidth());
  ui->sbxSectorScanAzimuth->setValue(mObelixSimThread->SectorScanAzimuth());


  // DISPLAY
  ui->sbxScopeDisplayRange->setValue(ui->olxPlot->RangeNm());
  ui->sbxDisplayPxKtsRatio->setValue(ui->olxPlot->DisplayPxKtsRatio());
  ui->sbxScopeRangeRingSpace->setValue(ui->olxPlot->RangeRingSpaceNm());
  ui->ckxDispVideo->setChecked(ui->olxPlot->DisplayVideo());
  ui->ckxDispTracks->setChecked(ui->olxPlot->DisplayTracks());
  ui->ckxDispCompas->setChecked(ui->olxPlot->DisplayCompas());
  ui->ckxDispAircraft->setChecked(ui->olxPlot->DisplayAircraft());
  ui->ckxDispHeading->setChecked(ui->olxPlot->DisplayHeading());
  ui->ckxNorthUp->setChecked(ui->olxPlot->NorthUp());
  ui->ckxDispPolygons->setChecked(ui->olxPlot->DisplayMapPolygons());
  ui->ckxDispPoints->setChecked(ui->olxPlot->DisplayMapPoints());
  ui->ckxDispPlateformHistory->setChecked(ui->olxPlot->DisplayPlateformHistory());
  ui->ckxDispTracksHistory->setChecked(ui->olxPlot->DisplayTracksHistory());
  ui->ckxDispShapeMap->setChecked(ui->olxPlot->DisplayShapeMap());
  ui->ckxDispAntenna->setChecked(ui->olxPlot->DisplayAntenna());
  ui->ckxDispRangeLimit->setChecked(ui->olxPlot->DisplayRangeLimit());
  ui->ckxDispRangeRings->setChecked(ui->olxPlot->DisplayRangeRings());
  ui->sbxPresistance->setValue(ui->olxPlot->PresistenceRatio());
  ui->cbxPresistanceMode->setCurrentIndex(ui->olxPlot->PersistenceMode());

  ui->sbxSelectedTrack->setValue(ui->olxPlot->SelectedTrackIdx());
  ui->sbxHistoryMaxDurationS->setValue(ui->olxPlot->HistoryDisplayMaxTimeSec());






  mMainTimer = new QTimer(this);
  connect(mMainTimer,&QTimer::timeout,this,&MainWindow::OnTimerTick);
  mMainTimer->start(100);

  mSimTimer = new QTimer(this);
  connect(mSimTimer,&QTimer::timeout,this,&MainWindow::OnSimTimerTick);


  mSimTable = nullptr;
  mlCloudTable = nullptr;






  ui->pbxColorAntenna   ->SetColor(ui->olxPlot->ColorAntenna());
  ui->pbxColorRangeLimit->SetColor(ui->olxPlot->ColorRangeLimit());
  ui->pbxColorRangeRing ->SetColor(ui->olxPlot->ColorRangeRings());
  ui->pbxColorCompas    ->SetColor(ui->olxPlot->ColorCompas());
  ui->pbxColorTrack     ->SetColor(ui->olxPlot->ColorTracks());
  ui->pbxColorVideo     ->SetColor(ui->olxPlot->ColorVideo());
  ui->pbxColorAircraft  ->SetColor(ui->olxPlot->ColorAircraft());
  ui->pbxColorHeading   ->SetColor(ui->olxPlot->ColorHeading());
  ui->pbxColorPolygons  ->SetColor(ui->olxPlot->ColorMapPolygons());
  ui->pbxColorPoints    ->SetColor(ui->olxPlot->ColorMapPoints());
  ui->pbxColorShapeMap  ->SetColor(ui->olxPlot->ColorShapeMap());

  ui->pbxColorPlateformHistory    ->SetColor(ui->olxPlot->ColorPlateformHistory());
  ui->pbxColorTrackHistory    ->SetColor(ui->olxPlot->ColorTracksHistory());
  ui->pbxColorSelectedTrack    ->SetColor(ui->olxPlot->ColorSelectedTrack());

  connect(ui->pbxColorAntenna, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorRangeLimit, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorRangeRing, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorCompas, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorTrack, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorVideo, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorAircraft, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorHeading, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorPolygons, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorPoints, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorShapeMap, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorPlateformHistory, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorTrackHistory, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);
  connect(ui->pbxColorSelectedTrack, &ColorPickerButton::ColorChanged, this, &MainWindow::OnColorChanged);


  ui->sbxPlatformLat->setValue(48);
  ui->sbxPlatformLong->setValue(-5);

  BuildSimTrackTable();
  BuildSimCloudTable();



  ui->olxPlot->AddShapeMapFile("C:\\Data\\Pataugeoire\\ne_10m_coastline\\ne_10m_coastline.shp");

}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::OnColorChanged(QColor pColor)
{
  Q_UNUSED(pColor);

  //
  ui->olxPlot->SetColorAntenna    (ui->pbxColorAntenna   ->Color());
  ui->olxPlot->SetColorRangeLimit (ui->pbxColorRangeLimit->Color());
  ui->olxPlot->SetColorRangeRings (ui->pbxColorRangeRing ->Color());
  ui->olxPlot->SetColorCompas     (ui->pbxColorCompas    ->Color());
  ui->olxPlot->SetColorTracks     (ui->pbxColorTrack     ->Color());
  ui->olxPlot->SetColorVideo      (ui->pbxColorVideo     ->Color());
  ui->olxPlot->SetColorAircraft   (ui->pbxColorAircraft  ->Color());
  ui->olxPlot->SetColorHeading    (ui->pbxColorHeading   ->Color());
  ui->olxPlot->SetColorMapPolygons(ui->pbxColorPolygons  ->Color());
  ui->olxPlot->SetColorMapPoints  (ui->pbxColorPoints    ->Color());
  ui->olxPlot->SetColorPlateformHistory  (ui->pbxColorPlateformHistory    ->Color());
  ui->olxPlot->SetColorTracksHistory  (ui->pbxColorTrackHistory    ->Color());
  ui->olxPlot->SetColorSelectedTrack  (ui->pbxColorSelectedTrack    ->Color());
  ui->olxPlot->SetColorShapeMap(ui->pbxColorShapeMap ->Color());
}


void MainWindow::on_pbxRunSimulator_clicked()
{
  // Lock contols
  ui->sbxNbLevel->setEnabled(false);
  ui->sbxNbCells->setEnabled(false);
  ui->pbxRunSimulator->setEnabled(false);
  ui->pbxStopSimulator->setEnabled(true);

  // Set parameters
  mObelixSimThread->SetVideoBeamParameters(ui->sbxNbLevel->value(), ui->sbxNbCells->value());
  mObelixSimThread->SetUdpReaderParameters(ui->tbxIpVideo->text(), ui->sbxPortVideo->value(),
                                           ui->tbxIpTrack->text(), ui->sbxPortTrack->value(),
                                           ui->tbxIpMap->text()  , ui->sbxPortMap->value());

  // Run
  mObelixSimThread->start();
}


void MainWindow::on_pbxStopSimulator_clicked()
{
  // Stop
  mObelixSimThread->AskForStop();

  // unlock contols
  ui->sbxNbLevel->setEnabled(true);
  ui->sbxNbCells->setEnabled(true);
  ui->pbxRunSimulator->setEnabled(true);
  ui->pbxStopSimulator->setEnabled(false);
}




void MainWindow::on_pbxRunPlot_clicked()
{
  //
  ui->olxPlot->SetUdpReaderVideoParameters(ui->tbxIpVideo->text(), ui->sbxPortVideo->value());
  ui->olxPlot->SetUdpReaderTrackParameters(ui->tbxIpTrack->text(), ui->sbxPortTrack->value());
  ui->olxPlot->SetUdpReaderMapParameters  (ui->tbxIpMap->text()  , ui->sbxPortMap->value());

  // Run
  ui->olxPlot->Start(ui->sbxPlotTimer->value());
}


void MainWindow::OnTimerTick()
{
  ui->pgxFifoLoad->setValue(ui->olxPlot->FifoObelixVideoLoad());
  ui->pgxFifoTrackLoad->setValue(ui->olxPlot->FifoObelixTrackLoad());
  ui->pgxFifoMapLoad->setValue(ui->olxPlot->FifoObelixMapLoad());
  ui->tbxAverageTime->setText(QString("%1").arg(ui->olxPlot->ElapsedAverageTimeMs()));









}

void MainWindow::OnSimTimerTick()
{
  PushSimTrackTable();
  PushSimCloudTable();
  PushSimMapTable();
}

void MainWindow::on_sbxAnntenaSped_valueChanged(int arg1)
{
  mObelixSimThread->SetAntennaRpm(arg1);
}

void MainWindow::on_sbxBeamWidth_valueChanged(double arg1)
{
  mObelixSimThread->SetBeamWidth(arg1);
}

void MainWindow::on_cbxVideoMode_currentIndexChanged(int index)
{
  mObelixSimThread->SetVideoMode(index);
}

void MainWindow::on_sbxIntensty_valueChanged(double arg1)
{
  ui->olxPlot->SetVideoIntensity(arg1);
}

void MainWindow::on_pbxClearScope_clicked()
{
  ui->olxPlot->ClearScope();
}

void MainWindow::on_ckxDispAntenna_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayAntenna(ui->ckxDispAntenna->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispRangeLimit_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayRangeLimit(ui->ckxDispRangeLimit->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispRangeRings_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayRangeRings(ui->ckxDispRangeRings->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispCompas_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayCompas(ui->ckxDispCompas->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispAircraft_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayAircraft(ui->ckxDispAircraft->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispHeading_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayHeading(ui->ckxDispHeading->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxNorthUp_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetNorthUp(ui->ckxNorthUp->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispVideo_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayVideo(ui->ckxDispVideo->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispTracks_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayTracks(ui->ckxDispTracks->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_sbxPresistance_valueChanged(double arg1)
{
  ui->olxPlot->SetPresistenceRatio(arg1);
}

void MainWindow::on_cbxPresistanceMode_currentIndexChanged(int index)
{
  ui->olxPlot->SetPersistenceMode(static_cast<T_PersistMode>(index));
}

void MainWindow::on_sbxDisplayPxKtsRatio_valueChanged(double arg1)
{
  ui->olxPlot->SetDisplayPxKtsRatio(arg1);
}

void MainWindow::on_sbxScopeDisplayRange_valueChanged(double arg1)
{
  ui->olxPlot->SetRangeNm(arg1);
}

void MainWindow::on_sbxSimNoise_valueChanged(double arg1)
{
  mObelixSimThread->SetVideoNoise(arg1);
}

void MainWindow::on_sbxRange_valueChanged(int arg1)
{
  mObelixSimThread->SetRange(arg1);
}

void MainWindow::on_sbxPlatformHeading_valueChanged(double arg1)
{
  mObelixSimThread->SetPlatformHeading(arg1);
}






void MainWindow::on_pbxDispSource_clicked(bool checked)
{
  ui->gbxUdp->setVisible(checked);
  ui->gbxSource->setVisible(checked);
}

void MainWindow::on_pbxDispSim_clicked(bool checked)
{
  ui->gbxSimulator->setVisible(checked);
}

void MainWindow::on_pbxDispScope_clicked(bool checked)
{
  ui->gbxScope->setVisible(checked);
}


#define CLOUD_COUNT 20

void MainWindow::BuildSimCloudTable()
{
  if (mlCloudTable)
  {
    free (mlCloudTable);
  }

  //
  double lPlatformLat  = ui->sbxPlatformLat->value();
  double lPlatformLong = ui->sbxPlatformLong->value();



  mlCloudTable = (T_ObelixCloud*)calloc(CLOUD_COUNT, sizeof(T_ObelixCloud));

  //
  for (int i=0; i<CLOUD_COUNT;  i++)
  {
    mlCloudTable[i].Id = i;

    double lCloudLat  = lPlatformLat  + (-100.0 + rand()%200)/50.0;
    double lCloudLong = lPlatformLong + (-100.0 + rand()%200)/50.0;

    for (int j=0; j<5; j++)
    {
      mlCloudTable[i].Nodes[j].Latitude  = lCloudLat  +(-100 + rand()%200)/500.0;
      mlCloudTable[i].Nodes[j].Longitude = lCloudLong +(-100 + rand()%200)/500.0;
      mlCloudTable[i].Nodes[j].Intensity = (rand()%100)/700.0;
    }
  }
}

void MainWindow::BuildSimTrackTable()
{
  if (mSimTable)
  {
    free (mSimTable);
  }

  //
  double lPlatformLat  = ui->sbxPlatformLat->value();
  double lPlatformLong = ui->sbxPlatformLong->value();

  //
  mSimTableSize = ui->sbxNbSimTracks->value();
  mSimTable  = (T_SimTrack*)calloc(mSimTableSize, sizeof(T_SimTrack));

  //
  mlTrackTable = (T_ObelixTrack*)calloc(mSimTableSize, sizeof(T_ObelixTrack));

  //
  for (int i=0; i<mSimTableSize;  i++)
  {
    //
    if (ui->ckxRandomSimAz->isChecked() == true)
    {
      mSimTable[i].Bearing = rand()%static_cast<int>(ui->sbxSimAz->value());
    }
    else
    {
      mSimTable[i].Bearing = ui->sbxSimAz->value();
    }

    //
    if (ui->ckxRandomSimDist->isChecked() == true)
    {
      mSimTable[i].Distance = rand()%static_cast<int>(ui->sbxSimDist->value());
    }
    else
    {
      mSimTable[i].Distance = ui->sbxSimDist->value();
    }

    //
    if (ui->ckxRandomSimCourse->isChecked() == true)
    {
      mSimTable[i].Course = rand()%static_cast<int>(ui->sbxSimCourse->value());
    }
    else
    {
      mSimTable[i].Course = ui->sbxSimCourse->value();
    }

    //
    if (ui->ckxRandomSimSpeed->isChecked() == true)
    {
      mSimTable[i].GroundSpeed = rand()%static_cast<int>(ui->sbxSimSpeed->value());
    }
    else
    {
      mSimTable[i].GroundSpeed = ui->sbxSimSpeed->value();
    }

    //
    OTB::AzDstToLatLon(lPlatformLat,
                       lPlatformLong,
                       mSimTable[i].Bearing,
                       mSimTable[i].Distance*OTB::NM_TO_M,
                       mSimTable[i].Latitude,
                       mSimTable[i].Longitude);
  }
}

void MainWindow::PushSimTrackTable()
{
  double lPlatformLat  = ui->sbxPlatformLat->value();
  double lPlatformLong = ui->sbxPlatformLong->value();
  //
  double lTrackInLat = 0;
  double lTrackInLong = 0;
  double lTrackOutLat = 0;
  double lTrackOutLong = 0;
  //
  double lDeltaT = 0;

  // Compute trajectory
  for (int i=0; i<mSimTableSize;  i++)
  {

    double lTrackBearing  = static_cast<double>(mSimTable[i].Bearing);
    double lTrackDistance = static_cast<double>(mSimTable[i].Distance);
    double lTrackCourse   = static_cast<double>(mSimTable[i].Course);
    double lTrackSpeed    = static_cast<double>(mSimTable[i].GroundSpeed);



    lTrackInLat  = mSimTable[i].Latitude;
    lTrackInLong = mSimTable[i].Longitude;
    lDeltaT      = ui->sbxSimPeriod->value()*0.001*ui->sbxSimSpeedFactor->value();

    // Compute track trajectory
    OTB::ComputeTrajectory(lTrackInLat,
                           lTrackInLong,
                           lTrackOutLat,
                           lTrackOutLong,
                           mSimTable[i].GroundSpeed*OTB::KNOT_TO_MS,
                           mSimTable[i].Course,
                           lDeltaT);

    // Get Azimuth distance from platfrom
    OTB::ComputeAzimuthDistance(lPlatformLat,
                                lPlatformLong,
                                lTrackOutLat,
                                lTrackOutLong,
                                lTrackBearing,
                                lTrackDistance);

    // Unit corrections
    lTrackDistance = lTrackDistance/OTB::NM_TO_M;

    // Update track message
    if (lTrackDistance >= 100)
    {
      // Reset track
      mSimTable[i].Bearing     = rand()%static_cast<int>(ui->sbxSimAz->value());
      mSimTable[i].Distance    = rand()%static_cast<int>(ui->sbxSimDist->value());
      mSimTable[i].Course      = rand()%static_cast<int>(ui->sbxSimCourse->value());
      mSimTable[i].GroundSpeed = rand()%static_cast<int>(ui->sbxSimSpeed->value());
      //
      OTB::AzDstToLatLon(lPlatformLat, lPlatformLong, mSimTable[i].Bearing, mSimTable[i].Distance*OTB::NM_TO_M, lTrackOutLat, lTrackOutLong);
      //
      mSimTable[i].Latitude  = lTrackOutLat;
      mSimTable[i].Longitude = lTrackOutLong;
    }
    else
    {
      mSimTable[i].Latitude  = lTrackOutLat;
      mSimTable[i].Longitude = lTrackOutLong;
      mSimTable[i].Bearing   = lTrackBearing;
      mSimTable[i].Distance  = lTrackDistance;
    }
  }

  // Push to sim thread
  for (quint16 i=0; i<mSimTableSize;  i++)
  {
    mlTrackTable[i].Id          = i;
    mlTrackTable[i].Bearing     = static_cast<quint16>(mSimTable[i].Bearing    /OBX_TRK_BEARINGCOURSE_LSB);
    mlTrackTable[i].Distance    = static_cast<quint16>(mSimTable[i].Distance   /OBX_TRK_DISTANCE_LSB);
    mlTrackTable[i].Course      = static_cast<quint16>(mSimTable[i].Course     /OBX_TRK_BEARINGCOURSE_LSB);
    mlTrackTable[i].GroundSpeed = static_cast<quint16>(mSimTable[i].GroundSpeed/OBX_TRK_SPEED_LSB);
    //
    mlTrackTable[i].Latitude  = static_cast<quint32>(mSimTable[i].Latitude/OBX_TRK_LATLONG_LSB);
    mlTrackTable[i].Longitude = static_cast<quint32>(mSimTable[i].Longitude/OBX_TRK_LATLONG_LSB);
    //
    sprintf(mlTrackTable[i].CallSing, mSimTable[i].CallSing.c_str());
  }

  mObelixSimThread->PushTracks(mlTrackTable, mSimTableSize);
}

void MainWindow::PushSimCloudTable()
{


  mObelixSimThread->PushClouds(mlCloudTable, CLOUD_COUNT);
}





void MainWindow::PushSimMapTable()
{
  T_ObelixMapPoint lSinglePointTbl[5];

  // N
  lSinglePointTbl[0].Latitude  = (49)/OBX_TRK_LATLONG_LSB;
  lSinglePointTbl[0].Longitude = (-5)/OBX_TRK_LATLONG_LSB;
  sprintf(lSinglePointTbl[0].Label, "N");

  // E
  lSinglePointTbl[1].Latitude  = (48)/OBX_TRK_LATLONG_LSB;
  lSinglePointTbl[1].Longitude = (-4)/OBX_TRK_LATLONG_LSB;
  sprintf(lSinglePointTbl[1].Label, "E");


  // S
  lSinglePointTbl[2].Latitude  = (47)/OBX_TRK_LATLONG_LSB;
  lSinglePointTbl[2].Longitude = (-5)/OBX_TRK_LATLONG_LSB;
  sprintf(lSinglePointTbl[2].Label, "S");


  // W
  lSinglePointTbl[3].Latitude  = (48)/OBX_TRK_LATLONG_LSB;
  lSinglePointTbl[3].Longitude = (-6)/OBX_TRK_LATLONG_LSB;
  sprintf(lSinglePointTbl[3].Label, "W");


  // W
  lSinglePointTbl[4].Latitude  = (49)/OBX_TRK_LATLONG_LSB;
  lSinglePointTbl[4].Longitude = (-6)/OBX_TRK_LATLONG_LSB;
  sprintf(lSinglePointTbl[4].Label, "WNW");


  mObelixSimThread->PushMapObject(0,OBX_MAP_SINGLE,lSinglePointTbl,5);



  int lPolySize = 10;
  T_ObelixMapPoint lPolyPointTbl[10];
  for (int i=0; i<lPolySize; i++)
  {
    lPolyPointTbl[i].Latitude   = (48.0 + (rand()%100)*0.01)/OBX_TRK_LATLONG_LSB;
    lPolyPointTbl[i].Longitude = (-5.0 + (rand()%100)*0.01)/OBX_TRK_LATLONG_LSB;
  }
  mObelixSimThread->PushMapObject(1,OBX_MAP_POLY,lPolyPointTbl,lPolySize);






  mObelixSimThread->SetPlatformPosition(ui->sbxPlatformLat->value(), ui->sbxPlatformLong->value());
}

void MainWindow::on_pbxBuildSimGenerated_clicked()
{
  BuildSimTrackTable();
}

void MainWindow::on_pbxClearSimGenerated_clicked()
{

}

void MainWindow::on_pbxRunSim_clicked()
{
  mSimTimer->start(ui->sbxSimPeriod->value());
}

void MainWindow::on_pbxStopSim_clicked()
{
  mSimTimer->stop();
}











void MainWindow::on_ckxSectorScan_clicked(bool checked)
{
  mObelixSimThread->SetSectorScan(checked);
}

void MainWindow::on_sbxSectorScanAzimuth_valueChanged(double arg1)
{
  mObelixSimThread->SetSectorScanAzimuth(arg1);
}


void MainWindow::on_sbxSectorScanWitdh_valueChanged(double arg1)
{
  mObelixSimThread->SetSectorScanWidth(arg1);
}

void MainWindow::on_ckxSectorScanPlatform_clicked(bool checked)
{
  mObelixSimThread->SetSectorScanPlatformRef(checked);
}

void MainWindow::on_sbxAzGapRto_valueChanged(double arg1)
{
  mObelixSimThread->SetVideoAzimutGapLevelRatio(arg1);
}

void MainWindow::on_sbxAzCorrectionGapRto_valueChanged(double arg1)
{
  mObelixSimThread->SetVideoAzimutGapCorrectionRatio(arg1);
}

void MainWindow::on_sbxRangeGapRto_valueChanged(double arg1)
{
  mObelixSimThread->SetVideoRangeGapLevelRatio(arg1);
}

void MainWindow::on_ckxDispPolygons_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayMapPolygons(ui->ckxDispPolygons->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_ckxDispPoints_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
  ui->olxPlot->SetDisplayMapPoints(ui->ckxDispPoints->isChecked());
  ui->olxPlot->RefreshScope();
}

void MainWindow::on_sbxScopeRangeRingSpace_valueChanged(int arg1)
{
  ui->olxPlot->SetRangeRingSpaceNm(arg1);
}

void MainWindow::on_sbxSelectedTrack_valueChanged(int arg1)
{
  ui->olxPlot->SetSelectedTrackIdx(arg1);
}

void MainWindow::on_sbxHistoryMaxDurationS_valueChanged(int arg1)
{
  ui->olxPlot->SetHistoryDisplayMaxTimeSec(arg1);
}

void MainWindow::on_ckxDispPlateformHistory_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
    ui->olxPlot->SetDisplayPlateformHistory(ui->ckxDispPlateformHistory->isChecked());
}

void MainWindow::on_ckxDispTracksHistory_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
    ui->olxPlot->SetDisplayTracksHistory(ui->ckxDispTracksHistory->isChecked());
}

void MainWindow::on_ckxDispShapeMap_stateChanged(int arg1)
{
  Q_UNUSED(arg1);
    ui->olxPlot->SetDisplayShapeMap(ui->ckxDispShapeMap->isChecked());
}


