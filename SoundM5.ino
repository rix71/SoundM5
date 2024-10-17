// #include <M5StickC.h>
#include <M5StickCPlus2.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <driver/i2s.h>

#define PIN_CLK 0
#define PIN_DATA 34
#define PIN_LED 10

static constexpr size_t read_len = 512;
static constexpr size_t sample_rate = 44100;

static int16_t buffer[read_len] = {0};

bool recording = false;

const char* ssid = "M5Stack_Ap";
const char* password = "66666666";

WiFiServer server(80);

static constexpr size_t packetSize = 1024;
static int16_t packetBuffer[packetSize] = {0};
int bytesPacked = 0;

void setup() {
  auto cfg = m5::M5Unified::config();
  StickCP2.begin(cfg);
  StickCP2.Lcd.setRotation(3);
  delay(5000);
  // M5.begin();
  Serial.begin(115200);
  StickCP2.Lcd.setRotation(3);
  StickCP2.Lcd.fillScreen(WHITE);
  delay(1000);  // give me time to bring up the serial monitor

  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, HIGH);

  StickCP2.Lcd.setTextColor(BLACK, WHITE);

  // ------------------------
  // Connect to WiFi
  StickCP2.Lcd.println("WIFI ACCESS POINT");
  Serial.println("WIFI ACCESS POINT");

  WiFi.softAP(ssid, password);

  StickCP2.Lcd.println("Connected!");
  Serial.println("Connected!");

  IPAddress myIP = WiFi.softAPIP();

  StickCP2.Lcd.printf("Please connect:%s \nThen access to:", ssid);
  Serial.printf("Please connect:%s \nThen access to:", ssid);
  StickCP2.Lcd.println(myIP);
  Serial.println(myIP);
  StickCP2.Lcd.println("Starting server...");
  Serial.println("Starting server...");

  server.begin();

  StickCP2.Lcd.println("Success!");
  Serial.println("Success!");

  // ------------------------
  // Init I2S
  // i2sInit();
  StickCP2.Speaker.end();  // Just in case
  if (!StickCP2.Mic.begin()) {
    StickCP2.Lcd.println("Failed to start microphone");
    Serial.println("Failed to start microphone");
    while (true) {
      // Do nothing if the mic failed
    }
  }
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(10, 10);
    Serial.println("Got client");
    M5.Lcd.println("Got client");

    while (client.connected() && StickCP2.Mic.isEnabled()) {
      StickCP2.Mic.record(buffer, read_len, sample_rate);

      for (int i = 0; i < read_len; i++) {
        packetBuffer[bytesPacked] = buffer[i];
        bytesPacked++;
      }
      if (bytesPacked >= packetSize) {
        bytesPacked = 0;
        Serial.println("Sending packet");
        client.write((uint8_t*)packetBuffer, packetSize * sizeof(int16_t));
        memset(packetBuffer, 0, packetSize);
      }
    }
    client.stop();
    StickCP2.Mic.end();
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.println("Client disconnected");
    Serial.println("Client disconnected");
  }
  StickCP2.update();
}

#if 0
void i2sInit()  // Init I2S.
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX |
                           I2S_MODE_PDM),  // Set the I2S operating mode.
                                           //
      .sample_rate = SAMPLE_RATE,          // Set the I2S sampling rate.
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // Fixed 12-bit stereo MSB.

      .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,  // Set the channel format.
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
      .communication_format =
          I2S_COMM_FORMAT_STAND_I2S,  // Set the format of the communication.
#else
      .communication_format = I2S_COMM_FORMAT_I2S,
#endif
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,  // Set the interrupt flag.
      .dma_buf_count = 8,                        // DMA buffer count.
      .dma_buf_len = 128,                        // DMA buffer length.
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
  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT,
              I2S_CHANNEL_MONO);
}
#endif
