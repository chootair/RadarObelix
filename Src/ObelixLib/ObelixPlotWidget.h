#ifndef OBELIXPLOTWIDGET_H
#define OBELIXPLOTWIDGET_H

#include <QOpenGLWidget>
#include <QPainter>
#include <QImage>
#include <QLabel>
#include <QElapsedTimer>
#include <QReadWriteLock>
#include <QDateTime>

#include "Obelix.h"
#include "PresistImage.h"


typedef struct _T_FifoHandler
{
  uint*           mFifoIndexPtr;
  uint            mFifoSize;
  uint            mMessageSize;
  char*           mFifoPtr;
  QReadWriteLock* mFifoLocker;
}T_FifoHandler;


typedef struct _T_PlotTrack
{
  T_ObelixTrack Track;
  qint64        LastUpdate;
}T_PlotTrack;




class ObelixPlotWidget : public QOpenGLWidget
{
  Q_OBJECT
public:
  explicit ObelixPlotWidget(QWidget *parent = nullptr);
  ~ObelixPlotWidget();
  void RefreshScope();
  void ClearScope();

  void SetPresistenceRatio(double pRatio);
  inline double PresistenceRatio() const { return mPersistRatio;}


  inline double RangeNm() const {return mRangeNm;}
   inline void SetRangeNm(double pRangeNm) {mRangeNm = pRangeNm;}




  inline qint64 ElapsedAverageTimeMs() const {return mElapsedAverageTimeMs;}
  //
  inline uint   FifoObelixVideoLoad() const {return mFifoObelixVideoLoad;}
  inline T_FifoHandler   FifoObelixVideo() {return mFifoObelixVideo;}
  //
  inline uint   FifoObelixTrackLoad() const {return mFifoObelixTrackLoad;}
  inline T_FifoHandler   FifoObelixTrack() {return mFifoObelixTrack;}


  //
  inline double DisplayPxKtsRatio() const  {return mDisplayPxKtsRatio;}
  inline void SetDisplayPxKtsRatio(double pDisplayPxKtsRatio) {mDisplayPxKtsRatio=pDisplayPxKtsRatio;}



  inline bool DisplayAntenna   () const {return mDisplayAntenna;}
  inline bool DisplayRangeLimit() const {return mDisplayRangeLimit;}
  inline bool DisplayRangeRings() const {return mDisplayRangeRings;}
  inline bool DisplayCompas    () const {return mDisplayCompas;}
  inline bool DisplayVideo     () const {return mDisplayVideo;}
  inline bool DisplayTracks    () const {return mDisplayTracks;}
  inline void SetDisplayAntenna   (bool pEnable) {mDisplayAntenna = pEnable;}
  inline void SetDisplayRangeLimit(bool pEnable) {mDisplayRangeLimit = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayRangeRings(bool pEnable) {mDisplayRangeRings = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayCompas    (bool pEnable) {mDisplayCompas = pEnable; mNeedToPaintInfo=true;}
  inline void SetDisplayVideo     (bool pEnable) {mDisplayVideo = pEnable;}
  inline void SetDisplayTracks    (bool pEnable) {mDisplayTracks = pEnable;}



  inline QColor ColorAntenna   () const {return mColorAntenna;}
  inline QColor ColorRangeLimit() const {return mColorRangeLimit;}
  inline QColor ColorRangeRings() const {return mColorRangeRings;}
  inline QColor ColorCompas    () const {return mColorCompas;}
  inline QColor ColorVideo     () const {return mColorVideo;}
  inline QColor ColorTracks    () const {return mColorTracks;}

  inline void  SetColorAntenna   (QColor pColor) {mColorAntenna    = pColor;}
  inline void  SetColorRangeLimit(QColor pColor) {mColorRangeLimit = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorRangeRings(QColor pColor) {mColorRangeRings = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorCompas    (QColor pColor) {mColorCompas     = pColor; mNeedToPaintInfo=true;}
  inline void  SetColorVideo     (QColor pColor) {mColorVideo      = pColor;}
  inline void  SetColorTracks    (QColor pColor) {mColorTracks     = pColor;}


protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  void PaintControl();
  void SetMyGeometry();
  void PaintVideoCells(QPainter* pPainter);
  void PaintTrackPlots(QPainter* pPainter);
  void PaintTools(QPainter* pPainter);
  int ReadTrackPlots();

private:

  QImage* mScopeVideoImg;
  QImage* mScopeTrackImg;
  QImage* mToolsImg;
  QImage* mWidgetImg;
  GreenPresistImage* mPersistImg;

  //bool mIsPersistEnabled;
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
  QHash<uint16_t, T_PlotTrack> mTrackTable;
  qint64                       mLastTrackPlotUpdate;



  bool mDisplayAntenna;
  bool mDisplayRangeLimit;
  bool mDisplayRangeRings;
  bool mDisplayCompas;
  bool mDisplayVideo;
  bool mDisplayTracks;

  QColor mColorAntenna;
  QColor mColorRangeLimit;
  QColor mColorRangeRings;
  QColor mColorCompas;
  QColor mColorVideo;
  QColor mColorTracks;


  bool mNeedToPaintInfo;


};

#endif // OBELIXPLOTWIDGET_H
