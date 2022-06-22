#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#define EOCD_SIGNATURE 0x06054b50
#define LFL_SIGNATURE 0x04034b50

#pragma pack(1)
typedef struct SEOCD
{
  // Обязательная сигнатура равна 0x0605b50
  uint32_t signature;
  // Номер диска
  uint16_t diskNumber;
  // Номер диска, где находится начало Central Directory
  uint16_t startDiskNumber;
  // Количество записей в Central Directory в текущем диске
  uint16_t numberCentralDirectoryRecord;
  // Всего записей в Central Directory
  uint16_t totalCentralDirectoryRecord;
  // Размер Central Directory
  uint32_t sizeOfCentralDirectoryRecord;
  // Смещение Central Directory
  uint32_t centralDirectoryOffset;
  // Длина комментария
  uint16_t commentLength;
} TEOCD;
#pragma pack()

#pragma pack(1)
typedef struct SLocalFileHeader
{
  // Обязательная сигнатура равна 0x04034b50;
  uint32_t signature;
  // Минимальная версия для распаковки
  uint16_t versionToExtract;
  // Битовый флаг
  uint16_t generalPurposeBitFlag;
  // Метод сжатия (0 - без сжатия, 8 - deflate)
  uint16_t compressionMethod;
  // Время модификации файла
  uint16_t modificationTime;
  // Дата модификации файла
  uint16_t modificationDate;
  // Контрольная сумма
  uint32_t crc32;
  // Сжатый размер
  uint32_t compressedSize;
  // Несжатый размер
  uint32_t uncompressedSize;
  // Длина название файла
  uint16_t filenameLength;
  // Длина поля с дополнительными данными
  uint16_t extraFieldLength;
  // Название файла (размером filenameLength)
  uint8_t filename[];
  // // Дополнительные данные (размером extraFieldLength)
  // uint8_t *extraField;
} TLocalFileHeader;
#pragma pack()

void showFileName(FILE *f, int countFilesInZip)
{
  TLocalFileHeader LocalFileHeaderBuffer = {0};
  int kursorPositionOffset = -22;
  bool isExit = true;
  int countFileHeaderBuffer = 1;

  while (isExit)
  {
    fseek(f, kursorPositionOffset, SEEK_END);

    if (ftell(f) == 0)
    {
      isExit = false;
      continue;
    }

    fread(&LocalFileHeaderBuffer, sizeof(LocalFileHeaderBuffer), 1, f);

    if (LocalFileHeaderBuffer.signature == LFL_SIGNATURE)
    {
      printf("signature is %x count: %d, kursorPositionOffset: %d\n", LocalFileHeaderBuffer.signature, countFileHeaderBuffer, kursorPositionOffset);
      printf("modificationTime: %d count: %d\n", LocalFileHeaderBuffer.modificationTime, countFileHeaderBuffer);
      printf("filenameLength: %d count: %d\n\n", LocalFileHeaderBuffer.filenameLength, countFileHeaderBuffer);

      char *str = malloc(sizeof(char) * LocalFileHeaderBuffer.filenameLength + 1);
      if (str != NULL)
      {
        memcpy(str, LocalFileHeaderBuffer.filename, LocalFileHeaderBuffer.filenameLength);
        str[LocalFileHeaderBuffer.filenameLength + 1] = '\0';
        printf("file number %d is %s, длинна строки str равна = %ld\n", countFileHeaderBuffer, str, strlen(str));
      }

      countFileHeaderBuffer++;
      if (countFileHeaderBuffer == countFilesInZip)
        isExit = false;
      else
        kursorPositionOffset--;
      continue;
    }
    else
    {
      kursorPositionOffset--;
    }
  }
}

bool findEOCD(FILE *f, int *countFilesInZip)
{
  TEOCD EOCDBuffer = {0};
  int kursorPositionOffset = -22;
  bool isExit = true;
  bool isFindEOCD = false;

  while (isExit)
  {
    fseek(f, kursorPositionOffset, SEEK_END);

    if (ftell(f) == 0)
    {
      isExit = false;
      continue;
    }
    fread(&EOCDBuffer, sizeof(EOCDBuffer), 1, f);

    if (EOCDBuffer.signature == EOCD_SIGNATURE)
    {
      printf("signature is %x\n", EOCDBuffer.signature);

      *countFilesInZip = EOCDBuffer.numberCentralDirectoryRecord;
      isFindEOCD = true;
      isExit = false;
      continue;
    }
    else
    {
      kursorPositionOffset--;
    }
  }
  fseek(f, 0, SEEK_SET);

  return isFindEOCD;
}

int main()
{
  FILE *f;

  bool isFindEOCD = false;
  int countFilesInZip = 0;

  f = fopen("zipjpeg.jpg", "rb");
  if (f == NULL)
  {
    perror("Ошибка! Не удалось открыть файл на чтение");
  }
  else
  {
    isFindEOCD = findEOCD(f, &countFilesInZip);
    if (isFindEOCD)
    {
      printf("Этот файл содержит внутри архив\n");

      printf("numberCentralDirectoryRecord is %d\n", countFilesInZip);

      showFileName(f, countFilesInZip);
    }
  }
  fclose(f);

  return 0;
}
