#include <Arduino.h>
#include <spi_flash.h>

#define FLASH_ADDR 0x300000

String userInput = "";

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nESP8266 Flash Read / Write Example");
  Serial.println("----------------------------------");
  Serial.println("Enter your name then press ENTER:");
}

void loop()
{
  while (Serial.available())
  {
    char c = Serial.read();

    // Detect ENTER (works for \n, \r, or \r\n)
    if (c == '\n' || c == '\r')
    {
      if (userInput.length() == 0) return;

      userInput.trim();

      Serial.println();
      Serial.print("User entered: ");
      Serial.println(userInput);

      uint32_t sector = FLASH_ADDR / SPI_FLASH_SEC_SIZE;

      Serial.println("\nErasing flash sector...");

      if (spi_flash_erase_sector(sector) != SPI_FLASH_RESULT_OK)
      {
        Serial.println("Flash erase failed!");
        userInput = "";
        return;
      }

      Serial.println("Sector erased");

      uint8_t writeBuffer[64] = {0};
      userInput.toCharArray((char*)writeBuffer, sizeof(writeBuffer));

      uint32_t dataSize = userInput.length() + 1;

      Serial.println("Writing data to flash...");

      if (spi_flash_write(FLASH_ADDR, (uint32_t*)writeBuffer, dataSize) != SPI_FLASH_RESULT_OK)
      {
        Serial.println("Flash write failed!");
        userInput = "";
        return;
      }

      Serial.println("Write successful");

      uint8_t readBuffer[64] = {0};

      Serial.println("Reading from flash...");

      if (spi_flash_read(FLASH_ADDR, (uint32_t*)readBuffer, sizeof(readBuffer)) != SPI_FLASH_RESULT_OK)
      {
        Serial.println("Flash read failed!");
        userInput = "";
        return;
      }

      Serial.print("\nRead from flash: ");
      Serial.println((char*)readBuffer);

      Serial.println("\nEnter another name then press ENTER:");

      userInput = "";
    }
    else
    {
      userInput += c;
      Serial.print(c); // echo typed characters
    }
  }
}