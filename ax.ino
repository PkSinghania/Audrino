int ledpin=13;

/*
 * array contains the data to be transferred using AX.25
 */
 
byte senddata[27] = {0x86, 0xA2, 0x40, 0x40, 0x40, 0x40, 0x60, 0xAE, 0x64, 0x8C, 0xA6,0x40, 0x40, 0x68,0xA4,0x8A, 0x98, 0x82, 0xB2, 0x40, 0x61, 0x3F,0xF0, 0x54, 0x65, 0x73,0x74};

/* Two FCS bit set to FF.
 * FCS bit are required to implement CRC.
 */
 
byte fcslo = 0xFF;
byte fcshi= 0xFF;

int stuff = 0; // For bit stuffing

bool flag = false;  // This is true only when we are transmitting Flag.
bool fcsflag = false;  // This is true only when we are transmitting FCS bit.

/*
 * Function to send Packets.
 */
void sendpacket(){
  
  flag = true; // First send the flag.
  for (int i = 0; i<2; i++){ // for loop sending flag.
    sendbyte(0x7E);
  }
  
  flag = false; // sending flag ends.
  
  for (int i = 0; i<27; i++){ // sends the data.
    sendbyte(senddata[i]);
  }
  
  fcsflag =true; // sending the FCS bit.
  fcslo = fcslo ^ 0xFF;
  fcshi = fcshi ^ 0xFF;
  sendbyte(fcslo);
  sendbyte(fcshi);
  fcsflag = false;
  
  flag = true; // send flag in last to show end of packet.
  sendbyte(0x7E);
  
}

/*
 * Fuction to send a byte.
 */
void sendbyte (byte inbyte){
  
int k;
byte bt;

for (k=0;k<8;k++) {
  bt = inbyte & 0x01;
  
  // if byte is not Flag or FCS bit do FCS calculation.
  if ((fcsflag == false) & (flag == false)){ 
    crcbyte(bt);
  }

  if (bt == 0){
    flipout();
  }
  else { 
    stuff++; 
    if ((flag == false) & (stuff == 5)){
    delay (850); 
    flipout(); 
    }
  }
  
  inbyte = inbyte << 1;
  delay(850);
  }
}

/*
 * Function to on-off the led to check our transmission.
 */
void flipout(){
  stuff = 0;
  if (ledpin == HIGH){
    pinMode(ledpin, LOW);
    //Serial.println (0);
  }
  else{
    pinMode(ledpin, HIGH);
    //Serial.println (1);
  }
}

/* 
 *  Function to actually calculate FCS.
 */
void crcbyte(byte tbyte)
{
  
  byte a,b;
  a = fcshi & 0x01;
  b = fcslo & 0x01;
  fcshi >> 1;
  
  if ((fcslo ^ a) == 0x01){
    fcslo >> 1;
    fcslo = fcslo ^ 0x80;
  }
  else{
    fcslo >> 1;
  }
  if (((b & 0x01)^(tbyte)) == 0x01){
    fcshi = fcshi^0x84;
    fcslo = fcslo^0x08;
    
  }
}

/*
 * Setup function
 */
void setup() {
  // put your setup code here, to run once:
  pinMode (ledpin, OUTPUT);
  /*Serial.begin(100);
  while (!Serial) {
     // wait for serial port to connect. Needed for Native USB only
  }


  Serial.println("Goodnight moon!");*/

}

/*
 * Main fuction
 */
void loop() {
  // put your main code here, to run repeatedly:
  
  sendpacket();

}
