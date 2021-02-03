//----------------------------------------------------------------------------//
//--                              Radar OBELIX                              --//
//----------------------------------------------------------------------------//
/// \file    ObelixPlotWidget.h
/// \brief   Heder file for the ::ObelixPlotWidget class
/// ---------------------------------------------------------------------------
/// \version 1.0
/// \author  Guillaume.C
/// \date    01/04/2020
/// \note    Creation for initial version
//----------------------------------------------------------------------------//
#ifndef OBELIXPLOTWIDGET_H
#define OBELIXPLOTWIDGET_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QElapsedTimer>
#include <QReadWriteLock>
#include <QDateTime>

#include "ObelixLibDef.h"
#include "Obelix.h"
#include "PresistImage.h"


/// \brief FIFO handler
typedef struct _T_FifoHandler
{
  uint*           mFifoIndexPtr; ///< Index pointer
  uint            mFifoSize;     ///< Size
  uint            mMessageSize;  ///< Message size
  char*           mFifoPtr;      ///< FIFO pointer
  QReadWriteLock* mFifoLocker;   ///< FIFO locker
}T_FifoHandler;

/// \brief Plot track
typedef struct _T_PlotTrack
{
  T_ObelixTrack Track;
  qint64        LastUpdate;
}T_PlotTrack;

typedef struct _T_PlotMap
{
  uint8_t                 Type;
  uint16_t                Count;
  QList<T_ObelixMapPoint> Points;
}T_PlotMap;

/// \brief Persistant mode
typedef enum _T_PersistMode
{
  PersistComposition = 0, ///< Image composition
  PersistPixel            ///< Pixel computation
}T_PersistMode;



class OBELIXLIBSHARED_EXPORT ObelixPlotWidget : public QOpenGLWidget
{
  Q_OBJECT
public:
  explicit ObelixPlotWidget(QWidget *parent = nullptr);
  ~ObelixPlotWidget();

  ///
  void RefreshScope();
  void ClearScope();

  void SetPresistenceRatio(double pRatio);
  inline double PresistenceRatio() const { return mPersistRatio;}

  void SetPersistenceMode(T_PersistMode pPersistMode) {mPersistMode = pPersistMode;}
  inline T_PersistMode PersistenceMode() const { return mPersistMode;}

  inline double RangeNm() const {return mRangeNm;}
  void SetRangeNm(double pRangeNm);


  //
  inline uint   FifoObelixVideoLoad() const {return mFifoObelixVideoLoad;}
  inline T_FifoHandler   FifoObelixVideo() {return mFifoObelixVideo;}
  //
  inline uint   FifoObelixTrackLoad() const {return mFifoObelixTrackLoad;}
  inline T_FifoHandler   FifoObelixTrack() {return mFifoObelixTrack;}
  //
  inline uint   FifoObelixMapLoad() const {return mFifoObelixMapLoad;}
  inline T_FifoHandler   FifoObelixMap() {return mFifoObelixMap;}



  inline qint64 ElapsedAverageTimeMs() const {return mElapsedAverageTimeMs;}


  //
  inline double DisplayPxKtsRatio() const  {return mDisplayPxKtsRatio;}
  inline void SetDisplayPxKtsRatio(double pDisplayPxKtsRatio) {mDisplayPxKtsRatio=pDisplayPxKtsRatio;}


  inline bool NorthUp          () const {return mNorthUp;}
  inline bool DisplayAntenna   () const {return mDisplayAntenna;}
  inline bool DisplayRangeLimit() const {return mDisplayRangeLimit;}
  inline bool DisplayRangeRings() const {return mDisplayRangeRings;}
  inline bool DisplayCompas    () const {return mDisplayCompas;}
  inline bool DisplayVideo     () const {return mDisplayVideo;}
  inline bool DisplayTracks    () const {return mDisplayTracks;}
  inline bool DisplayAircraft  () const {return mDisplayAircraft;}
  inline bool DisplayHeading   () const {return mDisplayHeading;}
  inline void SetNorthUp          (bool pEnable) {mNorthUp = pEnable; mNeedToPaintInfo = true;}
  inline void SetDisplayAntenna   (bool pEnable) {mDisplayAntenna = pEnable;}
  inline void SetDisplayRangeLimit(bool pEnable) {mDisplayRangeLimit = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayRangeRings(bool pEnable) {mDisplayRangeRings = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayCompas    (bool pEnable) {mDisplayCompas = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayVideo     (bool pEnable) {mDisplayVideo = pEnable;}
  inline void SetDisplayTracks    (bool pEnable) {mDisplayTracks = pEnable;}
  inline void SetDisplayAircraft  (bool pEnable) {mDisplayAircraft = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayHeading   (bool pEnable) {mDisplayHeading  = pEnable; mNeedToPaintInfo=true;}


  inline QColor ColorAntenna   () const {return mColorAntenna;}
  inline QColor ColorRangeLimit() const {return mColorRangeLimit;}
  inline QColor ColorRangeRings() const {return mColorRangeRings;}
  inline QColor ColorCompas    () const {return mColorCompas;}
  inline QColor ColorVideo     () const {return mColorVideo;}
  inline QColor ColorTracks    () const {return mColorTracks;}
  inline QColor ColorAircraft  () const {return mColorAircraft;}
  inline QColor ColorHeading   () const {return mColorHeading;}

  inline void  SetColorAntenna   (QColor pColor) {mColorAntenna    = pColor;}
  inline void  SetColorRangeLimit(QColor pColor) {mColorRangeLimit = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorRangeRings(QColor pColor) {mColorRangeRings = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorCompas    (QColor pColor) {mColorCompas     = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorVideo     (QColor pColor) {mColorVideo      = pColor;}
  inline void  SetColorTracks    (QColor pColor) {mColorTracks     = pColor;}
  inline void  SetColorAircraft  (QColor pColor) {mColorAircraft   = pColor;}
  inline void  SetColorHeading   (QColor pColor) {mColorHeading    = pColor;}

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void PaintControl();
  void SetMyGeometry();
  void PaintVideoCells(QPainter* pPainter);
  void PaintTrackPlots(QPainter* pPainter);
  void PaintHeadingFeatures(QPainter* pPainter);
  void PaintTools(QPainter* pPainter);
  void PaintMap(QPainter* pPainter);
  int ReadTrackPlots();
  int ReadMapPlots();

private:

  QImage* mScopeVideoImg;
  QImage* mScopeTrackImg;
  QImage* mHeadingImg;
  QImage* mToolsImg;
  QImage* mMapImg;
  QImage* mWidgetImg;
  PresistImage* mPersistImg;

  //bool mIsPersistEnabled;
  T_PersistMode mPersistMode;
  double mPersistRatio;
  QColor mPersistMultiplyColor;


  double mRangeNm;

  //
  QLabel* mLbxWidget;
  //
  int mSideMargin;
  double graph_ratio_m;
  double spacing_graph_ratio_m;
  double mHrzRatio;
  //
  int mRefSize;
  int mMarkRad;
  int mPlotRad;
  int mXCtr;
  int mYCtr;
  //
      QPoint mCellPt[4];
      //
      double mLastAzimuthDeg;

      double mLastHeadingDeg;




      double mDisplayPxKtsRatio;

  //

      QElapsedTimer mElapsedTimer;
      qint64 mElapsedTime;
      qint64 mElapsedMonitorCnt;
      qint64 mElapsedAverageTimeMs;
  //
  T_FifoHandler mFifoObelixVideo;
  T_ObelixVideoMessage* mFifoVideoPtr;
  uint mFifoObelixVideoLoad;

  //
  T_FifoHandler mFifoObelixTrack;
  T_ObelixTrackReportMessage* mFifoTrackPtr;
  uint mFifoObelixTrackLoad;

  //
  T_FifoHandler mFifoObelixMap;
  T_ObelixMapMessage* mFifoMapPtr;
  uint mFifoObelixMapLoad;

  double  mLastMapPlatformLatitude;
  double  mLastMapPlatformLongitude;
  double  mLastMapPlatformHeading;





  //
  QHash<uint16_t, T_PlotTrack> mTrackTable;
  qint64                       mLastTrackPlotUpdate;


  QHash<uint16_t, T_PlotMap> mMapTable;



  bool mDisplayAntenna;
  bool mDisplayRangeLimit;
  bool mDisplayRangeRings;
  bool mDisplayCompas;
  bool mDisplayVideo;
  bool mDisplayTracks;
  bool mDisplayAircraft;
  bool mDisplayHeading;
  bool mNorthUp;

  QColor mColorAntenna;
  QColor mColorRangeLimit;
  QColor mColorRangeRings;
  QColor mColorCompas;
  QColor mColorVideo;
  QColor mColorTracks;
  QColor mColorAircraft;
  QColor mColorHeading;

  bool mNeedToPaintInfo;




};

#endif // OBELIXPLOTWIDGET_H
