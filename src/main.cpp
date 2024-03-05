#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte buttonUpPin = 2;
const byte buttonDownPin = 3;
const byte buttonEnterPin = 4;

// Variáveis para rastrear o estado dos botões
int buttonUpState = HIGH;
int buttonDownState = HIGH;
int buttonEnterState = HIGH;
int lastButtonUpState = HIGH;
int lastButtonDownState = HIGH;
int lastButtonEnterState = HIGH;
unsigned long pressedEnterTime = 0;

// Variáveis para controlar o menu
int menuIndex = 0;

// Variável para armazenar o valor a ser aumentado ou diminuído
bool value = false;

// Opções do menu
const int NUM_OPTIONS = 2;
String menuOptions[NUM_OPTIONS] = {"Automatico", "Manual"};

// Submenus para controle manual e automático
const int NUM_SUBOPTIONS = 2;
String options[NUM_SUBOPTIONS] = {"Litros:", "Abastecer"};

int litros[4] = {0, 0, 0, 0};

void displayMenu();
void displaySubmenu(String menu);

void setup() {
  // Inicializa o LCD com 16 colunas e 2 linhas
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  Serial.println("Iniciando...");
  lcd.println("Iniciando...");
  
  // Define os pinos dos botões como entrada
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonEnterPin, INPUT_PULLUP);
  delay(1000);
  displayMenu();
}

void loop() {
  // Lê o estado atual dos botões
  buttonUpState = digitalRead(buttonUpPin);
  buttonDownState = digitalRead(buttonDownPin);
  buttonEnterState = digitalRead(buttonEnterPin);

  if (buttonUpState != lastButtonUpState) {
    if (buttonUpState == LOW && value == false) {
      Serial.println("Aumentando");
      menuIndex++;
      if (menuIndex > NUM_OPTIONS - 1) {
        menuIndex = 0;
      }
      Serial.println("Menu Index: " + String(menuIndex));
      displayMenu();
    }
    lastButtonUpState = buttonUpState;
    delay(200);
  }

  if (buttonDownState != lastButtonDownState) {
    if (buttonDownState == LOW && value == false) {
      Serial.println("Diminuindo");
      menuIndex--;
      if (menuIndex < 0) {
        menuIndex = NUM_OPTIONS - 1;
      }
      Serial.println("Menu Index: " + String(menuIndex));
      displayMenu();
    }
    lastButtonDownState = buttonDownState;
    delay(200);
  }

  if (buttonEnterState != lastButtonEnterState) {
    if (buttonEnterState == LOW) {
      if (value == false) {
        value = true;
        Serial.println("Submenu");
        displaySubmenu(options[menuIndex]);
      }
    }
    lastButtonEnterState = buttonEnterState;
    delay(200);
  }
}

void displayMenu() {
  Serial.println("Displaying menu: " + String(menuIndex));
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Menu Principal:");
  lcd.setCursor(0, 1);
  lcd.print(menuOptions[menuIndex]);
}

void displaySubmenu(String menu) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(menu);
  lcd.setCursor(0, 1);
  lcd.print("               "); // Limpa a área para exibir o valor
  if(menuIndex == 0){
    int cursor = 0;
    for (int i = 0; i <= 3; i++) {
      litros[i] = 0;
      lcd.setCursor(i, 1);
      lcd.print(litros[i]);
    }

    while (cursor < 4){
      lcd.setCursor(cursor, 1);
      lcd.print(" ");
      delay(100);
      lcd.setCursor(cursor, 1);
      lcd.print(litros[cursor]);
      delay(200);

      if (digitalRead(buttonUpPin) == LOW) {
        litros[cursor]++;
        if (litros[cursor] > 9) {
          litros[cursor] = 0;
        }
      }

      if (digitalRead(buttonDownPin) == LOW) {
        litros[cursor]--;
        if (litros[cursor] < 0) {
          litros[cursor] = 9;
        }
      }

      if (digitalRead(buttonEnterPin) == LOW) {
        lcd.setCursor(cursor, 1);
        lcd.print(litros[cursor]);
        cursor++;
      }
    } 
    
    int total = litros[0] * 1000 + litros[1] * 100 + litros[2] * 10 + litros[3];
    lcd.clear();
    delay(100);
    if (total > 0) {
      bool aux = true;
      lcd.setCursor(0, 0);
      lcd.print("Abastecer " + String(total) + "L?");
      lcd.setCursor(0, 1);
      lcd.print("              ");
      delay(100);
      while(!digitalRead(buttonEnterPin) == LOW) {
        delay(100);
        if (digitalRead(buttonDownPin) == LOW) {
          aux = false;
        }
        if (digitalRead(buttonUpPin) == LOW) {
          aux = true;
        }

        if (aux == true) {
          lcd.setCursor(0, 1);
          lcd.print(">Sim Nao");
        } else {
          lcd.setCursor(0, 1);
          lcd.print("Sim >Nao");
        }
      }
      if (aux == true)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Abastecendo...");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Abastecido!");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Volte sempre!");
      }
    }

  } else {
    while(true){
      delay(100);
      if (digitalRead(buttonEnterPin) == LOW) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Saindo");
        delay(2000);
        break;
      }

      while (digitalRead(buttonDownPin) == LOW) {
        lcd.setCursor(0,1);
        lcd.print("<-");
        delay(100);
      }

      while (digitalRead(buttonUpPin) == LOW) {
        lcd.setCursor(0,1);
        lcd.print("->");
        delay(100);
      }
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  }
  value = false;
  displayMenu();
}