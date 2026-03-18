#include "utils.h"

#include <DDesktopEntry>
#include <DSysInfo>
#include <DConfig>

#include <QProcessEnvironment>
#include <QDebug>
#include <QProcess>

DCORE_USE_NAMESPACE

#define VERSION "9.0.0"

bool Utils::isExternalEnvSet = false;
Request_Pulic_Header Utils::externalEnv;

void Utils::getSystemInfo(StoreService::Request_Pulic_Header &systeminfo)
{
    systeminfo.arch = systemArch();
    systeminfo.iswayland = isWayland() ? "1" : "0";
    systeminfo.mode = systemMode();
    systeminfo.platform = systemPlatform();
#ifdef NetTestEnviro
    systeminfo.platform = "youngster";
#endif
    systeminfo.mac = getMachineId();
    systeminfo.region = systemRegion();
    systeminfo.language = systemLanguage();
    systeminfo.baseline_main_version = systemMajorVersion();
    systeminfo.baseline_version = systemMinorVersion();
    systeminfo.os_build = systemOsBuild();
    systeminfo.client_version = getAppstoreVersion();
    systeminfo.motherboard = "";
    systeminfo.cpu_clip = "";
    systeminfo.macAddress = internetMacAddress();
    systeminfo.supFeatures = getSupFeatures();
    systeminfo.templateId = "";

    // user_agent
    systeminfo.user_agent.client_version = systeminfo.client_version;
    systeminfo.user_agent.os = systeminfo.platform;
    systeminfo.user_agent.os_version = systeminfo.baseline_main_version + "," + systeminfo.baseline_version;
    systeminfo.user_agent.os_build = systeminfo.os_build;
    systeminfo.user_agent.device_id = systeminfo.mac;
    systeminfo.user_agent.client = "app_store";
    systeminfo.user_agent.channel = getChannel();
    systeminfo.user_agent.uid = "";
    systeminfo.user_agent.youthSys = getIsChildrenGuard() ? "true" : "false";
    systeminfo.user_agent.cpuid = "";
    systeminfo.user_agent.uuid = "";
    systeminfo.user_agent.sn = "";
}

QString Utils::systemArch()
{
    // 架构分为cpu架构和包架构，举例如下：
    // x86 cpu架构: x86_64; 对应的包架构：amd64
    // loongarch64 v20和v25 cpu架构都是：loongarch64； v20包架构是：loongarch64；v25包架构是: loong64
    // sw v20和v25 cpu架构都是：sw_64; v20包架构是：sw_64 v25包架构是：sw64
    // 服务端需要的架构信息是包架构，所以这里需要不能用 uname -m 获取架构信息，而是通过 dpkg --print-architecture 获取包架构信息。

    if (isExternalEnvSet && !externalEnv.arch.isEmpty()) {
        return externalEnv.arch;
    }

    QString arch = "";
    QProcess process;
    process.start("dpkg", {"--print-architecture"});
    process.waitForFinished(-1);
    if (process.exitCode() == 0 && process.exitStatus() == QProcess::NormalExit) {
        QString outData = process.readAllStandardOutput();
        if (!outData.isEmpty()) {
            arch = outData.trimmed();
        }
    }

    return arch;
}

bool Utils::isWayland()
{
    if (isExternalEnvSet && !externalEnv.iswayland.isEmpty()) {
        return externalEnv.iswayland == "1" ? true : false;
    }

    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        return true;
    }
    return false;
}

QString Utils::systemMode()
{
    if (isExternalEnvSet && !externalEnv.mode.isEmpty()) {
        return externalEnv.mode;
    }

    return DSysInfo::uosProductTypeName(QLocale(QLocale::English));
}

QString Utils::systemPlatform()
{
    QString platform = DSysInfo::uosEditionName(QLocale(QLocale::English));
    switch (DSysInfo::uosEditionType()) {
    case DSysInfo::UosProfessional:
        platform = "Professional";
        break;
    case DSysInfo::UosHome:
        platform = "Home";
        break;
    case DSysInfo::UosCommunity:
        platform = "Community";
        break;
    case DSysInfo::UosEducation:
        platform = "Education";
        break;
    case DSysInfo::UosMilitary:
        platform = "Military";
        break;
#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 11, 1) && DTK_VERSION < DTK_VERSION_CHECK(5, 7, 0, 0))
    case DSysInfo::UosPersonal:
        platform = "Personal";
        break;
#endif
    default:
        platform = "Unknown";
        break;
    }

    return platform;
}

QString Utils::systemRegion()
{
    if (isExternalEnvSet && !externalEnv.region.isEmpty()) {
        return externalEnv.region;
    }

    // 优先从配置文件中获取地区信息(v25版本中新增了该配置，v20无此配置)，若没读到配置，则fallback到从时区读取地区信息
    DConfig *config = DConfig::createGeneric("org.deepin.region-format", QString());
    QString countryStr = config->value("country").toString();
    if (!countryStr.isEmpty()) {
        QStringList list;
        list << "China" << "Hong Kong" << "Macau" << "Taiwan";
        return list.contains(countryStr) ? "CN" : "OTHER";
    }

    QList<QLocale::Country> countryList;
    countryList << QLocale::Country::China << QLocale::Country::HongKong << QLocale::Country::Macau << QLocale::Country::Taiwan;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QLocale::Territory territory = QTimeZone::systemTimeZone().territory();
    return countryList.contains(territory) ? "CN" : "OTHER";
#else
    QLocale::Country country = QLocale::Country::China;
    QLocale::Country currentCountry = QDateTime::currentDateTime().timeZone().country();
    if (currentCountry != QLocale::AnyCountry) {
        country = currentCountry;
    } else {
        QLocale::Country systemCountry = QTimeZone::systemTimeZone().country();
        if (systemCountry != QLocale::AnyCountry) {
            country = systemCountry;
        } else {
            QLocale::Country localeCountry = QLocale::system().country();
            if (localeCountry != QLocale::AnyCountry) {
                country = localeCountry;
            }
        }
    }
    return countryList.contains(country) ? "CN" : "OTHER";
#endif
}

QString Utils::systemLanguage()
{
    if (isExternalEnvSet && !externalEnv.language.isEmpty()) {
        return externalEnv.language;
    }

    return QLocale::system().name();
}

QString Utils::systemMajorVersion()
{
    if (isExternalEnvSet && !externalEnv.baseline_main_version.isEmpty()) {
        return externalEnv.baseline_main_version;
    }

    return DSysInfo::majorVersion();
}

QString Utils::systemMinorVersion()
{
    if (isExternalEnvSet && !externalEnv.baseline_version.isEmpty()) {
        return externalEnv.baseline_version;
    }

    return DSysInfo::minorVersion();
}

QString Utils::systemOsBuild()
{
    if (isExternalEnvSet && !externalEnv.os_build.isEmpty()) {
        return externalEnv.os_build;
    }

    DDesktopEntry parser("/etc/os-version");
    if (parser.status() == DDesktopEntry::AccessError || parser.status() == DDesktopEntry::FormatError) {
        qWarning() << "DDesktopEntry /etc/os-version Status Error:" << parser.status();
        return QString();
    }

    QString localeKey = QString("%1[%2]").arg("OsBuild", QLocale(QLocale::English).name());
    return parser.stringValue(localeKey, "Version", parser.stringValue("OsBuild", "Version"));
}

QString Utils::getAppstoreVersion()
{
    return QString(VERSION);
}

QString Utils::internetMacAddress()
{
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
    int nCnt = nets.count();
    QString strMacAddr;
    for (int i = 0; i < nCnt; i++) {
        if (nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning)
            && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)) {
            strMacAddr = nets[i].hardwareAddress();
            break;
        }
    }
    return strMacAddr;
}

QString Utils::getSupFeatures()
{
    if (isExternalEnvSet && !externalEnv.supFeatures.isEmpty()) {
        return externalEnv.supFeatures;
    }

    SupFeatures supFeatures = SupFeature::SupportSpecificDeb
                              | SupFeature::SupportEduCloudPlatform
                              | SupFeature::SupportDetailPageCards;
    if (isLinglongSupported()) {
        supFeatures |= SupFeature::SupportSpecificLinglong
                       | SupFeature::SupportLinglong
                       | SupFeature::SupportNewLinglong;
    }

    return QString::number(supFeatures, 2);
}

QString Utils::getMachineId()
{
    QFile file("/etc/machine-id");
    if (!file.open(QIODevice::ReadOnly)) {
        file.close();
        return QString();
    }

    QString machineId = QString::fromUtf8(file.readAll().trimmed());
    file.close();

    return machineId;
}

QString Utils::getChannel()
{
    QFile file("/var/cache/gather/key.txt");
    if (!file.open(QIODevice::ReadOnly)) {
        file.close();
        return QString();
    }

    QString channel = QString::fromUtf8(file.readAll().trimmed());
    file.close();

    return channel;
}

bool Utils::getIsChildrenGuard() 
{
    return Utils::systemPlatform() == "youngster";
}
