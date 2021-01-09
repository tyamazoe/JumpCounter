#include <M5StickC.h>

#define UPPER_LIMIT 350.0  // jump up threshold
#define LOWER_LIMIT -400.0 // jump down threshold

#define SAMPLE_PERIOD 20   // sampling msec
#define SAMPLE_SIZE 150    // 20ms x150 = 3sec
#define X0 5               // chart horizontal start
#define MINZ -600          // chart vertical max
#define MAXZ  600          // chart vertical min

float gyroX = 0.0F;
float gyroY = 0.0F;
float gyroZ = 0.0F;
float gY[SAMPLE_SIZE];

int mode = 0;
uint16_t counter = 0;
int upcount = 0;
int gcounter = 0;

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Axp.ScreenBreath(8);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.IMU.Init();
}

void loop() {
  M5.update();
  // get gyro data
  M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ);
  Serial.printf("%6.2f\n", gyroY);

  // check jump up
  if (gyroY > UPPER_LIMIT) {
    upcount++;
  }
  // jump down and count
  if (gyroY < LOWER_LIMIT && upcount > 0) {
    upcount = 0;
    counter++;
  }

  // Display counter
  if (mode == 0) {
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(0, 20);
    M5.Lcd.printf(" %4d", counter);
  }
  // Draw glaph
  else if (mode == 1) {
    if (++gcounter > SAMPLE_SIZE) {
      gcounter = 0;
      M5.Lcd.fillScreen(BLACK);
    }
    else {
      gY[gcounter] = gyroY;
      int y0 = map((int)(gY[gcounter - 1]), MINZ, MAXZ, M5.Lcd.height(), 0);
      int y1 = map((int)(gY[gcounter]), MINZ, MAXZ, M5.Lcd.height(), 0);
      M5.Lcd.drawLine(gcounter - 1 + X0, y0, gcounter + X0, y1, GREEN);
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(0, 0);
      M5.Lcd.printf("%4d", counter);
    }
  }

  // BtnA change display mode
  if (M5.BtnA.wasPressed()) {
    mode = mode ? 0 : 1;
    M5.Lcd.fillScreen(BLACK);
  }
  // BtnB reset counter
  if (M5.BtnB.wasPressed()) {
    counter = 0;
    M5.Lcd.fillScreen(BLACK);
  }
  delay(SAMPLE_PERIOD);
}
