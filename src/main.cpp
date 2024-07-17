#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QSharedPointer>

#include <unistd.h>

#include "logger.hpp"
#include "writer.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(PROJECT_NAME);
    QCoreApplication::setApplicationVersion(PROJECT_VERSION);
    QCoreApplication a(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription(PROJECT_DESCRIPTION);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "portagecfg_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
        }
    }

    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption(
        QStringList() << "E" << "env-global",
        QObject::tr("Write given env config to Portage's global env folder."),
        "env")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "e" << "env-package",
        QObject::tr("Write given env config to Portage's package env folder."
                    "\n"
                    "Note that given env config should exist in /etc/portage/env for portage to work as expected!"),
        "env")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "f" << "filename",
        QObject::tr("Optional filename to write configuration to. Package name will be used if not specified."),
        "filename")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "k" << "keywords",
        QObject::tr("Write given keyword to Portage's keywords folder. Pass - for ~amd64."),
        "keywords")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "l" << "licenses",
        QObject::tr("Write given license to Portage's licenses folder."),
        QObject::tr("licenses"))
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "m" << "mask",
        QObject::tr("Mask given package."))
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "p" << "package",
        QObject::tr("The package in question. This option is mandatory."), "category/packageName")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "u" << "useflags",
        QObject::tr("Write given USE flags to Portage's USE flags folder."),
        "USE flags")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "U" << "unmask",
        QObject::tr("Unmask given package."))
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "V" << "verbose",
        QObject::tr("Describe every given step."))
    );

    parser.process(a);

    bool verbose = parser.isSet("verbose");

    auto logger = QSharedPointer<Logger>(Logger::instance(verbose));

    auto username = qgetenv("USER");
    if (username.isEmpty()) {
        username = qgetenv("USERNAME");
    }

    if (geteuid() != 0) {
        logger->log(
            QObject::tr("Superuser rights needed. Portage's directories belong to root. "
                        "Can't write there as %1.").arg(username),
            Logger::TYPE::FATAL
        );
    }

    QString package = parser.value("package");
    if (package.isEmpty() and not parser.isSet("env-global")) {
        logger->log(QObject::tr("Package wasn't provided."), Logger::TYPE::FATAL);
    }

    if (parser.isSet("mask") and parser.isSet("unmask")) {
        logger->log(
            QObject::tr("Both mask and unmask options were provided where just one at a time is allowed."),
            Logger::TYPE::FATAL
        );
    }

    Writer writer(package, parser.value("filename"));
    QObject::connect(&writer, &Writer::done, &a, &QCoreApplication::quit, Qt::QueuedConnection);

    if (parser.isSet("env-global")) {
        if (not parser.isSet("filename")) {
            logger->log(QObject::tr("Filename must be provided."), Logger::TYPE::FATAL);
        }

        writer.write(Writer::TYPE::GLOBAL_ENV, parser.value("env-global"));
    }

    if (parser.isSet("env-package")) {
        writer.write(Writer::TYPE::PACKAGE_ENV, parser.value("env-package"));
    }

    if (parser.isSet("keywords")) {
        auto keywords = parser.value("keywords");
        if (keywords == "-") {
            keywords = "~amd64";
        }

        writer.write(Writer::TYPE::KEYWORD, keywords);
    }

    if (parser.isSet("licenses")) {
        writer.write(Writer::TYPE::LICENSE, parser.value("licenses"));
    }

    if (parser.isSet("mask")) {
        writer.write(Writer::TYPE::MASK);
    } else if (parser.isSet("unmask")) {
        writer.write(Writer::TYPE::UNMASK);
    }

    if (parser.isSet("useflags")) {
        writer.write(Writer::TYPE::USEFLAGS, parser.value("useflags"));
    }

    return a.exec();
}
