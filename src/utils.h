#ifndef APPSTORE_UTILS_H
#define APPSTORE_UTILS_H

#include "common.h"

#include <QString>
#include <QFile>
#include <QProcess>
#include <QLocale>
#include <QTimeZone>
#include <QDateTime>
#include <QNetworkInterface>
#include <QSettings>

using namespace StoreService;

enum SupFeature : int {
    Unknown = 0b0,
    SupportSpecificDeb = 0b1,
    SupportSpecificLinglong = 0b10,
    SupportEduCloudPlatform = 0b100,
    SupportDetailPageCards = 0b1000,
    SupportLinglong = 0b10000,
    SupportNewLinglong = 0b100000
};
Q_DECLARE_FLAGS(SupFeatures, SupFeature)
Q_DECLARE_OPERATORS_FOR_FLAGS(SupFeatures)

class Utils 
{
public:
    
    static void getSystemInfo(StoreService::Request_Pulic_Header &systeminfo);

    static QString systemArch();
    static bool isWayland();
    static QString systemMode();
    static QString systemPlatform();
    static QString systemRegion();
    static QString systemLanguage();
    static QString systemMajorVersion();
    static QString systemMinorVersion();
    static QString systemOsBuild();
    static QString getAppstoreVersion();
    static QString internetMacAddress();
    static QString getSupFeatures();
    static QString getMachineId();
    static QString getChannel();
    static bool getIsChildrenGuard();
    static QString getAuthorizationState() { return "1"; }; // MOCK
    static bool isLinglongSupported() { return true; } // MOCK

public:
    static bool isExternalEnvSet;
    static Request_Pulic_Header externalEnv;
};

#endif // APPSTORE_UTILS_H
