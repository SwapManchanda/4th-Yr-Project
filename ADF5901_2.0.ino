#include <SPI.h>


int PIN_CE = 38;
int CS = 4;


SPISettings spiSetting(115200, MSBFIRST, SPI_MODE0);

unsigned long step1_R7 = 0x02000007;
unsigned long step2_R11 = 0x0000002B;
unsigned long step3_R11 = 0x0000000B;
unsigned long step4_R10 = 0x1032A64A;
unsigned long step5_R9 = 0x2A20B929;
unsigned long step6_R8 = 0x40003E88;
unsigned long step7_R0 = 0x809FE520;
unsigned long step8_R7 = 0x011F4827;
unsigned long step9_R6 = 0x00000006;
unsigned long step10_R5 = 0x01E00005;
unsigned long step11_R4 = 0x00200004;
unsigned long step12_R3 = 0x01893803;
unsigned long step13_R2 = 0x00020642;
unsigned long step14_R1 = 0xFFF7FFE1;
unsigned long step15_R0 = 0x809FE720;
unsigned long step16_R0 = 0x809FE560;
unsigned long step17_R0 = 0x809FED60;
unsigned long step18_R0 = 0x809FE5A0;
unsigned long step19_R0 = 0x809FF5A0;
unsigned long step20_R9 = 0x2800B929;
unsigned long step21_R0 = 0x809F25A0;

unsigned long step1_ReCalibration_R0 = 0x819FE520;
unsigned long step2_ReCalibration_R9 = 0x2A20B929;
unsigned long step3_ReCalibration_R1 = 0xFFF7FFE1;
unsigned long step4_ReCalibration_R0 = 0x819FE720;
unsigned long step5_ReCalibration_R0 = 0x819FE560;
unsigned long step6_ReCalibration_R0 = 0x819FED60;
unsigned long step7_ReCalibration_R0 = 0x819FE5A0;
unsigned long step8_ReCalibration_R0 = 0x809FF5A0;
unsigned long step9_ReCalibration_R9 = 0x2800B929;
unsigned long step10_ReCalibration_R0 = 0x819F25A0;

int MAX_CMD_LENGTH = 20;
char cmd[20];
int cmdIndex;
char incomingByte;

void setup()
{ // put your setup code here, to run once:
  pinMode(PIN_CE, OUTPUT);
  SPI.begin(4);
  pinMode(CS, OUTPUT);
  pinMode(MOSI, OUTPUT);
  digitalWrite(PIN_CE, HIGH);
  digitalWrite(MOSI, LOW);
  digitalWrite(SS, HIGH);
  Serial.begin(115200);
  cmdIndex = 0;
  Serial.println("Enter integer related to mode of operation: [1][2][3]");
  Serial.println("[1] Initialisation Mode. Type into serial command 'one'.");
  Serial.println("[2] Ramp Re-Configuration Mode. Type into serial command 'two'.");
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
  int variable32[32];
  CrackLong32( val,  variable32 );
  Serial.println("About to transfer the following sequence: ");
  for (int a = 0; a < 32; a++)
  {
    Serial.print(variable32[a]);
  }
  Serial.println();
  digitalWrite(CS, LOW);
  unsigned long leVal = toLittleEndian(val);
  SPI.transfer(&leVal, 4);
  digitalWrite(CS, HIGH);
}
unsigned long readRegister()
{

  int variable32[32];
  SPI.beginTransaction(spiSetting);
  digitalWrite(CS, LOW);
  uint8_t buff[4];
  unsigned long DOUT = 0x00;
  for (int i = 3; i >= 0; i--)
  {
    byte b = SPI.transfer(0x00);
    buff[i] = b;
    //result_37_part1 = (result_37_part1<<(i*8)) |(b1);
  }
  DOUT = *buff;

  Serial.println(DOUT, BIN);
  Serial.println();
  digitalWrite(CS, HIGH);
  SPI.endTransaction();
  Serial.println("Finished reading DOUT of ADF5901 ");


}

void CrackLong32( unsigned long b, int result[32] )
{
  for (int i = 0; i < 32; ++i )
  {
    result[i] = b & 1;
    b = b >> 1;
  }
}
void writeInit()
{
  SPI.beginTransaction(spiSetting);
  writeRegister(step1_R7);
  writeRegister(step2_R11);
  writeRegister(step3_R11);
  writeRegister(step4_R10);
  writeRegister(step5_R9);
  writeRegister(step6_R8);
  writeRegister(step7_R0);

  writeRegister(step8_R7);
  writeRegister(step9_R6);
  writeRegister(step10_R5);
  writeRegister(step11_R4);
  writeRegister(step12_R3);
  writeRegister(step13_R2);
  writeRegister(step14_R1);
  writeRegister(step15_R0);

  writeRegister(step16_R0);
  writeRegister(step17_R0);
  delayMicroseconds(400);
  writeRegister(step18_R0);
  writeRegister(step19_R0);

  writeRegister(step20_R9);
  writeRegister(step21_R0);

  SPI.endTransaction();
  Serial.println("Finished initialising the registers of ADF5901 ");
}

void writeInit_reCalibrationSequence()
{
  SPI.beginTransaction(spiSetting);
  writeRegister(step1_ReCalibration_R0);
  writeRegister(step2_ReCalibration_R9);
  writeRegister(step3_ReCalibration_R1);
  writeRegister(step4_ReCalibration_R0);
  delayMicroseconds(800);
  writeRegister(step5_ReCalibration_R0);
  writeRegister(step6_ReCalibration_R0);
  delayMicroseconds(400);
  writeRegister(step7_ReCalibration_R0);
  writeRegister(step8_ReCalibration_R0);
  delayMicroseconds(400);
  writeRegister(step9_ReCalibration_R9);
  writeRegister(step10_ReCalibration_R0);
  SPI.endTransaction();
  Serial.println("Finished recalibrating the registers of ADF5901 ");
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

      cmd[cmdIndex - 1] = '\0';
      cmdIndex = 0;
      Serial.println(cmd);


      if (strcmp(cmd, "one")  == 0) {
        Serial.println("Command received: one ");
        writeInit();
      } else if (strcmp(cmd, "two")  == 0) {
        Serial.println("Command received: two");
        writeInit_reCalibrationSequence();
      } else {
        Serial.println("Command received: unknown!");
      }
      SPI.end();
      delay(1000);
    }
    else
    {
      if (cmdIndex++ >= MAX_CMD_LENGTH)
      {
        cmdIndex = 0;
      }
    }
  }
}
