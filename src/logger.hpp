#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT
    bool m_verbose;
    explicit Logger(bool verbose, QObject *parent = nullptr);
public:
    enum class TYPE { INFO = 0, WARNING, ERROR, FATAL };
    static Logger *instance(bool verbose = false);
    void setVerbose(bool verbose);
    void log(QString message, TYPE type = TYPE::INFO);
};

#endif // LOGGER_HPP
