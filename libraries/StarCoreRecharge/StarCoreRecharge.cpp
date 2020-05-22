#include "Arduino.h"
#include "StarCoreRecharge.h"

//Costructor
StarCoreRecharge::StarCoreRecharge()
{
}

/*
 * Public Methods
 */

//Init
void StarCoreRecharge::init(bool pDebug)
{
    debugMode = pDebug;
    logger.init(debugMode);

    pinMode(CHARGE_PIN, INPUT);
    pinMode(STANDBY_PIN, INPUT);
    pinMode(USB_PIN, INPUT);
}

//Checking if recharge is active
NeedBlinkRecharge StarCoreRecharge::needBlinkRecharge()
{
    NeedBlinkRecharge result = {false, 0};

    if (digitalRead(USB_PIN) == HIGH)
    {
        if (currentStatus == Status::disarmed)
        {
            currentStatus = Status::disarmedInRecharge;
            time = millis();
        }

        if (currentStatus == Status::disarmedInRecharge)
        {
            if (digitalRead(CHARGE_PIN) == HIGH)
            {
                if (millis() - time > BLINK_RECHARGE_STATUS_TIME)
                {
                    logger.writeParamString("Is Charge Pin High", "YES");
                    logger.writeParamLong("Recharging", millis() - time);
                    result.needRecharge = true;
                    result.colorRecharge = RECHARGE_COLOR;
                    time = millis();
                }
            }

            if (digitalRead(STANDBY_PIN) == HIGH)
            {
                if (millis() - time > BLINK_RECHARGED_STATUS_TIME)
                {
                    logger.writeParamString("Is StandBy Pin High", "YES");
                    logger.writeParamLong("Recharged", millis() - time);
                    result.needRecharge = true;
                    result.colorRecharge = RECHARGED_COLOR;
                    time = millis();
                }
            }
        }
    }
    else
    {
        if (currentStatus == Status::disarmedInRecharge)
        {
            currentStatus = Status::disarmed;
            time = 0;
        }
    }

    return result;
}

//Looping status
void StarCoreRecharge::loop(Status &rStatus, NeedBlinkRecharge &rNeedBlinkRecharge)
{
    currentStatus = rStatus;
    currentNeedBlinkRechargeStatus = rNeedBlinkRecharge;

    if ((currentStatus == Status::disarmed) ||
        (currentStatus == Status::disarmedInRecharge))
    {
        currentNeedBlinkRechargeStatus = needBlinkRecharge();
    }

    rStatus = currentStatus;
    rNeedBlinkRecharge = currentNeedBlinkRechargeStatus;
}