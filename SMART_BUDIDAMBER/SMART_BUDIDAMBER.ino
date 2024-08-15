#include <Wire.h>
#include <RTClib.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// WiFi credentials
char ssid[] = "BACKUP";
char pass[] = "12345678";

// Isikan sesuai pada Firebase
#define FIREBASE_HOST "budidamber-kknugm2024-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "oQ5oIE2Udnarga8VRzhSgQlvItEHXL0B0Ytqil6L"

// Mendeklarasikan objek data dari FirebaseESP8266
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Inisialisasi DS1307RTC
RTC_DS1307 rtc;
DateTime targetTime1;
DateTime targetTime2;

bool countdownActive1 = false;
bool countdownActive2 = false;
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const long interval1 = 1000; // Interval 1 detik untuk printScheduledTimes
const long interval2 = 1000; // Interval 1 detik untuk updateCountdown

// Definisikan pin untuk push button
const int buttonPin1 = D7; // Push button untuk countdown 1 menit
const int buttonPin2 = D6; // Push button untuk countdown 2 menit
const int buttonPin3 = D3; // Push button untuk reset countdown

bool buttonState1 = LOW; // Status tombol 1
bool lastButtonState1 = LOW;
bool buttonState2 = LOW; // Status tombol 2
bool lastButtonState2 = LOW;
bool buttonState3 = LOW; // Status tombol 3
bool lastButtonState3 = LOW;

//Servo Settings
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void setup_wifi() {
 delay(10);
 // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  rtc.begin();
  // Uncomment baris di bawah ini untuk mengatur waktu awal
#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(DATE), F(TIME)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    rtc.adjust(DateTime(2024, 8, 1, 23, 5, 0));
  }
   servo1.attach(4);
   servo2.attach(15);
   servo3.attach(16); 
   servo4.attach(14);
  Serial.begin(115200);
  Serial.println("'Button 1' untuk countdown 1 menit");
  Serial.println("'Button 2' untuk countdown 2 menit");
  Serial.println("'Button 3' untuk reset countdown");

  // Set pin push button ke mode INPUT
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  setup_wifi();
  // Konfigurasi Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);
}

void loop() {
  DateTime now = rtc.now();
  printScheduledTimes(now);

  // Kontrol servo berdasarkan pembacaan sensor US1 dan kirim data Firebase
  if (Firebase.setInt(firebaseData, "/Data/Hour", now.hour()) &&
      Firebase.setInt(firebaseData, "/Data/Minute", now.minute()) &&
      Firebase.setInt(firebaseData, "/Data/Second", now.second())){
      
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();

      Serial.println("Data RTC Terkirim");
  } else {
      Serial.println("Data RTC tidak terkirim");
      Serial.print("Alasan: ");
      Serial.println(firebaseData.errorReason());
  }

    if (Firebase.getString(firebaseData, "Data/inlet") &&
        Firebase.getString(firebaseData, "Data/outlet") && 
        Firebase.getString(firebaseData, "Data/feeder1") && 
        Firebase.getString(firebaseData, "Data/feeder2")) { // Get the value as a string
    String inletStatusString = firebaseData.stringData();
    String outletStatusString = firebaseData.stringData();
    String feeder1StatusString = firebaseData.stringData();
    String feeder2StatusString = firebaseData.stringData();
    int inletStatus = inletStatusString.toInt(); // Convert the string to an integer
    int outletStatus = outletStatusString.toInt(); // Convert the string to an integer
    int feeder1Status = feeder1StatusString.toInt(); // Convert the string to an integer
    int feeder2Status = feeder2StatusString.toInt(); // Convert the string to an integer

    if (Firebase.getString(firebaseData, "Data/inlet")) {
        String inletStatusString = firebaseData.stringData();
        int inletStatus = inletStatusString.toInt();

        if (inletStatus == 1) {
            servo1.write(180);
            delay(1000); // Simulate time for servo to move to 180 degrees
            servo1.write(0);

            // Reset Firebase value to 0
            if (Firebase.setInt(firebaseData, "Data/inlet", 0)) {
                Serial.println("Successfully set inlet to 0");
            } else {
                Serial.print("Failed to set inlet to 0: ");
                Serial.println(firebaseData.errorReason());
            }
        }
    }

    if (Firebase.getString(firebaseData, "Data/outlet")) {
        String outletStatusString = firebaseData.stringData();
        int outletStatus = outletStatusString.toInt();

        if (outletStatus == 1) {
            servo2.write(180);
            delay(1000); // Simulate time for servo to move to 180 degrees
            servo2.write(0);

            // Reset Firebase value to 0
            if (Firebase.setInt(firebaseData, "Data/outlet", 0)) {
                Serial.println("Successfully set outlet to 0");
            } else {
                Serial.print("Failed to set outlet to 0: ");
                Serial.println(firebaseData.errorReason());
            }
        }
    }

    if (Firebase.getString(firebaseData, "Data/feeder1")) {
        String feeder1StatusString = firebaseData.stringData();
        int feeder1Status = feeder1StatusString.toInt();

        if (feeder1Status == 1) {
            servo3.write(180);
            Serial.println("FEEDER 1 MENYALA ABANGKUH");
            delay(1000); // Simulate time for servo to move to 180 degrees
            servo3.write(0);

            // Reset Firebase value to 0
            if (Firebase.setInt(firebaseData, "Data/feeder1", 0)) {
                Serial.println("Successfully set feeder1 to 0");
            } else {
                Serial.print("Failed to set feeder1 to 0: ");
                Serial.println(firebaseData.errorReason());
            }
        }
    }

    if (Firebase.getString(firebaseData, "Data/feeder2")) {
        String feeder2StatusString = firebaseData.stringData();
        int feeder2Status = feeder2StatusString.toInt();

        if (feeder2Status == 1) {
            servo4.write(180);
            Serial.println("GANTIAN FEEDER 2 COK");
            delay(1000); // Simulate time for servo to move to 180 degrees
            servo4.write(0);

            // Reset Firebase value to 0
            if (Firebase.setInt(firebaseData, "Data/feeder2", 0)) {
                Serial.println("Successfully set feeder2 to 0");
            } else {
                Serial.print("Failed to set feeder2 to 0: ");
                Serial.println(firebaseData.errorReason());
            }
        }
    }

    delay(500); // Allow time to ensure changes are saved
}
  // Tangani input dari push button
  handleButtonInput(buttonPin1, buttonState1, lastButtonState1, 1);
  handleButtonInput(buttonPin2, buttonState2, lastButtonState2, 2);
  handleButtonInput(buttonPin3, buttonState3, lastButtonState3, 3);

  // Update countdown jika aktif
  if (countdownActive1) {
    updateCountdown(1);
  }
  if (countdownActive2) {
    updateCountdown(2);
  }
 }

void handleButtonInput(int buttonPin, bool &buttonState, bool &lastButtonState, int countdownNumber) {
  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    delay(50); // Debounce delay
  }
  
  if (reading == HIGH && lastButtonState == LOW) {
    buttonState = !buttonState;
    if (buttonState == HIGH) {
      if (countdownNumber == 1) {
        startCountdown(1, 1); // Mulai countdown selama 1 menit (countdown 1)
      } else if (countdownNumber == 2) {
        startCountdown(2, 2); // Mulai countdown selama 2 menit (countdown 2)
      } else if (countdownNumber == 3) {
        resetCountdown(); // Reset countdown
      }
    }
  }
  
  lastButtonState = reading;
}

void startCountdown(int minutes, int countdownNumber) {
  // Reset countdown jika ada yang aktif
  if (countdownActive1 || countdownActive2) {
    resetCountdown();
  }

  if (countdownNumber == 1) {
    targetTime1 = rtc.now() + TimeSpan(0, 0, minutes, 0); // Tambahkan menit ke waktu saat ini
    countdownActive1 = true;
    Serial.println("Countdown 1 dimulai");
    Serial.print(minutes);
    Serial.println(" menit");
  } else if (countdownNumber == 2) {
    targetTime2 = rtc.now() + TimeSpan(0, 0, minutes, 0); // Tambahkan menit ke waktu saat ini
    countdownActive2 = true;
    Serial.println("Countdown 2 dimulai");
    Serial.print(minutes);
    Serial.println(" menit");
  }
}

void updateCountdown(int countdownNumber) {
  unsigned long currentMillis = millis();
  if (countdownNumber == 1 && currentMillis - previousMillis1 >= interval2) {
    previousMillis1 = currentMillis;
    DateTime now = rtc.now();
    if (now >= targetTime1) {
      Serial.println("Countdown 1 selesai!");
      Serial.println("Servo mulai");
      servo1.write(90);
      unsigned long servoStart = millis();
      while (millis() - servoStart < 4000) {
        if (millis() - servoStart < 1000) {
          Serial.println("Servo 1 berhenti");
           servo1.write(0);
        } else if (millis() - servoStart < 2000) {
          Serial.println("Servo 2 mulai");
           servo2.write(90);
        } else if (millis() - servoStart < 3000) {
          Serial.println("Servo 2 berhenti");
          servo2.write(0);
        }
        yield(); // Mengizinkan tugas latar belakang untuk berjalan
      }
      if (!countdownActive1) return; // Jika countdown telah di-reset, jangan mulai lagi
      startCountdown(1, 1); // Mulai kembali countdown 1
    } else {
      TimeSpan remaining = targetTime1 - now;
      Serial.print("C1 Tersisa: ");
      Serial.print(remaining.minutes());
      Serial.print("m ");
      Serial.print(remaining.seconds());
      Serial.println("s");
    }
  } else if (countdownNumber == 2 && currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    DateTime now = rtc.now();
    if (now >= targetTime2) {
      Serial.println("Countdown 2 selesai!");
      Serial.println("Servo mulai");
      unsigned long servoStart = millis();
      while (millis() - servoStart < 4000) {
        if (millis() - servoStart < 1000) {
          Serial.println("Servo 1 berhenti");
           servo1.write(0);
        } else if (millis() - servoStart < 2000) {
          Serial.println("Servo 2 mulai");
          servo2.write(90);
        } else if (millis() - servoStart < 3000) {
          Serial.println("Servo 2 berhenti");
          servo2.write(0);
        }
        yield(); // Mengizinkan tugas latar belakang untuk berjalan
      }
      if (!countdownActive2) return; // Jika countdown telah di-reset, jangan mulai lagi
      startCountdown(2, 2); // Mulai kembali countdown 2
    } else {
      TimeSpan remaining = targetTime2 - now;
      Serial.print("C2 Tersisa: ");
      Serial.print(remaining.minutes());
      Serial.print("m ");
      Serial.print(remaining.seconds());
      Serial.println("s");
    }
  }
}

void printScheduledTimes(DateTime now) {
  // Cek dan cetak waktu sesuai jadwal
  if (now.second() == 15) {
    Serial.println("Jam 7 pagi");
    unsigned long servoStart = millis();
    while (millis() - servoStart < 2000) {
      if (millis() - servoStart < 1000) {
         servo3.write(0);
         servo4.write(0);
      } else {
         servo3.write(180);
         servo4.write(180);
      }
      yield(); // Mengizinkan tugas latar belakang untuk berjalan
    }
  } else if (now.second() == 20) {
    Serial.println("Jam 1 siang");
    unsigned long servoStart = millis();
    while (millis() - servoStart < 2000) {
      if (millis() - servoStart < 1000) {
         servo3.write(0);
         servo4.write(0);
      } else {
         servo3.write(180);
         servo4.write(180);
      }
      yield(); // Mengizinkan tugas latar belakang untuk berjalan
    }
  } else if (now.second() == 30) {
    Serial.println("Jam 4 sore");
    unsigned long servoStart = millis();
    while (millis() - servoStart < 2000) {
      if (millis() - servoStart < 1000) {
         servo3.write(0);
         servo4.write(0);
      } else {
         servo3.write(180);
         servo4.write(180);
      }
      yield(); // Mengizinkan tugas latar belakang untuk berjalan
    }
  }
}

void resetCountdown() {
  // Hentikan countdown yang aktif
  countdownActive1 = false;
  countdownActive2 = false;
  Serial.println("Countdown dihentikan");
  delay(2000); // Tambahkan delay untuk melihat pesan reset
}
