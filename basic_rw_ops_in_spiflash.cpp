#include <Arduino.h>
#include <spi_flash.h>

#define FLASH_ADDR 0x300000   // Safe flash address (4KB aligned)

char userInput[64];

// Clear UART RX buffer
void clearSerialBuffer()
{
  while (Serial.available())
  {
    Serial.read();
  }
}

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nESP8266 Flash Read / Write Example");
  Serial.println("----------------------------------");

  clearSerialBuffer();

  Serial.println("Enter your name and press ENTER:");
}

void loop()
{
  if (Serial.available())
  {
    int len = Serial.readBytesUntil('\n', userInput, sizeof(userInput) - 1);

    userInput[len] = '\0';

    // Remove carriage return if present
    if (len > 0 && userInput[len - 1] == '\r')
    {
      userInput[len - 1] = '\0';
    }

    Serial.print("\nUser entered: ");
    Serial.println(userInput);

    uint32_t sector = FLASH_ADDR / SPI_FLASH_SEC_SIZE;

    // -------- ERASE FLASH SECTOR --------
    Serial.println("\nErasing flash sector...");

    if (spi_flash_erase_sector(sector) != SPI_FLASH_RESULT_OK)
    {
      Serial.println("Flash erase failed!");
      return;
    }

    Serial.println("Sector erased");

    // -------- WRITE DATA --------
    uint8_t writeBuffer[64] = {0};

    strcpy((char *)writeBuffer, userInput);

    Serial.println("Writing data to flash...");

    if (spi_flash_write(FLASH_ADDR, (uint32_t *)writeBuffer, strlen(userInput) + 1) != SPI_FLASH_RESULT_OK)
    {
      Serial.println("Flash write failed!");
      return;
    }

    Serial.println("Write successful");

    // -------- READ DATA --------
    uint8_t readBuffer[64] = {0};

    Serial.println("Reading from flash...");

    if (spi_flash_read(FLASH_ADDR, (uint32_t *)readBuffer, sizeof(readBuffer)) != SPI_FLASH_RESULT_OK)
    {
      Serial.println("Flash read failed!");
      return;
    }

    Serial.print("\nRead from flash: ");
    Serial.println((char *)readBuffer);

    Serial.println("\nEnter another name:");

    clearSerialBuffer();
  }
}