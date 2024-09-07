#include "writer.hpp"

#include <QDir>
#include <QFile>

Writer::Writer(const QString &package, const QString &filename, QObject *parent)
    : QObject{parent}
    , m_package(package)
    , m_filename(filename)
    , m_logger(Logger::instance())
{
    const QString baseFolder = "/etc/portage/package.";

    m_folders[Writer::TYPE::GLOBAL_ENV] = baseFolder.mid(0, baseFolder.lastIndexOf('/') + 1) + "env";
    m_folders[Writer::TYPE::PACKAGE_ENV] = baseFolder + "env";
    m_folders[Writer::TYPE::LICENSE] = baseFolder + "license";
    m_folders[Writer::TYPE::KEYWORD] = baseFolder + "accept_keywords";
    m_folders[Writer::TYPE::MASK] = baseFolder + "mask";
    m_folders[Writer::TYPE::USEFLAGS] = baseFolder + "use";
    m_folders[Writer::TYPE::UNMASK] = baseFolder + "unmask";
}

void Writer::setFilename(const QString &filename)
{
    m_filename = filename;
}

void Writer::write(Writer::TYPE type, const QString &values)
{
    if (values.isEmpty() and type != Writer::TYPE::MASK and type != Writer::TYPE::UNMASK) {
        m_logger->log(
            tr("Values weren't provided for package: %1").arg(m_package),
            Logger::TYPE::ERROR
        );
        return;
    }

    const auto folder = m_folders[type];
    {
        QDir d;
        if (not d.exists(folder)) {
            m_logger->log(tr("Directory: %1 doesn't exist. Creating it...").arg(folder));
            d.mkpath(folder);
        }
    }

    if (m_filename.isEmpty()) {
        m_logger->log(tr("Using package name as file name."));
        m_filename = m_package.mid(m_package.lastIndexOf('/') + 1, m_package.size());
    }

    auto fullPath = QString("%1%2%3").arg(folder, QDir::separator(), m_filename);
    QString contents = m_package;
    if (type != Writer::TYPE::MASK and type != Writer::TYPE::UNMASK) {
        contents += " " + values;
    }

    contents = contents.trimmed();

    m_logger->log(
        tr("Writing %1'%2'%3 to %4%5%6.")
            .arg(GREEN, contents, NO_COLOR, BLUE, fullPath, NO_COLOR)
    );

    contents += "\n";

    QFile file(QString("%1%2%3").arg(folder, QDir::separator(), m_filename));
    if (not file.open(QIODevice::Append)) {
        m_logger->log(
            tr("Could not open file: '%1' to write config to.")
                .arg(m_filename),
            Logger::TYPE::FATAL
        );
    }

    file.write(QByteArray::fromStdString(contents.toStdString()));

    if (type == TYPE::PACKAGE_ENV) {
        /* values == env file config, e.g.: lto.conf */
        auto path = QString("%1%2%3").arg(m_folders[TYPE::GLOBAL_ENV], QDir::separator(), values);
        QFile file(path);
        if (not file.exists()) {
            m_logger->log(
                tr("Env file: %1 doesn't exist. This config will do nothing if env file is not created.")
                    .arg(file.fileName()),
                Logger::TYPE::WARNING
            );
        }
    }

    emit done();
}
