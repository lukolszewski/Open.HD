#ifndef OPENHD_WIFI_H
#define OPENHD_WIFI_H


#include <string>

#include "openhd-util.hpp"
#include "openhd-log.hpp"

typedef enum WiFiCardType {
    WiFiCardTypeRealtek8812au,
    WiFiCardTypeRealtek8814au,
    WiFiCardTypeRealtek88x2bu,
    WiFiCardTypeRealtek8188eu,
    WiFiCardTypeAtheros9khtc,
    WiFiCardTypeAtheros9k,
    WiFiCardTypeRalink,
    WiFiCardTypeIntel,
    WiFiCardTypeBroadcom,
    WiFiCardTypeUnknown
} WiFiCardType;


typedef enum WiFiHotspotType {
    WiFiHotspotTypeInternal2GBand,
    WiFiHotspotTypeInternal5GBand,
    WiFiHotspotTypeInternalDualBand,
    WiFiHotspotTypeExternal,
    WiFiHotspotTypeNone
} WiFiHotspotType;

typedef enum WifiUseFor{
    WifiUseForMonitorMode, //Use for wifibroadcast, aka set to monitor mode
    WifiUseForHotspot, //Use for hotspot, aka start a wifi hotspot with it
    WifiUseForUnknown // Not sure what to use this wifi card for ;)
}WifiUseFor;


struct WiFiCard {
    WiFiCardType type;
    std::string name;
    std::string vendor;
    std::string mac;
    bool supports_5ghz;
    bool supports_2ghz;
    bool supports_injection;
    bool supports_hotspot;
    bool supports_rts;
    // ? Dynamically changed settings ?
    std::string frequency;
    std::string txpower;
    WifiUseFor use_for=WifiUseForUnknown;
    std::string wifi_client_ap_name;
    std::string wifi_client_password;
    std::string hotspot_channel;
    std::string hotspot_password;
    std::string hotspot_band;
};

static nlohmann::json wificard_to_json(const WiFiCard& p) {
    auto j=nlohmann::json{ {"type", p.type},
                        {"name", p.name},
                        {"vendor", p.vendor},
                        {"mac", p.mac},
                        {"supports_5ghz", p.supports_5ghz},
                        {"supports_2ghz", p.supports_2ghz},
                        {"supports_injection", p.supports_injection},
                        {"supports_hotspot", p.supports_hotspot},
                        {"supports_rts", p.supports_rts},
    };
    return j;
}

static WiFiCard wificard_from_json(const nlohmann::json& j) {
    WiFiCard p;
    j.at("type").get_to(p.type);
    j.at("name").get_to(p.name);
    j.at("vendor").get_to(p.vendor);
    j.at("mac").get_to(p.mac);
    j.at("supports_5ghz").get_to(p.supports_5ghz);
    j.at("supports_2ghz").get_to(p.supports_2ghz);
    j.at("supports_injection").get_to(p.supports_injection);
    j.at("supports_hotspot").get_to(p.supports_hotspot);
    j.at("supports_rts").get_to(p.supports_rts);
    return p;
}


inline std::string wifi_card_type_to_string(const WiFiCardType& card_type) {
    switch (card_type) {
        case WiFiCardTypeAtheros9k: {
            return "ath9k_htc";
        }
        case WiFiCardTypeAtheros9khtc: {
            return "ath9k_htc";
        }
        case WiFiCardTypeRealtek8812au: {
            return "88xxau";
        }
        case WiFiCardTypeRealtek88x2bu: {
            return "88x2bu";
        }
        case WiFiCardTypeRealtek8188eu: {
            return "8188eu";
        }
        case WiFiCardTypeRalink: {
            return "rt2800usb";
        }
        case WiFiCardTypeIntel: {
            return "iwlwifi";
        }
        case WiFiCardTypeBroadcom: {
            return "brcmfmac";
        }
        default: {
            return "unknown";
        }
    }
}


inline WiFiCardType string_to_wifi_card_type(const std::string& driver_name) {
    if (to_uppercase(driver_name).find(to_uppercase("ath9k_htc")) != std::string::npos) {
        return WiFiCardTypeAtheros9khtc;
    } else if (to_uppercase(driver_name).find(to_uppercase("ath9k")) != std::string::npos) {
        return WiFiCardTypeAtheros9k;
    } else if (to_uppercase(driver_name).find(to_uppercase("rt2800usb")) != std::string::npos) {
        return WiFiCardTypeRalink;
    } else if (to_uppercase(driver_name).find(to_uppercase("iwlwifi")) != std::string::npos) {
        return WiFiCardTypeIntel;
    } else if (to_uppercase(driver_name).find(to_uppercase("brcmfmac")) != std::string::npos) {
        return WiFiCardTypeBroadcom;
    } else if (to_uppercase(driver_name).find(to_uppercase("88xxau")) != std::string::npos) {
        return WiFiCardTypeRealtek8812au;
    } else if (to_uppercase(driver_name).find(to_uppercase("8812au")) != std::string::npos) {
        return WiFiCardTypeRealtek8812au;
    } else if (to_uppercase(driver_name).find(to_uppercase("88x2bu")) != std::string::npos) {
        return WiFiCardTypeRealtek88x2bu;
    } else if (to_uppercase(driver_name).find(to_uppercase("8188eu")) != std::string::npos) {
        return WiFiCardTypeRealtek8188eu;
    }

    return WiFiCardTypeUnknown;
}


inline std::string wifi_hotspot_type_to_string(const WiFiHotspotType& wifi_hotspot_type) {
    switch (wifi_hotspot_type) {
        case WiFiHotspotTypeInternal2GBand: {
            return "internal2g";
        }
        case WiFiHotspotTypeInternal5GBand: {
            return "internal5g";
        }
        case WiFiHotspotTypeInternalDualBand: {
            return "internaldualband";
        }
        case WiFiHotspotTypeExternal: {
            return "external";
        }
        default: {
            return "none";
        }
    }
}


inline WiFiHotspotType string_to_wifi_hotspot_type(const std::string& hotspot_type) {
    if (to_uppercase(hotspot_type).find(to_uppercase("internal2g")) != std::string::npos) {
        return WiFiHotspotTypeInternal2GBand;
    } else if (to_uppercase(hotspot_type).find(to_uppercase("internal5g")) != std::string::npos) {
        return WiFiHotspotTypeInternal5GBand;
    } else if (to_uppercase(hotspot_type).find(to_uppercase("internaldualband")) != std::string::npos) {
        return WiFiHotspotTypeInternalDualBand;
    } else if (to_uppercase(hotspot_type).find(to_uppercase("external")) != std::string::npos) {
        return WiFiHotspotTypeExternal;
    }

    return WiFiHotspotTypeNone;
}


static nlohmann::json wificards_to_manifest(const std::vector<WiFiCard>& cards){
    nlohmann::json j;
    auto wifi_cards_json = nlohmann::json::array();
    for (auto &_card : cards) {
        auto cardJson = wificard_to_json(_card);
        std::stringstream message;
        message << "Detected wifi (" << wifi_card_type_to_string(_card.type) << ") interface: " << _card.name << std::endl;
        ohd_log(STATUS_LEVEL_INFO, message.str());
        wifi_cards_json.push_back(cardJson);
    }
    j["cards"] = wifi_cards_json;
    return j;
}

static constexpr auto WIFI_MANIFEST_FILENAME="/tmp/wifi_manifest";

static std::vector<WiFiCard> wificards_from_manifest(){
    std::vector<WiFiCard> ret;
    try {
        std::ifstream f(WIFI_MANIFEST_FILENAME);
        nlohmann::json j;
        f >> j;
        for (const auto& _card : j["cards"]) {
            WiFiCard card= wificard_from_json(_card);
            ret.push_back(card);
        }
    } catch (std::exception &ex) {
        // don't do anything, but send an error message to the user through the status service
        ohd_log(STATUS_LEVEL_EMERGENCY, "WiFi manifest processing failed");
        std::cerr << "WiFi::process_manifest: " << ex.what() << std::endl;
        return ret;
    }
    return ret;
}

#endif
