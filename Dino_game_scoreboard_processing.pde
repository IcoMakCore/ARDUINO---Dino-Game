import processing.serial.*;
import java.util.HashMap;
import java.io.File;

Serial arduino;
String nomeGiocatore = "";
boolean inviato = false;

HashMap<String, Integer> classificaMap = new HashMap<String, Integer>();

// Bottoni
int btnResetX = 400, btnResetY = 30, btnW = 160, btnH = 30;
int btnInserisciX = 400, btnInserisciY = 70;

void setup() {
  size(600, 400);
  textSize(16);

  // Carica classifica da file
  String[] lines = loadStrings(dataPath("classifica.txt"));
  if (lines != null) {
    for (String line : lines) {
      if (line.contains(":")) {
        String[] parts = splitTokens(line, ":, ");
        if (parts.length >= 2) {
          String nome = parts[1];
          int punti = int(parts[3]);
          classificaMap.put(nome, punti);
        }
      }
    }
  }

  // Apri la porta seriale corretta
  arduino = new Serial(this, "/dev/ttyACM1", 9600); // Cambia se necessario
}

void draw() {
  background(255);
  fill(0);
  text("Nome giocatore:", 20, 40);
  text(nomeGiocatore, 20, 70);

  if (!inviato) {
    fill(150, 0, 0);
    text("Digita un nome e premi INVIO", 20, 100);
  }

  fill(0);
  text("Classifica (Record migliori):", 20, 140);
  int y = 170;
  for (String nome : classificaMap.keySet()) {
    text(nome + " - " + classificaMap.get(nome), 20, y);
    y += 20;
  }

  drawButton(btnResetX, btnResetY, btnW, btnH, "CANCELLA CLASSIFICA");
  drawButton(btnInserisciX, btnInserisciY, btnW, btnH, "INSERISCI NOME");

  // Ricezione da Arduino
  if (arduino.available() > 0) {
    String line = arduino.readStringUntil('\n');
    if (line != null && line.startsWith("GIOCATORE")) {
      line = line.trim();
      String[] parts = splitTokens(line, ":, ");
      if (parts.length >= 4) {
        String nome = parts[1];
        int punti = int(parts[3]);
        if (!classificaMap.containsKey(nome) || punti > classificaMap.get(nome)) {
          classificaMap.put(nome, punti);
          salvaClassifica();
        }
      }
    }
  }
}

void drawButton(int x, int y, int w, int h, String label) {
  fill(200);
  rect(x, y, w, h, 8);
  fill(0);
  textAlign(CENTER, CENTER);
  text(label, x + w / 2, y + h / 2);
  textAlign(LEFT, BASELINE);
}

void mousePressed() {
  if (overButton(btnResetX, btnResetY, btnW, btnH)) {
    classificaMap.clear();
    File f = new File(dataPath("classifica.txt"));
    if (f.exists()) f.delete();
  }

  if (overButton(btnInserisciX, btnInserisciY, btnW, btnH)) {
    nomeGiocatore = "";
    inviato = false;
    arduino.write("RESET\n");  // Avvisa Arduino di attendere un nuovo nome
  }
}

boolean overButton(int x, int y, int w, int h) {
  return mouseX >= x && mouseX <= x + w && mouseY >= y && mouseY <= y + h;
}

void keyPressed() {
  if (key == BACKSPACE && nomeGiocatore.length() > 0) {
    nomeGiocatore = nomeGiocatore.substring(0, nomeGiocatore.length() - 1);
  } else if ((key == ENTER || key == RETURN) && !inviato && nomeGiocatore.length() > 0) {
    arduino.write(nomeGiocatore + "\n");
    inviato = true;
  } else if (key != CODED && !inviato) {
    nomeGiocatore += key;
  }
}

void salvaClassifica() {
  String[] lines = new String[classificaMap.size()];
  int i = 0;
  for (String nome : classificaMap.keySet()) {
    lines[i++] = "GIOCATORE: " + nome + ", RECORD: " + classificaMap.get(nome);
  }
  saveStrings(dataPath("classifica.txt"), lines);
}
