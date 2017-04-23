/*
 * This algorithm was written to transmit and receive 32-bits.
 * the structure of the algorithm is written to accomodate ADF4159.
 * 
 * This code allows the user to readback the frequency of operation of 
 * the ADF4159
 */


#include <Arduino.h>
#include <SPI.h>

int PIN_TXDATA = 1; //Set appropriately
int PIN_LE = 2; //Set appropriately
int PIN_SCK = 3; //Set appropriately
int PIN_MOSI = 4; //Set appropriately
int PIN_MISO = 5; //Set appropriately
int PIN_CE = 6; //Set appropriately

SPISettings spiSetting(115800, MSBFIRST, SPI_MODE0);
unsigned long R0_Init = 0x00000000;
unsigned long R1_Init = 0x00000001;
unsigned long R2_Init = 0x00000002;
unsigned long R3_Init = 0x00000003;
unsigned long R4_CLKDIVSEL_0_Init = 0x00000004;
unsigned long R4_CLKDIVSEL_1_Init = 0x00000044;
unsigned long R5_DEVSEL_0_Init = 0x00000005;
unsigned long R5_DEVSEL_1_Init = 0x00800005;
unsigned long R6_STEPSEL_0_Init = 0x00000006;
unsigned long R6_STEPSEL_1_Init = 0x00800006;
unsigned long R7_Init = 0x00000007;
unsigned long R0_Config = 0xB03C0000;
unsigned long R1_Config = 0x00000001;
unsigned long R2_Config = 0x0740801A;
unsigned long R3_Config = 0x00C30043;
unsigned long R4_Config = 0x00780D04;
unsigned long R5_Config = 0x00402005;
unsigned long R6_Config = 0x00000206;
unsigned long R7_Config = 0x00000007;

int MAX_CMD_LENGTH = 20;
char cmd[20];
int cmdIndex;
char incomingByte;

void setup()
{
  // put your setup code here, to run once:
  pinMode(PIN_CE, OUTPUT);
  pinMode(PIN_LE, OUTPUT);
  pinMode(PIN_TXDATA, OUTPUT);
  digitalWrite(PIN_CE, HIGH);
  digitalWrite(PIN_TXDATA, LOW);
  digitalWrite(PIN_LE, HIGH);
  Serial.begin(9600);
  cmdIndex = 0;
  delay(3000);
  Serial.println("Enter integer related to mode of operation: [1][2][3]");
  Serial.println("[1] Initialisation Mode. Type into serial command 'one'.");
  Serial.println("[2] Ramp Configuration Mode. Type into serial command 'two'.");
  Serial.println("[3] Data Readback Mode. Type into serial command 'three'. ");
  delay(2000);
}
unsigned long toLittleEndian(unsigned long num)
{
  return ((num >> 24) & 0xff) | // move byte 3 to byte 0
         ((num << 8) & 0xff0000) | // move byte 1 to byte 2
         ((num >> 8) & 0xff00) | // move byte 2 to byte 1
         ((num << 24) & 0xff000000); //move byte 0 to byte 3
}
void writeRegister(unsigned long val)
{
  digitalWrite(SS, LOW);
  unsigned long leVal = toLittleEndian(val);
  SPI.transfer(&leVal, 4);
  digitalWrite(SS, HIGH);
  delay(1);
}

unsigned long readRegister()
{
   // The following seven steps summarises the readRegister function. 
  // 1. Reg 0 write
  // 2. LE high
  // 3. Pulse on TXDATA
  // 4. Frequency readback
  // 5. Pulse on TXDATA
  // 6. Reg 4 write
  // 7.Pulse on TXDATA

  int variable32[32];
  int variable40[40];

  unsigned long wR0 = 0xF83C0000;
  unsigned long wR4 = 0x00580D04;
  unsigned long wR5 = 0x0C402005;
  SPI.beginTransaction(spiSetting);
  writeRegister(wR5);
  writeRegister(wR0);
  SPI.endTransaction();
  SPI.beginTransaction(spiSetting);
  digitalWrite(PIN_LE, HIGH);
  digitalWrite(PIN_TXDATA, HIGH);
  digitalWrite(PIN_TXDATA, LOW);
  uint8_t buff1[4];
  unsigned long DOUT1 = 0x00;
  for (int i = 3; i >= 0; i--)
  {
    byte b1 = SPI.transfer(0x00);
    buff1[i] = b1;

  }
  DOUT1 = *buff1;
  SPI.endTransaction();

  digitalWrite(PIN_LE, LOW);
  SPI.beginTransaction(spiSetting);
  digitalWrite(PIN_LE, HIGH);
  digitalWrite(PIN_TXDATA, HIGH);
  digitalWrite(PIN_TXDATA, LOW);

  writeRegister(wR4);

  uint8_t buff2[4];
  unsigned long DOUT2 = 0x00;
  for (int i = 4; i >= 0; i--)
  {
    byte b2 = SPI.transfer(0x00);
    buff2[i] = b2;

  }
  DOUT2 = *buff2;
  SPI.endTransaction();

  digitalWrite(PIN_TXDATA, HIGH);
  digitalWrite(PIN_TXDATA, LOW);

  CrackLong32( DOUT1,  variable32 );
  CrackLong40( DOUT2,  variable40 );
  delay(1000);
  Serial.println("The 32bit binary sequence is");
  for (int a = 0; a < 32; a++)
  {
    Serial.print(variable32[a]);
  }
  delay(1000);
  Serial.println("The 40bit frequency readback binary sequence is");
  for (int a = 0; a < 40; a++)
  {
    Serial.print(variable40[a]);
  }
  Serial.println();

}

void CrackLong32( unsigned long b, int result[32] )
{
  for (int i = 0; i < 40; ++i )
  {
    result[i] = b & 1;
    b = b >> 1;
  }
}
void CrackLong40( unsigned long b, int result[40] )
{
  for (int i = 0; i < 40; ++i )
  {
    result[i] = b & 1;
    b = b >> 1;
  }
}
void writeInit()
{
  SPI.beginTransaction(spiSetting);
  writeRegister(R7_Init);
  writeRegister(R6_STEPSEL_0_Init);
  writeRegister(R6_STEPSEL_1_Init);
  writeRegister(R5_DEVSEL_0_Init);
  writeRegister(R5_DEVSEL_1_Init);
  writeRegister(R4_CLKDIVSEL_0_Init);
  writeRegister(R4_CLKDIVSEL_1_Init);
  writeRegister(R3_Init);
  writeRegister(R2_Init);
  writeRegister(R1_Init);
  writeRegister(R0_Init);
  SPI.endTransaction();
  Serial.println("Finished initialising the registers of ADF4159 ");
}
void writeConfig()
{
  SPI.beginTransaction(spiSetting);
  writeRegister(R7_Config);
  writeRegister(R6_Config);
  writeRegister(R5_Config);
  writeRegister(R4_Config);
  writeRegister(R3_Config);
  writeRegister(R2_Config);
  writeRegister(R1_Config);
  writeRegister(R0_Config);
  SPI.endTransaction();
  Serial.println("Finished configurating the registers of ADF4159 ");
}
void loop()
{
  if (incomingByte = Serial.available() > 0) {

    char byteIn = Serial.read();
    cmd[cmdIndex] = byteIn;

    if (byteIn == '\n')
    {
      // Command finished
      SPI.begin();
      delay(1000);

      cmd[cmdIndex - 1] = '\0';
      Serial.println(cmd);
      cmdIndex = 0;

      if (strcmp(cmd, "one")  == 0) {
        Serial.println("Command received: one ");
        writeInit();
      } else if (strcmp(cmd, "two")  == 0) {
        Serial.println("Command received: two ");
        writeConfig();
      } else if (strcmp(cmd, "three")  == 0) {
        Serial.println("Command received: three");
        readRegister();
      } else {
        Serial.println("Command received: unknown!");
      }
      SPI.end();
      delay(1000);
    } else
    {
      if (cmdIndex++ >= MAX_CMD_LENGTH) {
        cmdIndex = 0;
      }
    }
  }
}

