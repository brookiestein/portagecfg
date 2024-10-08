#include <iostream>
#include <string>

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDirIterator>
#include <QLocale>
#include <QProcess>
#include <QRegularExpression>
#include <QSet>
#include <QSharedPointer>
#include <QTranslator>

#include <unistd.h>

#include "logger.hpp"
#include "writer.hpp"

bool fixPackageName(QString &packageName, Logger *logger);
QVector<Writer::TYPE> parseCommentWriter(const QString &places,
                                         Logger *logger,
                                         const QCommandLineParser &parser);

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
        QStringList() << "a" << "args",
        QObject::tr("Optional Portage arguments. Default is: --ask=n."),
        "args")
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "c" << "comment",
        QObject::tr("Optionally write a comment for this configuration."),
        "comment")
    );

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
        QStringList() << "r" << "run-portage",
        QObject::tr("Run 'emerge <args> --ask=n category/package' after writing given configuration."))
    );

    parser.addOption(QCommandLineOption(
        QStringList() << "R" << "repo",
        QObject::tr("Optionally be more specific about what repository is the package in."),
        QObject::tr("repo-name"))
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

    parser.addOption(QCommandLineOption(
        QStringList() << "w" << "where",
        QObject::tr("Where to write comment specified with -c. "
                    "Available options: all, env-global, keywords, env-package, licenses, mask, unmask, useflags. "
                    "Options can be combined."),
        "any-option")
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

    if (not fixPackageName(package, logger.data())) {
        logger->log(
            QObject::tr("Package: %1 wasn't found on your system.").arg(package),
            Logger::TYPE::FATAL
        );
    }

    auto repo = parser.value("repo");
    auto filename = parser.value("filename");
    auto comment = parser.value("comment");
    QVector<Writer::TYPE> where;

    if (not comment.isEmpty()) {
        if (parser.isSet("where")) {
            where = parseCommentWriter(parser.value("where"), logger.data(), parser);
        } else {
            logger->log(
                QObject::tr("No place to write comment to was specified, defaulting to everywhere."),
                Logger::TYPE::WARNING
            );

            where.push_back(Writer::TYPE::ALL);
        }
    } else if (parser.isSet("where")) {
        logger->log(
            QObject::tr("-w/--where specified without specifying -c/--comment. No comment will be written anywhere."),
            Logger::TYPE::WARNING
        );
    }

    Writer writer(package, repo, filename, comment, where);
    /* Let Writer::done end program execution only when run-portage is not set. If it is, it'll take control of that. */
    if (not parser.isSet("run-portage")) {
        QObject::connect(&writer, &Writer::done, &a, &QCoreApplication::quit, Qt::QueuedConnection);
    }

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

    QProcess process;

    if (parser.isSet("run-portage")) {
        QString program {"emerge"};
        QStringList arguments;
        if (parser.isSet("args")) {
            arguments << parser.value("args").split(" ");
        }
        arguments << "--ask=n" << package;

        QObject::connect(&process, &QProcess::finished, &a, &QCoreApplication::quit, Qt::QueuedConnection);

        QObject::connect(&process, &QProcess::stateChanged, [&] (QProcess::ProcessState newState) {
            switch (newState)
            {
            case QProcess::NotRunning:
                if (process.exitStatus() == QProcess::CrashExit or process.exitCode() != 0) {
                    logger->log(
                        QObject::tr("Portage didn't finish normally, exit code: %1. Please read its log files for more info!")
                                .arg(QString::number(process.exitCode())
                        )
                    );
                } else {
                    logger->log(QObject::tr("Done!"));
                }
                break;
            case QProcess::Starting:
                logger->log(QObject::tr("Starting Portage with arguments: %1...").arg(arguments.join(" ")));
                break;
            case QProcess::Running:
                logger->log(QObject::tr("Portage's running, please wait for it to finish."));
                break;
            }
        });

        process.setProgram(program);
        process.setArguments(arguments);
        process.start();
    }

    return a.exec();
}

bool fixPackageName(QString &packageName, Logger *logger)
{
    QRegularExpression regex("\\w+/\\w+");
    if (packageName.contains(regex)) {
        logger->log(QObject::tr("Full package name provided."));
        return true;
    }

    logger->log(
        QObject::tr("Only package name was provided. Looking for full package name. Please wait a second...")
    );

    QSet<QString> matches;
    QList<QString> categories;
    regex = QRegularExpression(QString("/.+%1-[0-9]+.+ebuild$").arg(packageName));
    QDirIterator iterator(PORTAGE_REPOS_DIR, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QString path = iterator.next();
        if (path.contains(regex)) {
            /* Remove ebuild file, e.g. portagecfg-VERSION.ebuild */
            path = path.mid(0, path.lastIndexOf('/'));
            /*
             * Remove package name, normally used as a folder containing
             * all files required by Portage.
             */
            path = path.mid(0, path.lastIndexOf('/'));
            /* Get actual category getting rid of /var/db/repos/REPO/ */
            path = path.mid(path.lastIndexOf('/') + 1, path.size());
            matches.insert(path);
        }
    }

    categories = matches.values();

    if (categories.isEmpty()) {
        return false;
    } else if (categories.size() == 1) {
        packageName = QString("%1/%2").arg(categories[0], packageName);
        return true;
    }

    qInfo().noquote() << QObject::tr("Found %1 categories for package %2.")
                   .arg(QString::number(categories.size()), packageName);
    int i = 1;
    for (const auto &value : categories) {
        qInfo().noquote() << i++ << value;
    }

    std::string largeSelection;
    int shortSelection = -1;
    while (true) {
        std::cout << QObject::tr("What's the correct category for package %1? ").arg(packageName).toStdString();
        std::getline(std::cin, largeSelection);

        QString s(largeSelection.c_str());
        if (s.isEmpty()) {
            qCritical().noquote() << QObject::tr("You wrote nothing.");
            continue;
        }

        bool ok {false};
        shortSelection = s.toInt(&ok);

        if (not ok) {
            shortSelection = s.split(" ")[0].toInt(&ok);
        }

        if (ok) {
            if (shortSelection <= 0 or shortSelection > categories.size()) {
                qCritical().noquote() << QObject::tr("There isn't such a category.");
                continue;
            }

            --shortSelection; /* Remember Lists are 0-indexed? */
            break;
        }

        /* If arrived here, user didn't enter a number, let's check if large option is valid. */
        for (const auto &category : categories) {
            if (category.toLower() == s.toLower()) {
                shortSelection = categories.indexOf(category);
                ok = true;
                break;
            }
        }

        if (ok) {
            break;
        }

        qCritical().noquote() << QObject::tr("There isn't such a category.");
    }

    packageName = QString("%1/%2").arg(categories[shortSelection], packageName);

    return true;
}

QVector<Writer::TYPE> parseCommentWriter(const QString &places, Logger *logger, const QCommandLineParser &parser)
{
    QMap<QString, Writer::TYPE> values;
    values["env-global"] = Writer::TYPE::GLOBAL_ENV;
    values["keyword"] = Writer::TYPE::KEYWORD;
    values["env-package"] = Writer::TYPE::PACKAGE_ENV;
    values["licenses"] = Writer::TYPE::LICENSE;
    values["mask"] = Writer::TYPE::MASK;
    values["unmask"] = Writer::TYPE::UNMASK;
    values["use"] = Writer::TYPE::USEFLAGS;

    QVector<Writer::TYPE> where;
    QStringList listOfPlaces = places.split(" ");

    if (places.isEmpty()) {
        return {};
    }

    for (const auto &place : listOfPlaces) {
        auto value = values.value(place.toLower(), Writer::TYPE::NOTHING);
        if (value != Writer::TYPE::NOTHING) {
            if (not parser.isSet(place.toLower())) {
                logger->log(
                    QObject::tr("Place '%1' is specified, but that config isn't. Ignoring it.").arg(place),
                    Logger::TYPE::WARNING
                );
                continue;
            }

            where.push_back(value);
            continue;
        }

        logger->log(
            QObject::tr("Invalid option for writing comment to: '%1'.").arg(place),
            Logger::TYPE::ERROR
        );
    }

    if (where.isEmpty()) {
        logger->log(
            QObject::tr("No valid place to write comment to was provided. No comment will be written."),
            Logger::TYPE::WARNING
        );
    }

    return where;
}
