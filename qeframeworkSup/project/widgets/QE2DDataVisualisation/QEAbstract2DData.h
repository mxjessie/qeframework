/*  QEAbstract2DData.h
 *
 *  This file is part of the EPICS QT Framework, initially developed at the
 *  Australian Synchrotron.
 *
 *  Copyright (c) 2020 Australian Synchrotron
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:
 *    Andrew Starritt
 *  Contact details:
 *    andrews@ansto.gov.au
 */

#ifndef QE_ABSTRACT_2D_DATA_H
#define QE_ABSTRACT_2D_DATA_H

#include <QList>
#include <QMenu>
#include <QString>
#include <QSize>
#include <QRect>
#include <QWidget>
#include <QEFloating.h>
#include <QEFloatingArray.h>
#include <QEFloatingFormatting.h>
#include <QEInteger.h>
#include <QEIntegerFormatting.h>
#include <QCaVariableNamePropertyManager.h>
#include <QEFrameworkLibraryGlobal.h>
#include <QEFrame.h>

/// \brief The QEAbstract2DData class.
/// This is the base class for the QESpectogram, QEWaterfall and QESurface
/// widgets. It provides data management functionaly for each sub class, and
/// is not intended to be used as a concrete class.
///
class QE_FRAMEWORK_LIBRARY_SHARED_EXPORT QEAbstract2DData : public QEFrame
{
   Q_OBJECT

   typedef QEFrame ParentWidgetClass;

public:
   // Some QEFrame properties are really not applicable to this widget hierarchy.
   // These are re-declared as DESIGNABLE false.
   // Consider spliting QEFrame into QEFrame (basic) and QEPixmapFrame - keep that for 3.8
   //
   Q_PROPERTY (bool scaledContents READ getScaledContents  WRITE setScaledContents  DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap0     READ getPixmap0Property WRITE setPixmap0Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap1     READ getPixmap1Property WRITE setPixmap1Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap2     READ getPixmap2Property WRITE setPixmap2Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap3     READ getPixmap3Property WRITE setPixmap3Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap4     READ getPixmap4Property WRITE setPixmap4Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap5     READ getPixmap5Property WRITE setPixmap5Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap6     READ getPixmap6Property WRITE setPixmap6Property DESIGNABLE false)
   Q_PROPERTY (QPixmap pixmap7     READ getPixmap7Property WRITE setPixmap7Property DESIGNABLE false)

   /// EPICS main data variable name.
   ///
   Q_PROPERTY (QString dataVariable  READ getDataPvName     WRITE setDataPvName)

   /// Data width variable name - needed when data format is 2D, otherwise ignored.
   ///
   Q_PROPERTY (QString widthVariable READ getWidthPvName    WRITE setWidthPvName)

   /// Default macro substitutions. The default is no substitutions.
   /// The format is NAME1=VALUE1[,] NAME2=VALUE2...
   /// Values may be quoted strings. For example, 'PUMP=PMP3, NAME = "My Pump"'
   /// These substitutions are applied to variable names for all QE widgets.
   /// In some widgets are are also used for other purposes.
   ///
   Q_PROPERTY (QString variableSubstitutions
               READ getVariableNameSubstitutions
               WRITE setVariableNameSubstitutions)

   /// If a width variable is NOT specified, the data width may be set as a property.
   /// If defined, the widthVariable always overrides this value.
   /// Constrained to be >= 1. Default 100
   ///
   Q_PROPERTY (int dataWidth         READ getDataWidth      WRITE setDataWidth)

   /// \enum DataFormats
   /// Defines whether the source data is treated as a 1D array oe a 2D array.
   /// When source data is defined as 1D, the widget accumulates data on a FIFO
   /// basis, similar to the compress record, upto a maximum of numberOfSets
   /// "rows" of data. The accumulated data is then treated as a 2D array.
   /// Default: array2D
   ///
   enum DataFormats {
      array1D,    ///<
      array2D     ///<
   };
   Q_ENUMS(DataFormats)

   Q_PROPERTY (DataFormats dataFormat   READ getDataFormat  WRITE setDataFormat)

   /// Specifies the number of data sets to accumulate when the dataFormat
   /// property is set to array1D, otherwise it is ignored.
   /// Allowed range: 1 .. 1024
   /// Default: 40
   ///
   Q_PROPERTY (int numberOfSets      READ getNumberOfSets   WRITE setNumberOfSets)

   /// Data display options - order is slice, rotate then flip.
   ///
   /// Slice properties
   /// These properties can be negative, which interpreted as dimension number - abs(value)
   ///
   Q_PROPERTY (int verticalSliceFirst    READ getVerticalSliceFirst    WRITE setVerticalSliceFirst)
   Q_PROPERTY (int verticalSliceLast     READ getVerticalSliceLast     WRITE setVerticalSliceLast)
   Q_PROPERTY (int horizontalSliceFirst  READ getHorizontalSliceFirst  WRITE setHorizontalSliceFirst)
   Q_PROPERTY (int horizontalSliceLast   READ getHorizontalSliceLast   WRITE setHorizontalSliceLast)

   // The rptation and flip properties/meanings cribbed from QEImage.
   // Note: One 4-way rotation and two 2-way flips is apparently 4x2x2 = 16 options.
   // However in reality, there is redundancy here - there are only 8 distinct
   // rotate/flip options. We follow the QEImage paradigm for both consistancy
   // and probably less mental gymnastics for the user.
   //
   /// \enum RotationOptions
   enum RotationOptions {
      NoRotation,              ///< No data rotation
      Rotate90Right,           ///< Rotate data 90 degrees clockwise
      Rotate180,               ///< Rotate data 180 degrees
      Rotate90Left             ///< Rotate data 90 degrees anti-clockwise
   };
   Q_ENUMS(RotationOptions)

   /// Data rotation option.
   ///
   Q_PROPERTY (RotationOptions rotation  READ getRotation              WRITE setRotation)

   /// If true, flip image vertically.
   ///
   Q_PROPERTY (bool verticalFlip         READ getVerticalFlip          WRITE setVerticalFlip)

   /// If true, flip image horizontally.
   ///
   Q_PROPERTY (bool horizontalFlip       READ getHorizontalFlip        WRITE setHorizontalFlip)

   Q_PROPERTY (bool autoScale            READ getAutoScale             WRITE setAutoScale)
   Q_PROPERTY (double minimum            READ getMinimum               WRITE setMinimum)
   Q_PROPERTY (double maximum            READ getMaximum               WRITE setMaximum)

public:
   enum Constants {
      DATA_PV_INDEX = 0,
      WIDTH_PV_INDEX = 1
   };

   // QEAbstract2DData context menu values
   //
   enum OwnContextMenuOptions {
      A2DDCM_NONE = CM_SPECIFIC_WIDGETS_START_HERE,
      A2DDCM_NO_ROTATION,
      A2DDCM_ROTATE_90_RIGHT,
      A2DDCM_ROTATE_180,
      A2DDCM_ROTATE_90_LEFT,
      A2DDCM_VERTICAL_FLIP,
      A2DDCM_HORIZONTAL_FLIP,
      A2DDCM_SUB_CLASS_WIDGETS_START_HERE
   };

   /// Create without a variable.
   /// Use setDataPvName () and setVariableNameSubstitutions() to define a
   /// variable and, optionally, macro substitutions later.
   ///
   explicit QEAbstract2DData (QWidget* parent = 0);

   /// Create with a variable (s).
   /// A connection is automatically established.
   /// If macro substitutions are required, create without a variable and
   /// set the variable and macro substitutions after creation.
   ///
   explicit QEAbstract2DData (const QString& dataVariableName,
                              QWidget* parent = 0);

   explicit QEAbstract2DData (const QString& dataVariableName,
                              const QString& widthVariableName,
                              QWidget* parent = 0);

   /// Destruction
   virtual ~QEAbstract2DData ();

   int getEffectiveDataWidth () const;

   // Property access functions.
   //
   void setDataPvName (const QString dataVariable);
   QString getDataPvName () const;

   void setWidthPvName (const QString widthVariable);
   QString getWidthPvName () const;

   void setVariableNameSubstitutions (const QString variableSubstitutions);
   QString getVariableNameSubstitutions () const;

public slots:
   // All non-PV name related property setters are also slots.
   //
   void setDataWidth (const int dataWidth);
   void setDataFormat (const DataFormats dataFormat);
   void setNumberOfSets (const int numberOfSets);
   void setVerticalSliceFirst (const int first);
   void setVerticalSliceLast (const int last);
   void setHorizontalSliceFirst (const int first);
   void setHorizontalSliceLast (const int last);
   void setRotation (const RotationOptions rotation);
   void setVerticalFlip (const bool verticalFlip);
   void setHorizontalFlip (const bool horizontalFlip);
   void setAutoScale (const bool autoScale);
   void setMinimum (const double minimum);
   void setMaximum (const double maximum);

public:
   int getDataWidth () const;
   DataFormats getDataFormat () const;
   int getNumberOfSets () const;
   int getVerticalSliceFirst () const;
   int getVerticalSliceLast () const;
   int getHorizontalSliceFirst () const;
   int getHorizontalSliceLast () const;
   RotationOptions getRotation() const;
   bool getVerticalFlip () const;
   bool getHorizontalFlip () const;
   bool getAutoScale () const;
   double getMinimum () const;
   double getMaximum () const;

signals:
   // This signal is emitted using the QEEmitter::emitDbConnectionChanged function.
   /// Sent when the widget state updated following a channel connection change
   /// Applies to every varible.
   ///
   void dbConnectionChanged (const bool& isConnected);

   /// Sent when the widget is updated following a data change
   /// Can be used to pass on data update 'event' to other widgets.
   ///
   void dbValueChanged ();

protected:
   // Override parent virtual functions.
   //
   // Drag and Drop
   //
   void dragEnterEvent (QDragEnterEvent* event) { this->qcaDragEnterEvent (event);  }
   void dropEvent (QDropEvent* event)           { this->qcaDropEvent (event);       }
   void mousePressEvent (QMouseEvent* event)    { this->qcaMousePressEvent (event); }

   // Use default getDrop/setDrop

   // Copy paste
   //
   QString copyVariable ();
   QVariant copyData ();
   void paste (QVariant s);

   // override QEWidget functions.
   //
   qcaobject::QCaObject* createQcaItem (unsigned int variableIndex);
   void establishConnection (unsigned int variableIndex);

   QMenu* buildContextMenu ();                        // Build the specific context menu
   void contextMenuTriggered (int selectedItemNum);   // An action was selected from the context menu

   // Data is held as a list of vectors.
   // For 2D data, there is only one vector in the list.
   //
   typedef QList<QEFloatingArray> TwoDimensionalData;

   TwoDimensionalData getData () const;

   // This function returns value at the displayed row and col position.
   // It use the slicing, rotation, and flip states to extract the underlying
   // data value.
   // If row or col out of range of the available data, this function
   // returns the specified defaultValue.
   //
   double getValue (const int displayRow, const int displayCol,
                    const double defaultValue) const;

   int getUpdateCount () const;

   // Determines and updates min and max values if data available, otherwise
   // min and max left "as is", i.e. unchanged. Therefore caller should supply
   // sensible default.
   //
   void getDataMinMaxValues (double& min, double& max) const;

   // This function returns the number of displayed rows and cols.
   // When 1D data is being accumulated, the number of rows (or cols) is the
   // potential number, not the number so far accumumated.
   // Takes into account slicing and if/when a 90 degree rotation selected,
   // switched the number of rows and cols.
   //
   void getNumberRowsAndCols (int& numberRows, int& numberCols) const;

   // Get the data engineering units and precision.
   //
   QString getUnits() const;
   int getPrecision() const;

   void setReadOut (const QString& text);
   void setElementReadout (const int row, const int col);

   virtual void updateDataVisulation ();   // hook function

private:
   void commonSetup ();
   void calculateDataVisulationValues ();

   QCaVariableNamePropertyManager dnpm;   // data name
   QCaVariableNamePropertyManager wnpm;   // width name

   QEFloatingFormatting floatingFormatting;
   QEIntegerFormatting integerFormatting;

   // Property members
   //
   int mDataWidth;

   // Slice parameters.
   //
   int mVerticalSliceFirst;    // row first
   int mVerticalSliceLast;     // row last

   int mHorizontalSliceFirst;  // col first
   int mHorizontalSliceLast;   // col last

   RotationOptions mRotation;
   bool mVerticalFlip;
   bool mHorizontalFlip;
   bool mAutoScale;
   double mMinimum;
   double mMaximum;
   DataFormats mDataFormat;
   int mNumberOfSets;

   // When mDataFormat is array2D, this is limited to one QEFloatingArray
   // When mDataFormat is array1D, is limited to mNumberOfSets QEFloatingArrays.
   //
   TwoDimensionalData data;
   int updateCount;

   bool pvDataWidthAvailable;
   int  pvDataWidth;

   // data values
   //
   int rawNumberOfRows;         // number of available/potential rows of data
   int rawNumberOfCols;         // number of columns of data

   int sliceRowOffset;          // slice offset
   int sliceColOffset;          // slice offset

   int slicedNumberOfRows;      // number rows of data after sliceing
   int slicedNumberOfCols;      // number cols of data after sliceing

   int displayedNumberOfRows;   // number rows of data after any rotation
   int displayedNumberOfCols;   // number cols of data after any rotation

private slots:
   void setVariableNameProperty (QString variableName,
                                 QString variableNameSubstitutions,
                                 unsigned int variableIndex);

   void connectionChanged (QCaConnectionInfo& connectionInfo,
                           const unsigned int& variableIndex);

   void onDataArrayUpdate (const QVector<double>& values, QCaAlarmInfo&,
                           QCaDateTime&, const unsigned int&);

   void onWidthUpdate (const long value, QCaAlarmInfo&,
                       QCaDateTime&, const unsigned int&);
};

#ifdef QE_DECLARE_METATYPE_IS_REQUIRED
Q_DECLARE_METATYPE (QEAbstract2DData::DataFormats)
Q_DECLARE_METATYPE (QEAbstract2DData::RotationOptions)
#endif

#endif // QE_ABSTRACT_2D_DATA_H
