#ifndef WRITER_HPP
#define WRITER_HPP

#include <map>
#include <QObject>

#include "logger.hpp"

class Writer : public QObject
{
    Q_OBJECT
public:
    enum class TYPE { NOTHING = -2, ALL, GLOBAL_ENV, PACKAGE_ENV, LICENSE, KEYWORD, MASK, USEFLAGS, UNMASK };

    explicit Writer(const QString &package,
            const QString &repo = QString(),
            const QString &filename = QString(),
            const QString &comment = QString(),
            const QVector<Writer::TYPE> &where = QVector<Writer::TYPE>(),
            QObject *parent = nullptr);

    void setFilename(const QString &filename);
    void setValues(const QVector<QString> &values);
    void write(TYPE type, const QString &values = QString());
private:
    QString m_filename;
    QString m_package;
    QString m_repo;
    QString m_comment;
    QVector<Writer::TYPE> m_where;
    std::map<TYPE, QString> m_folders;
    Logger *m_logger;
signals:
    void done();
};

#endif // WRITER_HPP
