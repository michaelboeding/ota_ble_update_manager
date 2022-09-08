#include "OtaUpdateManager.h"


// default static interface that is set on init
static I_OtaUpdateManager defaultOtaManagerInterface;

//constructor
OtaUpdateManager::OtaUpdateManager(){
    //set the interface
     this->otaUpdateManagerInterface = &defaultOtaManagerInterface;
}

//destructor
OtaUpdateManager::~OtaUpdateManager(){
    //deinit all the objects here 

}

/**
* @brief Set the delegate callbacks.
*
* This function can be called to register a delegate that will be invoked when these
* events are detected.
*
* @param [in] delegate The interface that the messages will be sent to
*/
void OtaUpdateManager::setDelegate(I_OtaUpdateManager* delegate) {
    if (this->otaUpdateManagerInterface != nullptr) {
        this->otaUpdateManagerInterface = delegate;
    }
}

/**
* @brief Set the starting byte 
*
* This function can be called to set a custom starting byte for the ota firmware, the default 
* is 0xF1
*
* @param [in] startByte The starting byte for the ota firmware in hex
*/
void OtaUpdateManager::setOtaFirmwareStartByte(int startByte){
    this->OTA_FIRMWARE_START_BYTE = startByte;
}

/**
* @brief Set the starting byte 
*
* This function can be called to set a custom ending byte for the ota firmware, the default 
* is 0xFF
*
* @param [in] startByte The ending byte for the ota firmware in hex
*/
void OtaUpdateManager::setOtaFirmwareEndByte(int endByte){
    this->OTA_FIRMWARE_END_BYTE = endByte;
}

//MARK: HANDLE FUNCTIONS 
void OtaUpdateManager::handleOtaFirmwareData(NimBLECharacteristic* characteristic){
    //handle ota firmware data if it is the correct data
    //get the data from the characterstic
    std::string data = characteristic->getValue();
    //convert the data into a uint8 array 
    uint8_t* dataArray = (uint8_t*)data.c_str();
    //check to make sure that the data array is not null
    if(dataArray != NULL){
        //check to see if its a starting or ending byte 
        if(data.length() == 1){
            //if it is a starting byte, check to see if it is the correct starting byte 
            if(dataArray[0] == this->OTA_FIRMWARE_START_BYTE){
                this->otaUpdateManagerInterface->otaUpdateDidStart();
                //setup the ota items and check for an error 
                this->update_partition = esp_ota_get_next_update_partition(NULL);
                //check to see if the partition is null, which means there was an error 
                if (this->update_partition == NULL) {
                    printf("OTA update partition error\n");
                    this->otaState = this->OtaState::OTA_STATE_ERROR;
                    this->otaUpdateManagerInterface->otaUpdateDidFail();
                }
                //if there was no error, try to begin the ota process 
                esp_err_t error = esp_ota_begin(this->update_partition, this->update_partition->size, &(this->otaHandler));
                //check to see if there was an error
                if (error != ESP_OK) {
                    printf("OTA update begin error\n");
                    //print the error 
                    this->otaState = this->OtaState::OTA_STATE_ERROR;
                    this->otaUpdateManagerInterface->otaUpdateDidFail();
                }
                //otherwise set the ota state to ota in progress
                this->otaState = this->OtaState::OTA_STATE_UPDATE_IN_PROGRESS;

            //confirm that the data is a ending byte, and an update is already in progress
            }else if (dataArray[0] == this->OTA_FIRMWARE_END_BYTE && this->otaState == this->OtaState::OTA_STATE_UPDATE_IN_PROGRESS){
                //update the mode to install 
                this->otaState = this->OtaState::OTA_STATE_INSTALLING;
                //now we need to update the esp32
                esp_ota_end((this->otaHandler));
                //confirm that the update is complete and ready to install
                esp_err_t error = esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL));
                //check to see if there was an error
                if (error != ESP_OK) {
                    printf("OTA update set boot partition error\n");
                    this->otaState = this->OtaState::OTA_STATE_ERROR;
                    this->otaUpdateManagerInterface->otaUpdateDidFail();
                }else{
                    //if there was no error, set the ota state to ota complete
                    this->otaState = this->OtaState::OTA_STATE_INSTALLED;
                    this->otaUpdateManagerInterface->otaUpdateDidFinish();
                    //delay for two seconds 
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                    //now we need to restart the esp32 and apply the changes 
                    esp_restart();
                }
            }
        //if the data length is longer that we are most likely getting a chunk of data
        }else if (data.length() > 1 && this->otaState == this->OtaState::OTA_STATE_UPDATE_IN_PROGRESS){
            //write the data to the ota handler 
            esp_err_t error = esp_ota_write((this->otaHandler), data.c_str(), data.length());
            //check to see if there was an error
            if (error != ESP_OK) {
                printf("OTA update write error\n");
                this->otaState = this->OtaState::OTA_STATE_ERROR;
                this->otaUpdateManagerInterface->otaUpdateDidFail();
            }
        }
    }

}