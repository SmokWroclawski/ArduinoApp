#include <BLE_API.h>
#include "bmp180.hpp"
#include "pms.hpp"
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

BLE ble;
Timeout timeout;

static uint8_t rx_buf[20];
static uint8_t rx_buf_num;
static uint8_t rx_state=0;

static const uint8_t service1_uuid[]      = {0x71, 0x3D, 0x00, 0x00, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_tx_uuid[]   = {0x71, 0x3D, 0x00, 0x03, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service1_rx_uuid[]   = {0x71, 0x3D, 0x00, 0x02, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t uart_base_uuid_rev[] = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0x00, 0x00, 0x3D, 0x71};

uint8_t tx_value[20] = {0,};
uint8_t rx_value[20] = {0,};

GattCharacteristic   characteristic1(service1_tx_uuid, tx_value, 1, 20, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE
                                                                      | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE);
GattCharacteristic   characteristic2(service1_rx_uuid, rx_value, 1, 20, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);
GattCharacteristic * uartChars[] = {&characteristic1, &characteristic2};
GattService          uartService(service1_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));

bmp_state bmp;
pms_state pms;
double temperature = 0.0, pressure = 0.0, humidity = 0.0, pm25 = 0.0, pm10 = 0.0;

DHT dht(DHTPIN, DHTTYPE);

Ticker ticker;

void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  ble.startAdvertising();
}

void gattServerWriteCallBack(const GattWriteCallbackParams *Handler) {
  uint8_t buf[20];
  uint16_t bytesRead;

  if (Handler->handle == characteristic1.getValueAttribute().getHandle()) {
    ble.readCharacteristicValue(characteristic1.getValueAttribute().getHandle(), buf, &bytesRead);
    if (memcmp(buf, "measure", sizeof("measure") - 1) != 0) {
      ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), (const uint8_t *)"error", sizeof("error") - 1);
      return;
    }
    
    int16_t f_temperature = temperature * 10;
    uint16_t f_pressure = pressure;
    uint16_t f_humidity = humidity;
    uint16_t f_pm25 = pm25 * 10;
    uint16_t f_pm10 = pm10 * 10;
    
    uint8_t frame[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    memcpy(&frame[0], &f_temperature, sizeof(f_temperature));
    memcpy(&frame[2], &f_pressure, sizeof(f_pressure));
    memcpy(&frame[4], &f_humidity, sizeof(f_humidity));
    memcpy(&frame[6], &f_pm25, sizeof(f_pm25));
    memcpy(&frame[8], &f_pm10, sizeof(f_pm10));

    Serial.println(frame[0]);
    Serial.println(frame[1]);
    Serial.println(frame[2]);
    Serial.println(frame[3]);
    Serial.println(frame[4]);
    Serial.println(frame[5]);
    Serial.println(frame[6]);
    Serial.println(frame[7]);
    Serial.println(frame[8]);
    Serial.println(frame[9]);
    Serial.println();

    if (f_temperature > -500 && f_temperature < 600 && f_pressure > 900 && f_pressure < 1100
        && f_humidity >= 0 && f_humidity < 100 && f_pm25 >= 0 && f_pm10 >= 0)
      ble.updateCharacteristicValue(characteristic2.getValueAttribute().getHandle(), frame, 10);
  }
}

// Function executed in interrupt once 30 sec
void ticker_handle()
{
  bmp_read_temp_and_pressure(&bmp);
  temperature = bmp_get_temperature(&bmp);
  pressure = bmp_get_pressure(&bmp);
  humidity = (double) dht.readHumidity();
  pm25 = pms.pm25;
  pm10 = pms.pm10;
}

void uart_handle(uint32_t id, SerialIrq event) {
  if(event == RxIrq) {
    while(Serial.available()) {
      uint8_t recv = Serial.read();
	    pms_recv_byte(&pms, recv);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Serial.attach(uart_handle);
  
  ble.init();
  ble.onDisconnection(disconnectionCallBack);
  ble.onDataWritten(gattServerWriteCallBack);

  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::SHORTENED_LOCAL_NAME,
                                   (const uint8_t *)"SMOK", sizeof("SMOK") - 1);
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,
                                   (const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  ble.addService(uartService);
  ble.setDeviceName((const uint8_t *)"Smok Wroclawski");
  ble.setTxPower(4);
  ble.setAdvertisingInterval(160);
  ble.setAdvertisingTimeout(0);
  ble.startAdvertising();

  bmp = bmp_init();
  bmp_read_compensation_data(&bmp);
  
  pms = pms_init();
  
  dht.begin();

  ticker.attach(ticker_handle, 5);
}

void loop() {
  ble.waitForEvent();
}
