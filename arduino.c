int led0 = 8;
int led1 = 9;
int led2 = 10;
int led3 = 11;
int led4 = 12;

int countPatternA = 0;
int countPatternB = 0;
int countPatternC = 0;

int inByte = 0;

void setup() {
    Serial.begin(9600);
    pinMode(led0, OUTPUT);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
}

void downAllLeds() {
    digitalWrite(led0, LOW);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
}

void upAllLeds() {
    digitalWrite(led0, HIGH);
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
    digitalWrite(led3, HIGH);
    digitalWrite(led4, HIGH);
}

void patternA() {
    downAllLeds();
    delay(1000);
    upAllLeds();
   
}
// pisca ledes 2 vezes
void patternB() {

    downAllLeds();
    delay(1000);
    upAllLeds();
    delay(1000);
    downAllLeds();
    delay(1000);
    upAllLeds();
}
// acendo um por um
void patternC() {
    downAllLeds();
    digitalWrite(led0, HIGH);
    delay(200);
    digitalWrite(led0, LOW);
    delay(200);
    digitalWrite(led1, HIGH);
    delay(200);
    digitalWrite(led1, LOW);
    delay(200);
    digitalWrite(led2, HIGH);
    delay(200);
    digitalWrite(led2, LOW);
    delay(200);
    digitalWrite(led3, HIGH);
    delay(200);
    digitalWrite(led3, LOW);
    delay(200);
    digitalWrite(led4, HIGH);
    delay(200);
    digitalWrite(led4, LOW);
    
}

void loop() {
    if (Serial.available()) {

        inByte = Serial.read();

        switch (inByte) {
            case 'A':
                countPatternA += 1;
                patternA();
                break;
            case 'B':
                countPatternB += 1;
                patternB();
                break;
            case 'C':
                countPatternC += 1;
                patternC();
                break;
            case 'D':
                Serial.write(countPatternA);
                Serial.write(countPatternB);
                Serial.write(countPatternC);
                break;

        }
    }
}