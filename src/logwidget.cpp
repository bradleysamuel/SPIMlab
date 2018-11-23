#include <QVBoxLayout>
#include <QTime>
#include <QApplication>

#include "logwidget.h"


LogWidget::LogWidget(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    textEdit = new QTextEdit();
    layout->addWidget(textEdit);
    setLayout(layout);

    qApp->installEventFilter(this);
}

bool LogWidget::eventFilter(QObject *obj, QEvent *event) {
    if(event->type() == MessageEvent::TYPE) {
        MessageEvent *e = (MessageEvent*)event;
        logMessage(e->getMsg(),e->getType());
        return true;
    }
    return false;
}

void LogWidget::logMessage(QString msg, MsgType type) {
    if(msg.isEmpty())
        return;
    QString timeString = QString("[%1]").arg(QTime::currentTime().toString());
    if(msg.contains(QRegExp("<")))
        msg.replace(QRegExp("<"),"&lt;");
    if(msg.contains(QRegExp(">")))
        msg.replace(QRegExp(">"),"&gt;");
    if(msg.contains(QRegExp("\\n")))
        msg.replace(QRegExp("\\n"),QString("<br>").append(timeString));
    if(msg.contains(QRegExp("\\r")))
        msg.replace(QRegExp("\\r"),"");

    msg.prepend(timeString);
    if(type == MSG_ERROR) {
        msg.prepend("<span style='color:red'>");
        msg.append("</span>");
    }

    msg.append("<br>");

    msg.prepend("<pre style=\"margin-bottom: 0px;\">");
    msg.append("</pre>");

    textEdit->insertHtml(msg);
    textEdit->ensureCursorVisible();
}
