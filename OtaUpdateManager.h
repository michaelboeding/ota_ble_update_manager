#ifndef OTA_UPDATE_MANAGER_H
#define OTA_UPDATE_MANAGER_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <esp_task_wdt.h>
#include "NimBLEDevice.h"
#include "esp_ota_ops.h"


class I_OtaUpdateManager;

class OtaUpdateManager {

    public:
        //MARK: OTA INFO 
        esp_ota_handle_t otaHandler = 0;
        const esp_partition_t *update_partition;
        //start byte for ota firmware
        const int OTA_FIRMWARE_START_BYTE = 0xF1;
        //end byte for ota firmware
        const int OTA_FIRMWARE_END_BYTE = 0xFF;
        //create an enum for the diffrent states of the device in the ota process
        typedef enum OtaState {
            OTA_STATE_UPDATE_IN_PROGRESS = 0,
            OTA_STATE_INSTALLING = 1,
            OTA_STATE_INSTALLED = 2,
            OTA_STATE_IDLE = 3,
            OTA_STATE_ERROR = 4
        }OtaState;
        //create the default ota state 
        OtaState otaState = OtaState::OTA_STATE_IDLE;
        //constructor 
        OtaUpdateManager();
        //destructor
        ~OtaUpdateManager();
        //function used to set the delegate
        void setDelegate(I_OtaUpdateManager* delegate);
        //function used to set the starting byte for the ota firmware in hex
        void setOtaFirmwareStartByte(int startByte);
        //function used to set the ending byte for the ota firmware in hex
        void setOtaFirmwareEndByte(int endByte);
        //handle the firmware update 
        void handleOtaFirmwareData(NimBLECharacteristic* characteristic);

    private:
        //this is the ref to your interface 
        I_OtaUpdateManager* otaUpdateManagerInterface;

};



class I_OtaUpdateManager {
    public:
        virtual void otaUpdateDidStart(){
            //default 
            std::cout << "OTA update did start default fired" << std::endl;
        };
        virtual void otaUpdateDidFinish(){
                //default 
        };
        virtual void otaUpdateDidFail(){
                //default 
        };
};


#endif // OTA_UPDATE_MANAGER_H