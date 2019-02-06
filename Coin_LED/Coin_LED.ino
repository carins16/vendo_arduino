
bool coin = 0;
bool nocoin = 0;
int coin_piso = 0;
int coin_acceptor = 13; //pin

void setup() {

pinMode(coin_acceptor, INPUT_PULLUP); // set Left button as a Digital Input
Serial.begin(9600);
}
void loop() {
  delay(50);
  if (digitalRead(coin_acceptor) == LOW){ 
  coin = true;
  nocoin = false;
  coin_piso++;
  
  }
  if (digitalRead(coin_acceptor) == LOW){ 
  delay(50);
  coin = false;
  nocoin = true;
  delay(1);
  }
Serial.println(coin_piso);
  if(coin == true && nocoin == false){
    switch (coin_piso) {
    case 1:
    Serial.println(coin_piso);
    break;
    case 2:
    Serial.println(coin_piso);
    break;
    default:
    Serial.println("BREAK");
    break;
  }
  }
  
  if(coin == false && nocoin == true){
  }
}
