#ifndef SPIMHUB_H
#define SPIMHUB_H

#include <QObject>
#include <QThread>

#include "savestackworker.h"
#include "statemachine.h"
#include "orcaflash.h"

class SPIMHub : public QObject
{
    Q_OBJECT
public:
    virtual ~SPIMHub();
    static SPIMHub* getInstance();

// C++ 11
#if __cplusplus >= 201103L
    SPIMHub(SPIMHub const &) = delete;
    void operator=(SPIMHub const &) = delete;
#else
    // C++ 03
    // Dont implement copy constructor and assignment operator
    SPIMHub(SPIMHub const &);
    void operator=(SPIMHub const &);
#endif

    void initialize();

    OrcaFlash *camera();
    void setCamera(OrcaFlash *camera);

    StateMachine *stateMachine();

signals:
    void initialized();
    void captureStarted();
    void stopped();

public slots:
    void startFreeRun();
    void startAcquisition();
    void stop();

private:
    SPIMHub();

    static SPIMHub* inst;

    OrcaFlash *orca;
    QThread *thread;
    StateMachine *sm;
    SaveStackWorker *worker;
};

#endif // SPIMHUB_H
