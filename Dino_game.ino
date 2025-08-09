#include <LiquidCrystal.h>
#include <EEPROM.h>

#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define E 8
#define RS 9

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

int score = 0;
bool allow_jump = true;
bool isJumping = false;
unsigned long jumpStartTime = 0;
const unsigned long jumpDuration = 300;

const int jumpButton = 3;
const int buzzer = 2;

unsigned long lastScrollTime = 0;
const unsigned long scrollInterval = 100;
unsigned long gameStartTime = 0;

int obstacleCooldown = 0;

int melody[] = { 262, 294, 330, 262, 262, 294, 262, 330, 392, 440, 392, 330 };
int currentNote = 0;
unsigned long lastNoteTime = 0;
const int noteDurationMs = 150;
bool playMusic = false;

byte dino_l[8] = {B00000111, B00000101, B00000111, B00010110, B00011111, B00011110, B00001110, B00000100};
byte dino_r[8] = {B00000111, B00000101, B00000111, B00010110, B00011111, B00011110, B00001110, B00000010};
byte cactus_small[8] = {B00100, B00101, B10101, B10101, B10111, B11100, B00100, B00000};
byte cactus_big[8] = {B00000000, B00000100, B00000101, B00010101, B00010110, B00001100, B00000100, B00000100};

char world[] = {32, 32, 32, 32, 32, 32, 32, 83, 99, 111, 114, 101, 58, 32, 32, 32, 32, 0, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32};

String playerName = "";

void setup() {
  pinMode(jumpButton, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  lcd.begin(16, 2);
  lcd.createChar(0, dino_l);
  lcd.createChar(1, dino_r);
  lcd.createChar(2, cactus_small);
  lcd.createChar(3, cactus_big);

  Serial.begin(9600);

  // Ricevi nome dal PC
  lcd.setCursor(0, 0);
  lcd.print("Attendo nome...");
  while (true) {
  if (Serial.available()) {
    String nome = Serial.readStringUntil('\n');
    nome.trim();
    if (nome.length() > 0) {
      playerName = nome;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ciao, ");
      lcd.print(playerName);
      break;
    }
  }
  }

  delay(1000);
  start_game();
}

void start_game() {
  score = 0;
  for (int i = 0; i < 32; i++) world[i] = 32;
  obstacleCooldown = 0;
  gameStartTime = millis();
  lastScrollTime = millis();
  allow_jump = true;
  isJumping = false;
  playMusic = true;
  currentNote = 0;
  lastNoteTime = millis();

  world[17] = byte(0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ciao, "); lcd.print(playerName);
  lcd.setCursor(0, 1);
  lcd.write(byte(0)); lcd.write(byte(3));
  lcd.print("Press Start!");
  lcd.write(byte(3)); lcd.write(byte(0));
  while (!get_button());
  lcd.clear();
}

bool get_button() {
  return digitalRead(jumpButton) == LOW;
}

void play_game_music() {
  if (!playMusic) return;
  if (millis() - lastNoteTime >= noteDurationMs) {
  int note = melody[currentNote];
  tone(buzzer, note, noteDurationMs - 10);
  lastNoteTime = millis();
  currentNote++;
  if (currentNote >= sizeof(melody) / sizeof(int)) currentNote = 0;
  }
}

uint8_t scroll_world() {
  unsigned long now = millis();
  if (now - gameStartTime < 2000) world[31] = 32;
  else if (obstacleCooldown > 0) { obstacleCooldown--; world[31] = 32; }
  else {
  int r = random(0, 10);
  world[31] = (r < 2) ? ((random(0, 2) == 0) ? 2 : 3) : 32;
  if (r < 2) obstacleCooldown = 5;
  }
  for (int i = 16; i < 32; i++) {
  if (world[i] == 2 || world[i] == 3) {
    char prev = (i < 31) ? world[i + 1] : 32;
    if (world[i - 1] < 2) return 1;
    world[i - 1] = world[i]; world[i] = prev;
  }
  }
  world[15] = 32; if (world[16] < 2) world[16] = 32;
  return 0;
}

void update_display() {
  lcd.setCursor(13, 0); lcd.print("   ");
  lcd.setCursor(13, 0); lcd.print(score);
  lcd.setCursor(0, 0);
  for (int i = 0; i < 32; i++) {
  if (world[i] < 2) world[i] ^= 1;
  if (i == 16) lcd.setCursor(0, 1);
  if (i < 13 || i > 15) lcd.write(byte(world[i]));
  }
}

void game_over_screen() {
  playMusic = false;
  noTone(buzzer);
  tone(buzzer, 500, 500);

  lcd.setCursor(0, 1);
  lcd.write(byte(0)); lcd.write(byte(3));
  lcd.print(" GAME OVER! ");
  lcd.write(byte(3)); lcd.write(byte(0));
  delay(1500);

  // Invia punteggio al PC
  Serial.print("GIOCATORE: ");
  Serial.print(playerName);
  Serial.print(", RECORD: ");
  Serial.println(score);

  start_game();  // Riavvia subito la partita
}

void loop() {
  play_game_music();

  static unsigned long buttonHoldStart = 0;
  if (get_button()) {
  if (buttonHoldStart == 0) {
    buttonHoldStart = millis();
  } else if (millis() - buttonHoldStart >= 3000) {
    lcd.clear();
    lcd.print("RECORD RESET");
    delay(1500);
    buttonHoldStart = 0;
  }
  } else {
  buttonHoldStart = 0;
  }

  if (get_button() && !isJumping && allow_jump) {
  isJumping = true;
  jumpStartTime = millis();
  allow_jump = false;
  lcd.setCursor(1, 1); lcd.write(byte(32));
  lcd.setCursor(1, 0); lcd.write(byte(0));
  tone(buzzer, 1000, 100);
  world[1] = byte(0);
  world[17] = byte(32);
  }

  if (!get_button() && !isJumping) {
  allow_jump = true;
  }

  if (isJumping && millis() - jumpStartTime >= jumpDuration) {
  isJumping = false;
  world[1] = byte(32);
  world[17] = byte(0);
  lcd.setCursor(1, 0); lcd.write(byte(32));
  lcd.setCursor(1, 1); lcd.write(byte(0));
  }

  if (millis() - lastScrollTime >= scrollInterval) {
  lastScrollTime = millis();
  uint8_t collision = scroll_world();
  if (score == 999) {
    lcd.clear(); lcd.print(" YOU WIN! ");
    playMusic = false;
    noTone(buzzer);
    while (1);
  }
  if (collision) game_over_screen();
  score++;
  update_display();
  }
}
