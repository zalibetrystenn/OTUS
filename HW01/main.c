#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include "circularBuffer.h"
#include <errno.h>

#define EOCD_SIGNATURE 0x06054b50
#define LFL_SIGNATURE 0x04034b50
#define CDFH_SIGNATURE 0x02014b50

#define IS_EXIST_DATA_DESCRIPTOR 0x4

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
  // uint8_t *filename;
  // // Дополнительные данные (размером extraFieldLength)
  // uint8_t *extraField;
} TLocalFileHeader;
#pragma pack()

#pragma pack(1)
typedef struct SCentralDirectoryFileHeader
{
  // Обязательная сигнатура, равна 0x02014b50
  uint32_t signature;
  // Версия для создания
  uint16_t versionMadeBy;
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
  // Длина комментариев к файлу
  uint16_t fileCommentLength;
  // Номер диска
  uint16_t diskNumber;
  // Внутренние аттрибуты файла
  uint16_t internalFileAttributes;
  // Внешние аттрибуты файла
  uint32_t externalFileAttributes;
  // Смещение до структуры LocalFileHeader
  uint32_t localFileHeaderOffset;
  // Имя файла (длиной filenameLength)
  // uint8_t *filename;
  // // Дополнительные данные (длиной extraFieldLength)
  // uint8_t *extraField;
  // // Комментарий к файла (длиной fileCommentLength)
  // uint8_t *fileComment;
} TCentralDirectoryFileHeader;
#pragma pack()

void readLocalFileHeader(FILE *f, int countFilesInZip)
{
  TCentralDirectoryFileHeader CentralDirectoryFileHeaderBuffer = {0};
  int kursorPositionOffset = 0;
  bool isContunue = true;
  int countFileHeaderBuffer = 1;

  while (isContunue)
  {
    fseek(f, kursorPositionOffset, SEEK_SET);

    if (feof(f))
    {
      isContunue = false;
      continue;
    }

    fread(&CentralDirectoryFileHeaderBuffer, sizeof(CentralDirectoryFileHeaderBuffer), 1, f);

    if (CentralDirectoryFileHeaderBuffer.signature == CDFH_SIGNATURE)
    {
      printf("LocalFileHeader signature is %x count: %d, kursorPositionOffset: %d\n", CentralDirectoryFileHeaderBuffer.signature, countFileHeaderBuffer, kursorPositionOffset);
      printf("filenameLength: %d, files count: %d\n", CentralDirectoryFileHeaderBuffer.filenameLength, countFileHeaderBuffer);

      char *str = malloc(sizeof(char) * CentralDirectoryFileHeaderBuffer.filenameLength + 1);
      fread(str, CentralDirectoryFileHeaderBuffer.filenameLength, 1, f);
      str[CentralDirectoryFileHeaderBuffer.filenameLength] = '\0';
      printf("file number %d is %s\n\n", countFileHeaderBuffer, str);

      if (str != NULL)
        free(str);

      countFileHeaderBuffer++;
      if (countFileHeaderBuffer == countFilesInZip)
        isContunue = false;
      else
        kursorPositionOffset++;
      continue;
    }
    else
    {
      kursorPositionOffset++;
    }
  }
}

size_t getSignaturePosition(FILE *f, sCircularBuffer *buff)
{
  size_t count = 0;

  uint8_t ch = 0;
  printf("getSignaturePosition()\n");
  circular_buffer_init(buff);
  while (1)
  {
    if (feof(f))
    {
      exit(1);
    }

    printf("getSignaturePosition()\n");
    fread(&ch, sizeof(ch), 1, f);
    count++;
    printf("count from getSignaturePosition: %ld, ch = %x\n", count, ch);

    if (circular_buffer_put(buff, ch))
    {
      return count;
    }
  }

  return count;
}

void readLocalFileHeaderOptimized(FILE *f, int countFilesInZip)
{
  TLocalFileHeader LocalFileHeaderBuffer = {0};
  bool isContunue = true;
  int countFileHeaderBuffer = 1;
  sCircularBuffer buff;
  size_t countOfReadedBytes = 0;

  countOfReadedBytes = getSignaturePosition(f, &buff);
  printf("countOfReadedBytes: %ld\n", countOfReadedBytes);

  if (countOfReadedBytes < 2)
    exit(1);

  fseek(f, countOfReadedBytes - 4, SEEK_SET);

  while (isContunue)
  {

    if (feof(f))
    {
      isContunue = false;
      continue;
    }

    fread(&LocalFileHeaderBuffer, sizeof(LocalFileHeaderBuffer), 1, f);

    if (LocalFileHeaderBuffer.signature == LFL_SIGNATURE)
    {
      printf("LocalFileHeader signature is %x count: %d\n", LocalFileHeaderBuffer.signature, countFileHeaderBuffer);
      printf("filenameLength: %d, files count: %d\n", LocalFileHeaderBuffer.filenameLength, countFileHeaderBuffer);
      printf("Flags %x\n", LocalFileHeaderBuffer.generalPurposeBitFlag);

      char *str = malloc(sizeof(char) * LocalFileHeaderBuffer.filenameLength + 1);
      fread(str, LocalFileHeaderBuffer.filenameLength, 1, f);
      str[LocalFileHeaderBuffer.filenameLength] = '\0';
      printf("file number %d is %s\n\n", countFileHeaderBuffer, str);
      countFileHeaderBuffer++;

      if (str != NULL)
        free(str);

      if (countFileHeaderBuffer == countFilesInZip)
      {
        isContunue = false;
        continue;
      }

      if (LocalFileHeaderBuffer.compressionMethod == 0)
        fseek(f, LocalFileHeaderBuffer.extraFieldLength + LocalFileHeaderBuffer.uncompressedSize, SEEK_CUR);
      else
        fseek(f, LocalFileHeaderBuffer.extraFieldLength + LocalFileHeaderBuffer.compressedSize, SEEK_CUR);

      if (LocalFileHeaderBuffer.generalPurposeBitFlag & IS_EXIST_DATA_DESCRIPTOR)
        fseek(f, sizeof(uint32_t) * 4, SEEK_CUR);
    }
    else
    {
      // fseek(f, sizeof(LocalFileHeaderBuffer) - 1, SEEK_CUR);
      //  printf("ELSE\n\n");
    }
  }
}

void showFileName(FILE *f, int countFilesInZip)
{
  TLocalFileHeader LocalFileHeaderBuffer = {0};
  int kursorPositionOffset = 0;
  bool isContunue = true;
  int countFileHeaderBuffer = 1;

  while (isContunue)
  {
    fseek(f, kursorPositionOffset, SEEK_SET);

    if (feof(f))
    {
      isContunue = false;
      continue;
    }

    fread(&LocalFileHeaderBuffer, sizeof(LocalFileHeaderBuffer), 1, f);

    if (LocalFileHeaderBuffer.signature == LFL_SIGNATURE)
    {
      printf("LocalFileHeader signature is %x count: %d, kursorPositionOffset: %d\n", LocalFileHeaderBuffer.signature, countFileHeaderBuffer, kursorPositionOffset);
      printf("filenameLength: %d, files count: %d\n", LocalFileHeaderBuffer.filenameLength, countFileHeaderBuffer);

      char str[100];

      fread(&str, sizeof(LocalFileHeaderBuffer.filenameLength), 1, f);
      str[LocalFileHeaderBuffer.filenameLength + 1] = '\0';
      printf("file number %d is %s, length of str is %ld\n\n", countFileHeaderBuffer, str, strlen(str));

      countFileHeaderBuffer++;
      if (countFileHeaderBuffer == countFilesInZip)
        isContunue = false;
      else
        kursorPositionOffset++;
      continue;
    }
    else
    {
      kursorPositionOffset++;
    }
  }
}

bool findEOCD(FILE *f, int *countFilesInZip)
{
  TEOCD EOCDBuffer = {0};
  int kursorPositionOffset = -22;
  bool isContunue = true;
  bool isFindEOCD = false;

  while (isContunue)
  {
    fseek(f, kursorPositionOffset, SEEK_END);

    if (ftell(f) == 0)
    {
      isContunue = false;
      continue;
    }
    fread(&EOCDBuffer, sizeof(EOCDBuffer), 1, f);

    if (EOCDBuffer.signature == EOCD_SIGNATURE)
    {
      printf("EOCD signature is %x\n\n", EOCDBuffer.signature);

      *countFilesInZip = EOCDBuffer.numberCentralDirectoryRecord;
      isFindEOCD = true;
      isContunue = false;
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

int main(int argc, char *argv[])
{
  if (argc == 0 || argc > 2)
  {
    return 1;
  }

  FILE *f;

  bool isFindEOCD = false;
  int countFilesInZip = 0;

  f = fopen(argv[1], "rb");
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

      readLocalFileHeaderOptimized(f, countFilesInZip);
    }
    else
    {
      printf("Внутри файла нет архива\n");
    }
  }
  fclose(f);

  return 0;
}
