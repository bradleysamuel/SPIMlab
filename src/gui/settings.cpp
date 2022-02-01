#include "settings.h"

#include "spim.h"

#include <memory>

#include <QDir>
#include <QSettings>

#define SET_VALUE(group, key, default_val) setValue(group, key, settings.value(key, default_val))

Settings::Settings()
{
    loadSettings();
}

Settings::~Settings() {}

QVariant Settings::value(const QString &group, const QString &key) const
{
    if (!map.contains(group) || !map[group]->contains(key)) {
        return QVariant();
    }
    return map[group]->value(key);
}

void Settings::setValue(const QString &group, const QString &key, const QVariant val)
{
    if (!map.contains(group)) {
        map.insert(group, new SettingsMap());
    }
    map.value(group)->insert(key, val);
}

void Settings::loadSettings()
{
    map.clear();

    QSettings settings;
    QString groupName;

    groupName = SETTINGSGROUP_OTHERSETTINGS;
    settings.beginGroup(groupName);

    SET_VALUE(groupName, SETTING_LUTPATH, "/opt/Fiji.app/luts/");
    SET_VALUE(groupName, SETTING_SCANVELOCITY, 2.0);
    QStringList camOutputPath;
    camOutputPath << "/mnt/dualspim"
                  << "/mnt/dualspim";
    SET_VALUE(groupName, SETTING_CAM_OUTPUT_PATH_LIST, camOutputPath);

    settings.endGroup();

    QStringList groups;
    for (int i = 0; i < SPIM_NPIDEVICES; ++i) {
        groups << SETTINGSGROUP_AXIS(i);
    }

    for (const QString &group : groups) {
        settings.beginGroup(group);

        QStringList sl;
        sl << SETTING_BAUD << SETTING_DEVICENUMBER << SETTING_SERIALNUMBER << SETTING_PORTNAME
           << SETTING_POS << SETTING_FROM << SETTING_TO << SETTING_STEP;

        for (const QString &s : sl) {
            SET_VALUE(group, s, QVariant());
        }

        SET_VALUE(group, SETTING_STEPSIZE, 0.1);
        SET_VALUE(group, SETTING_VELOCITY, 1.);
        SET_VALUE(group, SETTING_MOSAIC_ENABLED, false);

        settings.endGroup();
    }

    groupName = SETTINGSGROUP_GRAMP;
    settings.beginGroup(groupName);

    SET_VALUE(groupName, SETTING_PHYSCHANS, QStringList({"Dev1/ao2", "Dev1/ao3"}));
    SET_VALUE(groupName,
              SETTING_WFPARAMS,
              QList<QVariant>({0.0, 4.0, 0.0, 0.95, 0.0, 4.0, 0.0, 0.95}));
    settings.endGroup();

    groupName = SETTINGSGROUP_CAMTRIG;
    settings.beginGroup(groupName);

    SET_VALUE(groupName, SETTING_PULSE_TERMS, QStringList({"/Dev1/PFI0", "/Dev1/PFI1"}));
    SET_VALUE(groupName, SETTING_BLANKING_TERMS, QStringList({"/Dev1/PFI2", "/Dev1/PFI3"}));
    SET_VALUE(groupName, SETTING_TRIGGER_TERM, "/Dev1/PFI4");

    settings.endGroup();

    groupName = SETTINGSGROUP_ACQUISITION;
    settings.beginGroup(groupName);

    SET_VALUE(groupName, SETTING_EXPTIME, 0.15);
    SET_VALUE(groupName, SETTING_RUN_NAME, QString());
    SET_VALUE(groupName, SETTING_BINNING, 1);

    settings.endGroup();

    groups.clear();
    for (int i = 0; i < SPIM_NCOBOLT; ++i) {
        groups << SETTINGSGROUP_COBOLT(i);
    }
    for (const QString &group : groups) {
        settings.beginGroup(group);

        SET_VALUE(group, SETTING_PORTNAME, QString());

        settings.endGroup();
    }

    groups.clear();
    for (int i = 0; i < SPIM_NCAMS; ++i) {
        groups << SETTINGSGROUP_FILTERWHEEL(i);
    }

    QStringList filterNames;
    std::fill_n(std::back_inserter(filterNames), 6, "empty");

    for (const QString &group : groups) {
        settings.beginGroup(group);

        SET_VALUE(group, SETTING_SERIALNUMBER, QString());
        SET_VALUE(group, SETTING_FILTER_LIST, filterNames);

        settings.endGroup();
    }

    groups.clear();
    for (int i = 0; i < SPIM_NAOTF; ++i) {
        groups << SETTINGSGROUP_AOTF(i);
    }
    for (const QString &group : groups) {
        settings.beginGroup(group);

        SET_VALUE(group, SETTING_SERIALNUMBER, QString());

        settings.endGroup();
    }
}

void Settings::saveSettings() const
{
    QSettings settings;

    QMapIterator<QString, SettingsMap *> groupIt(map);

    while (groupIt.hasNext()) {
        groupIt.next();
        const SettingsMap *map = groupIt.value();
        QMapIterator<QString, QVariant> it(*map);

        settings.beginGroup(groupIt.key());
        while (it.hasNext()) {
            it.next();
            settings.setValue(it.key(), it.value());
        }
        settings.endGroup();
    }
}

Settings &settings()
{
    static auto instance = std::make_unique<Settings>();
    return *instance;
}
