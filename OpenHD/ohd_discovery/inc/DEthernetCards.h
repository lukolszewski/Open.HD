#ifndef ETHERNET_H
#define ETHERNET_H

#include <array>
#include <chrono>
#include <vector>

#include "DPlatform.h"

#include "json.hpp"

#include "openhd-ethernet.hpp"
#include "openhd-platform.hpp"
#include "openhd-discoverable.hpp"

/**
 * Discover all connected ethernet cards and write them to json.
 */
class DEthernetCards : public OHD::IDiscoverable{
public:
    DEthernetCards(PlatformType platform_type, BoardType board_type, CarrierType carrier_type);
    virtual ~DEthernetCards() = default;
    void discover();
    void process_card(const std::string& interface_name);
    nlohmann::json generate_manifest();
private:
    // All the discovered ethernet cards
    std::vector<EthernetCard> m_ethernet_cards;
    const PlatformType m_platform_type;
    const BoardType m_board_type;
    const CarrierType m_carrier_type;
};

#endif
