#ifndef WRITER_HPP
#define WRITER_HPP

#include <map>
#include <QObject>

#include "logger.hpp"

class Writer : public QObject
{
    Q_OBJECT
public:
    enum class TYPE { GLOBAL_ENV = 0, PACKAGE_ENV, LICENSE, KEYWORD, MASK, USEFLAGS, UNMASK };
    explicit Writer(const QString &package, const QString &filename = QString(), QObject *parent = nullptr);
    void setFilename(const QString &filename);
    void setValues(const QVector<QString> &values);
    void write(TYPE type, const QString &values = QString());
private:
    QString m_filename;
    QString m_package;
    std::map<TYPE, QString> m_folders;
    Logger *m_logger;
signals:
    void done();
};

#endif // WRITER_HPP
