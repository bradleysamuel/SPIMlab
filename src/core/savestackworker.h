#ifndef SAVESTACKWORKER_H
#define SAVESTACKWORKER_H

#include <QObject>
#include <QString>

class OrcaFlash;

class SaveStackWorker : public QObject
{
    Q_OBJECT
public:
    explicit SaveStackWorker(OrcaFlash *orca, QObject *parent = nullptr);

    void setFrameCount(uint count);
    void setOutputFileName(const QString &fname);

public slots:
    void saveToFile();

signals:
    void finished();
    void error(QString msg = "");

private:
    bool stopRequested;
    QString outputFileName;
    uint frameCount;
    OrcaFlash *orca;
};

#endif // SAVESTACKWORKER_H
