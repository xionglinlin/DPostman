#ifndef COMMON_H
#define COMMON_H

#include <QString>

namespace StoreService {

typedef struct _User_Agent_Data {
    QString device_id;
    QString uid;
    QString client = "app-store";
    QString client_version;
    QString os;
    QString os_version;
    QString os_build;
    QString channel;
    QString youthSys;
    QString cpuid;
    QString uuid;
    QString sn;
    
    QString toUserAgentStr() {
        QString data;
        data.append(QString("device_id/%1").arg(device_id));
        data.append(" ");
        data.append(QString("client/%1").arg(client));
        data.append(" ");
        data.append(QString("client_version/%1").arg(client_version));
        data.append(" ");
        data.append(QString("os/%1").arg(os));
        data.append(" ");
        data.append(QString("os_version/%1").arg(os_version));
        data.append(" ");
        data.append(QString("os_build/%1").arg(os_build));
        data.append(" ");
        data.append(QString("channel/%1").arg(channel));
        data.append(" ");
        data.append(QString("cpuid/%1").arg(cpuid));
        data.append(" ");
        data.append(QString("uuid/%1").arg(uuid));
        data.append(" ");
        data.append(QString("SN/%1").arg(sn));
        return data;
    }
} User_Agent_Data;

typedef struct _Request_Pulic_Header {
    QString arch = ""; //"sw_64","arm64","amd64","mips64"
    QString iswayland = ""; //取值0和1
    QString mode = ""; //"desktop","tablet"
    QString platform = ""; //取值为： professional，personal，huawei_kirin
    QString region = ""; //取值为：CN, OTHER 意义：CN 中国，OTHER 其它国家或地区
    QString language = ""; //语言，格式为[ISO-639]_[ISO-3166]，例如：zh_CN , en_us
    QString baseline_version = ""; //系统基线版本
    QString baseline_main_version = ""; //系统大版本
    QString os_build = ""; //系统 OsBuild
    QString client_version = ""; //客户端版本
    QString eabi = ""; //显卡能力（保留）--其他接口都需要加
    QString mac = ""; //网卡地址 现在为machineid
    QString userid = ""; //登录用户id 未登录时该值可不上传
    QString referer = ""; /*填写来源页面的id,如：在装机必备卡片上点击下载，则下载上报来源为装机必备卡片uri
    在搜索结果页面点击应用详情，则应用详情接口来源为搜索卡片的uri*/
    QString motherboard = ""; //主板名称
    QString cpu_clip = ""; // cpu名称
    QString is_system_active = ""; //系统是否激活，０：未激活，１：激活
    QString token = ""; //用户token
    QString macAddress = ""; //真正的网卡地址字段
    QString supFeatures = ""; //支持的特性，主要针对不兼容的特性，每个特性用一位二进制数表示，如：1111，表示支持4个不同的特性。本次定义第1位为指定包特性（从右边往左边开始）
    QString templateId = ""; //模板 ID，用于请求页面时后台广告系统判断显示广告
    User_Agent_Data user_agent;
} Request_Pulic_Header;

}

#endif // COMMON_H
