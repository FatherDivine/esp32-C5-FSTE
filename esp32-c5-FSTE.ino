#include "FS.h"
#include "LittleFS.h"

#define MAX_CMD_LEN 64 

void listDir(const char * dirname) {
  File root = LittleFS.open(dirname);
  File file = root.openNextFile();
  while(file){
    Serial.printf("  FILE: %-15s SIZE: %d bytes\n", file.name(), file.size());
    file = root.openNextFile();
  }
}

void readFile(const char * path) {
  File file = LittleFS.open(path, "r");
  if(!file) return;
  while(file.available()) Serial.write(file.read());
  file.close();
  Serial.println("\n--- End of File ---");
}

// Simple "Nano-style" line-by-line editor
void nanoEditor(const char * path) {
  File file = LittleFS.open(path, "w");
  if(!file) return;
  
  Serial.printf("Editing %s. Type ':q' on a new line to save/exit.\n", path);
  
  while(true) {
    if (Serial.available()) {
      String line = Serial.readStringUntil('\n');
      if (line.startsWith(":q")) break;
      file.println(line);
      Serial.print("> "); // Visual feedback for next line
    }
  }
  file.close();
  Serial.println("\nFile saved.");
}

void writeFile(const char * path, const char * msg) {
  File file = LittleFS.open(path, "w");
  file.print(msg);
  file.close();
}

void setup() {
  Serial.begin(115200);
  // Initialize LittleFS
  if(!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  Serial.println("\n--- ESP32 Console Editor Ready ---");
  Serial.println("Commands: ls, cat <file>, touch <file>, edit <file>, rm <file>");

  // Example: create and list a .c file
  //writeFile("/main.c, "#include <stdio.h>\nint main() { printf(\"Hello ESP32\"); return 0; }");
  //listFiles("/");

}

void loop() {
  if (Serial.available()) {
    String cmdLine = Serial.readStringUntil('\n');
    cmdLine.trim();

    if (cmdLine == "ls") {
      listDir("/");
    } else if (cmdLine.startsWith("cat ")) {
      readFile(cmdLine.substring(4).c_str());
    } else if (cmdLine.startsWith("touch ")) {
      writeFile(cmdLine.substring(6).c_str(), "");
    } else if (cmdLine.startsWith("edit ")) {
      nanoEditor(cmdLine.substring(5).c_str());
    } else if (cmdLine.startsWith("rm ")) {
      LittleFS.remove(cmdLine.substring(3).c_str());
      Serial.println("File deleted.");
    }
  }
}
