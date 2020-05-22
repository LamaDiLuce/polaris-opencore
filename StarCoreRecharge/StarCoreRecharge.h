/*
- Quando la spada è in carica ed il kill-switch è su ON, si vedrà un lampeggio di 500ms della
lama ogni 20 secondi, quando la spada è completamente carica si vedranno 5 lampeggi di
500ms della lama ogni 40 secondi
    - I colori delle due tipologie di lampeggio saranno predefiniti e non modificabili
dall’utente (saranno definiti in seconda battuta)
- La ricarica avviene in qualsiasi caso (a prescindere dallo stato del kill-switch), basta collegare
il cavo USB alla presa di corrente
- Se il kill-switch è su OFF, non si avranno le notifiche visive di ricarica “in corso” e “ultimata”
*/
#include "Arduino.h"
#include "Logging.h"
#include "StarCoreCommon.h"

#define CHARGE_PIN 13
#define STANDBY_PIN 15
#define USB_PIN 23

#define BLINK_RECHARGE_STATUS_TIME 8000
#define BLINK_RECHARGED_STATUS_TIME 12000

#ifndef StarCoreRecharge_h
#define StarCoreRecharge_h

class StarCoreRecharge
{
public:
  //Costructor
  StarCoreRecharge();
  //Init
  void init(bool debug);
  //Process loop
  void loop(Status &rStatus, NeedBlinkRecharge &rNeedBlinkRecharge);

private:
  bool debugMode = false;
  Logging logger;
  unsigned long time;
  Status currentStatus;
  NeedBlinkRecharge currentNeedBlinkRechargeStatus = {false, 0};
  NeedBlinkRecharge needBlinkRecharge();
};

#endif