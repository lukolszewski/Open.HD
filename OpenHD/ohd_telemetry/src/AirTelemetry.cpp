//
// Created by consti10 on 13.04.22.
//

#include "AirTelemetry.h"
#include "mav_helper.h"
#include "mavsdk_temporary/XMavlinkParamProvider.h"
#include "mavsdk_temporary/XMavsdkWrapperSerialConnection.h"
#include <chrono>

AirTelemetry::AirTelemetry(OHDPlatform platform,std::string fcSerialPort): _platform(platform),MavlinkSystem(OHD_SYS_ID_AIR) {
  //serialEndpoint = std::make_unique<SerialEndpoint>("FCSerial",SerialEndpoint::HWOptions{fcSerialPort, 115200});
  /*serialEndpoint = std::make_unique<mavsdk::XMavsdkWrapperSerialConnection>(fcSerialPort,115200);
  serialEndpoint->registerCallback([this](MavlinkMessage &msg) {
    this->onMessageFC(msg);
  });*/

  // any message coming in via wifibroadcast is a message from the ground pi
  wifibroadcastEndpoint = UDPEndpoint::createEndpointForOHDWifibroadcast(true);
  wifibroadcastEndpoint->registerCallback([this](MavlinkMessage &msg) {
	onMessageGroundPi(msg);
  });
  _ohd_main_component=std::make_shared<OHDMainComponent>(_platform,_sys_id,true);
  components.push_back(_ohd_main_component);
  std::cout << "Created AirTelemetry\n";
}

void AirTelemetry::sendMessageFC(const MavlinkMessage &message) {
  if(serialEndpoint){
    serialEndpoint->sendMessage(message);
  }else{
    //std::cout<<"Cannot send message to FC\n";
  }
  if(message.m.msgid==MAVLINK_MSG_ID_PING){
    std::cout<<"Sent ping to FC\n";
    MavlinkHelpers::debugMavlinkPingMessage(message.m);
  }
}

void AirTelemetry::sendMessageGroundPi(const MavlinkMessage &message) {
  //debugMavlinkMessage(message.m,"AirTelemetry::sendMessageGroundPi");
  // broadcast the mavlink message via wifibroadcast
  wifibroadcastEndpoint->sendMessage(message);
}

void AirTelemetry::onMessageFC(MavlinkMessage &message) {
  //debugMavlinkMessage(message.m,"AirTelemetry::onMessageFC");
  sendMessageGroundPi(message);
  // handling a message from the FC is really easy - we just forward it to the ground pi.
  if(message.m.msgid==MAVLINK_MSG_ID_PING){
	std::cout<<"Got ping from FC\n";
  }
}

void AirTelemetry::onMessageGroundPi(MavlinkMessage &message) {
  const mavlink_message_t &m = message.m;
  // we do not need to forward heartbeat messages coming from the ground telemetry service,
  // They solely have a debugging purpose such that one knows the other service is alive.
  if (m.msgid == MAVLINK_MSG_ID_HEARTBEAT && m.sysid == OHD_SYS_ID_GROUND) {
	// heartbeat coming from the ground service
	return;
  }
  // for now, do it as simple as possible
  sendMessageFC(message);
  // any data created by an OpenHD component on the air pi only needs to be sent to the ground pi, the FC cannot do anything with it anyways.
  for(auto& component: components){
    const auto responses=component->process_mavlink_message(message);
    for(const auto& response:responses){
      sendMessageGroundPi(response);
    }
  }
}

[[noreturn]] void AirTelemetry::loopInfinite(const bool enableExtendedLogging) {
  const auto log_intervall=std::chrono::seconds(5);
  const auto loop_intervall=std::chrono::milliseconds(500);
  auto last_log=std::chrono::steady_clock::now();
  while (true) {
        if(std::chrono::steady_clock::now()-last_log>=log_intervall){
          last_log=std::chrono::steady_clock::now();
          //std::cout << "AirTelemetry::loopInfinite()\n";
          // for debugging, check if any of the endpoints is not alive
          if (enableExtendedLogging && wifibroadcastEndpoint) {
            std::cout<<wifibroadcastEndpoint->createInfo();
          }
          if (enableExtendedLogging && serialEndpoint) {
            std::cout<<serialEndpoint->createInfo();
          }
        }
	// send messages to the ground pi in regular intervals, includes heartbeat.
	// everything else is handled by the callbacks and their threads
        for(auto& component:components){
          const auto messages=component->generate_mavlink_messages();
          for(const auto& msg:messages){
            sendMessageGroundPi(msg);
          }
        }
	// send out in X second intervals
	std::this_thread::sleep_for(loop_intervall);
  }
}

std::string AirTelemetry::createDebug() const {
  std::stringstream ss;
  //ss<<"AT:\n";
  if ( wifibroadcastEndpoint) {
	ss<<wifibroadcastEndpoint->createInfo();
  }
  if (serialEndpoint) {
	ss<<serialEndpoint->createInfo();
  }
  return ss.str();
}

void AirTelemetry::add_settings_component(
    const int comp_id, std::shared_ptr<openhd::XSettingsComponent> glue) {
  auto param_server=std::make_shared<XMavlinkParamProvider>(_sys_id,comp_id,std::move(glue));
  components.push_back(param_server);
  std::cout<<"Added parameter component\n";
}
