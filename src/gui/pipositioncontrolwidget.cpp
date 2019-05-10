#include <QPushButton>
#include <QMessageBox>
#include <QLabel>
#include <QState>
#include <QTimer>

#include "core/pidevice.h"
#include "core/spim.h"

#include "pipositioncontrolwidget.h"
#include "customspinbox.h"


PIPositionControlWidget::PIPositionControlWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUI();
}

void PIPositionControlWidget::setupUI()
{
    grid = new QGridLayout();

    gb = new QGroupBox();
    gb->setLayout(grid);

    int col = 1;
    grid->addWidget(new QLabel("Curr. Pos."), row, col);
    col += 2;
    grid->addWidget(new QLabel("Set Pos."), row, col++);
    grid->addWidget(new QLabel("Step down"), row, col++);
    grid->addWidget(new QLabel("Step up"), row, col++);

    QFrame *line;
    line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    grid->addWidget(line, row, col++);

    grid->addWidget(new QLabel("Step size"), row, col++);
    grid->addWidget(new QLabel("Velocity"), row, col++);

    row++;

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(gb);
    setLayout(vLayout);
}

void PIPositionControlWidget::appendRow(
    SPIM_PI_DEVICES d_enum, const QString &axis, const QString &axisName)
{
    PIDevice *device = spim().getPIDevice(d_enum);
    int col = 0;
    grid->addWidget(new QLabel(axisName), row, col++);
    QLabel *currentPos = new QLabel("0.000");
    grid->addWidget(currentPos, row, col++);
    QString s = "QPushButton {color: red;}";
    QPushButton *haltPushButton = new QPushButton("HALT");
    haltPushButton->setStyleSheet(s);
    grid->addWidget(haltPushButton, row, col++);

    DoubleSpinBox *sb = new DoubleSpinBox();
    sb->setDecimals(4);
    grid->addWidget(sb, row, col++);

    QPushButton *minusPushButton = new QPushButton("-");
    grid->addWidget(minusPushButton, row, col++);

    QPushButton *plusPushButton = new QPushButton("+");
    grid->addWidget(plusPushButton, row, col++);

    QFrame *line;
    line = new QFrame();
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    grid->addWidget(line, row, col++);

    QDoubleSpinBox *stepSpinBox = new QDoubleSpinBox();
    stepSpinBox->setValue(0.1);
    stepSpinBox->setDecimals(4);
    grid->addWidget(stepSpinBox, row, col++);

    DoubleSpinBox *velocitySpinBox = new DoubleSpinBox();
    velocitySpinBox->setValue(1);
    velocitySpinBox->setDecimals(4);
    grid->addWidget(velocitySpinBox, row, col++);

    row++;

    QState *cs = device->getConnectedState();
    QState *ds = device->getDisconnectedState();

    QList<QWidget *> wList;

    // enabled when connected, disabled when disconnected
    wList = {
        haltPushButton,
        sb,
        minusPushButton,
        plusPushButton,
        stepSpinBox,
        velocitySpinBox,
    };

    for (QWidget * w : wList) {
        cs->assignProperty(w, "enabled", true);
        ds->assignProperty(w, "enabled", false);
    }

    connect(haltPushButton, &QPushButton::clicked, this, [ = ](){
        try {
            device->halt(axis);
            device->getError();
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });

    connect(sb, &DoubleSpinBox::returnPressed, this, [ = ](){
        double pos = sb->value();
        double vel = velocitySpinBox->value();
        try {
            device->setVelocities(axis, &vel);
            device->move(axis, &pos);
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });

    connect(plusPushButton, &QPushButton::clicked, this, [ = ](){
        device->setStepSize(axis, stepSpinBox->value());
        try {
            device->stepUp(axis);
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });
    connect(minusPushButton, &QPushButton::clicked, this, [ = ](){
        device->setStepSize(axis, stepSpinBox->value());
        try {
            device->stepUp(axis);
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });

    connect(velocitySpinBox, &DoubleSpinBox::returnPressed, this, [ = ](){
        double vel = velocitySpinBox->value();
        try {
            device->setVelocities(axis, &vel);
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });

    QTimer *updateTimer = new QTimer(this);
    updateTimer->setInterval(500);

    connect(updateTimer, &QTimer::timeout, this, [ = ]() {
        try {
            double pos = device->getCurrentPosition(axis).at(0);
            currentPos->setText(QString("%1").arg(pos, 0, 'f', 4));
        }
        catch (std::runtime_error e) {
            QMessageBox::critical(nullptr, "Error", e.what());
        }
    });

    connect(device, &PIDevice::connected, this, [ = ](){
        double min = device->getTravelRangeLowEnd(axis).at(0);
        double max = device->getTravelRangeHighEnd(axis).at(0);
        double velocity = device->getVelocities(axis).at(0);
        double pos = device->getCurrentPosition(axis).at(0);

        sb->setRange(min, max);
        sb->setValue(pos);
        stepSpinBox->setRange(0, max);
        velocitySpinBox->setValue(velocity);

        updateTimer->start();
    });

    connect(device, &PIDevice::disconnected, updateTimer, &QTimer::stop);
}

void PIPositionControlWidget::setTitle(const QString &title)
{
    gb->setTitle(title);
}
