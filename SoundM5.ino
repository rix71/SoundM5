#include <M5StickC.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <driver/i2s.h>

#define PIN_CLK 0
#define PIN_DATA 34
#define PIN_LED 10
#define READ_LEN (2 * 256)
#define GAIN_FACTOR 15
#define SAMPLE_RATE 8000

int16_t BUFFER[READ_LEN] = {0};
int16_t *adcBuffer = nullptr;

bool recording = false;

// Set WiFi credentials
// Includes WIFI_SSID, WIFI_PASS and LOCAL_IP
// #include "wifi_cred.h"
// #define WIFI_SSID "XXX"
// #define WIFI_PASS "YYY"
// #define LOCAL_IP "Z.Z.Z.Z"

const char *ssid = "M5Stack_Ap";
const char *password = "66666666";

WiFiServer server(80);

const int packetSize = 1024;
uint16_t packetBuffer[packetSize];
int bytesPacked = 0;

void i2sInit() // Init I2S.
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX |
                             I2S_MODE_PDM), // Set the I2S operating mode.
                                            //
        .sample_rate = SAMPLE_RATE,         // Set the I2S sampling rate.
        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_16BIT, // Fixed 12-bit stereo MSB.

        .channel_format =
            I2S_CHANNEL_FMT_ALL_RIGHT, // Set the channel format.
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S, // Set the format of the communication.
#else
        .communication_format = I2S_COMM_FORMAT_I2S,
#endif
        .intr_alloc_flags =
            ESP_INTR_FLAG_LEVEL1, // Set the interrupt flag.
        .dma_buf_count = 8,       // DMA buffer count.
        .dma_buf_len = 128,       // DMA buffer length.
    };

    i2s_pin_config_t pin_config;

#if (ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif

    pin_config.bck_io_num = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num = PIN_DATA;

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void toggleRecordDisplay(const bool recording)
{
    if (recording)
    {
        M5.Lcd.fillScreen(WHITE);
        M5.Lcd.setCursor(0, 20);
        delay(1000);
        M5.Lcd.print("Recording...");
        digitalWrite(PIN_LED, LOW);
    }
    else
    {
        M5.Lcd.fillScreen(WHITE);
        M5.Lcd.setCursor(0, 20);
        M5.Lcd.println("Recording stopped");
        M5.Lcd.println("Press A to start recording again");
        digitalWrite(PIN_LED, HIGH);
        printWiFiInfo();
    }
}

void setup()
{
    M5.begin();
    Serial.begin(115200);
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(WHITE);
    delay(1000); // give me time to bring up the serial monitor

    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);

    M5.Lcd.setTextColor(BLACK, WHITE);

    // ------------------------
    // Connect to WiFi
    M5.Lcd.println("WIFI ACCESS POINT");
    Serial.println("WIFI ACCESS POINT");

    WiFi.softAP(ssid,
                password);

    M5.Lcd.println("Connected!");
    Serial.println("Connected!");

    IPAddress myIP = WiFi.softAPIP();

    M5.Lcd.printf("Please connect:%s \nThen access to:", ssid);
    Serial.printf("Please connect:%s \nThen access to:", ssid);
    M5.Lcd.println(myIP);
    Serial.println(myIP);
    M5.Lcd.println("Starting server...");
    Serial.println("Starting server...");

    server.begin();

    M5.Lcd.println("Success!");
    Serial.println("Success!");
    // ------------------------
    // Init I2S
    i2sInit();

    // M5.Lcd.print("Press A to start recording");
}

void printWiFiInfo()
{
    // Serial.println();

    // Serial.print("Sending to: ");
    // Serial.println(LOCAL_IP);
    // Serial.print("Port: ");
    // Serial.println(UDP_PORT);
    // M5.Lcd.print("Sending to: ");
    // M5.Lcd.println(LOCAL_IP);
    // M5.Lcd.print("Port: ");
    // M5.Lcd.println(UDP_PORT);
}

void loop()
{

    // if (M5.BtnA.wasPressed())
    // {
    //     recording = !recording;
    //     toggleRecordDisplay(recording);
    // }

    // M5.Lcd.setCursor(5, 5);
    // M5.Lcd.println("Looping...");

    WiFiClient client = server.available();
    if (client)
    {
        M5.Lcd.fillScreen(WHITE);
        M5.Lcd.setCursor(10, 10);
        Serial.println("Got client");
        M5.Lcd.println("Got client");
        while (client.connected())
        {
            size_t bytesread = 0;
            i2s_read(I2S_NUM_0, (char *)BUFFER, READ_LEN, &bytesread, 0);
            adcBuffer = (int16_t *)BUFFER;

            for (int i = 0; i < bytesread; i++)
            {
                packetBuffer[bytesPacked] = (int16_t)adcBuffer[i];
                bytesPacked++;
            }
            if (bytesPacked >= packetSize)
            {
                bytesPacked = 0;
                Serial.println("Sending packet");
                // udp.beginPacket(LOCAL_IP, UDP_PORT);
                client.write((uint8_t *)packetBuffer, packetSize);
                // udp.print((unsigned long)packetBuffer);
                // udp.printf("%d", packetBuffer);
                memset(packetBuffer, 0, packetSize);
                // udp.endPacket();
            }
        }
        client.stop();
        M5.Lcd.fillScreen(WHITE);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.println("Client disconnected");
        Serial.println("Client disconnected");
    }

    // if (recording)
    // {
    // // ------------------------------------------------------------------------
    // // Read the data from the I2S bus
    // size_t bytesread = 0;
    // i2s_read(I2S_NUM_0, (char *)BUFFER, READ_LEN, &bytesread, 0);
    // adcBuffer = (int16_t *)BUFFER;

    // for (int i = 0; i < bytesread; i++)
    // {
    //     packetBuffer[bytesPacked] = (int16_t)adcBuffer[i];
    //     bytesPacked++;
    // }
    // if (bytesPacked >= packetSize)
    // {
    //     bytesPacked = 0;
    //     Serial.println("Sending packet");
    //     udp.beginPacket(LOCAL_IP, UDP_PORT);
    //     udp.write((uint8_t *)packetBuffer, packetSize);
    //     // udp.print((unsigned long)packetBuffer);
    //     // udp.printf("%d", packetBuffer);
    //     memset(packetBuffer, 0, packetSize);
    //     udp.endPacket();
    // }
    // }

    M5.update();
}
