#include "logger.hpp"

#include <QDebug>

Logger::Logger(bool verbose, QObject *parent)
    : QObject{parent}
    , m_verbose(verbose)
{

}

Logger *Logger::instance(bool verbose)
{
    static Logger *logger;
    if (not logger) {
        logger = new Logger(verbose);
    }
    return logger;
}

void Logger::setVerbose(bool verbose)
{
    m_verbose = verbose;
}

void Logger::log(QString message, TYPE type)
{
    if (not m_verbose and type != TYPE::FATAL) {
        return;
    }

    switch (type)
    {
    case TYPE::INFO:
        message.insert(0, QString("%1INFO:%2 ").arg(WHITE, NO_COLOR));
        qInfo().noquote() << message;
        break;
    case TYPE::WARNING:
        message.insert(0, QString("%1WARN:%2 ").arg(YELLOW, NO_COLOR));
        qWarning().noquote() << message;
        break;
    case TYPE::ERROR:
        message.insert(0, QString("%1ERROR:%2 ").arg(RED, NO_COLOR));
        qCritical().noquote() << message;
        break;
    case TYPE::FATAL:
        message.insert(0, QString("%1FATAL:%2 ").arg(RED, NO_COLOR));
        qCritical().noquote() << message;
        std::exit(1);
        break;
    }
}
