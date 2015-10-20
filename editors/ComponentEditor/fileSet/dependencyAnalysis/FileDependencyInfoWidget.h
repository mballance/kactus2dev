//-----------------------------------------------------------------------------
// File: FileDependencyInfoWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M��tt�, Tommi Korhonen
// Date: 19.01.2013
//
// Description:
// Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCYINFOWIDGET_H
#define FILEDEPENDENCYINFOWIDGET_H

#include <QGroupBox>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class FileDependency;

//-----------------------------------------------------------------------------
//! Widget for showing information about one file dependency.
//-----------------------------------------------------------------------------
class FileDependencyInfoWidget : public QGroupBox
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    FileDependencyInfoWidget(QWidget* parent );

    /*!
     *  Destructor.
     */
    ~FileDependencyInfoWidget();

    /*!
     *  Returns the currently edited dependency.
     */
    QSharedPointer<FileDependency> getEditedDependency() const;

signals:

    /*!
     *  Emitted when the dependency has been changed.
     */
    void dependencyChanged(QSharedPointer<FileDependency> dependency);

public slots:
    /*!
     *  Sets the given file dependency editable in the widget.
     *
     *      @param [in] dependency The dependency to set for editing.
     *
     *      @remarks If the dependency is null, all contents of the widgets are cleared and
     *               the widgets are set to disabled mode.
     */
    void setEditedDependency(QSharedPointer<FileDependency> dependency);

private slots:

    /*!
     *  Handles a change in the direction..
     *
     *      @param [in] state   The new state for the direction.
     */
    void directionCheckBoxChanged(int state);

    /*!
     *  Handles the change in the locked check.
     *
     *      @param [in] state   The new state.
     */
    void lockedCheckChanged(int state);
    
    /*!
     *  Handles the change in the description change.
     */
    void descEditTextChanged();

    /*!
     *  Handles the reversing of the direction.
     */
    void directionReversed();

private:
    // Disable copying.
    FileDependencyInfoWidget(FileDependencyInfoWidget const& rhs);
    FileDependencyInfoWidget& operator=(FileDependencyInfoWidget const& rhs);

    /*!
     *  Update the file label.
     */
    void updateFileLabel();

    /*!
     *  Locks / unlocks everything.
     *
     *      @param [in] isLocked    The lock status.
     */
    void lockEverything(bool isLocked);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Editor for description.
    QPlainTextEdit descEdit_;

    //! Check box for changing whether direction is bidirectional.
    QCheckBox directionCheck_;
    
    //! Check box for locked state.
    QCheckBox lockedCheck_;

    //! Button for reversing the direction.
    QPushButton directionButton_;

    //! The currently edited dependency.
    QSharedPointer<FileDependency> dependency_;
};

//-----------------------------------------------------------------------------

#endif // FILEDEPENDENCYINFOWIDGET_H
