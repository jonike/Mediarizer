#include "jpegOrganizer.h"
#include "exif.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h> 
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <fcntl.h>
#include <errno.h>

#define ACR   "\x1b[31m"
#define ACG   "\x1b[32m"
#define ACRE  "\x1b[0m"

using namespace std;

int transfer(const char *source, const char *dest){

  char* transfer = (char*) malloc (32768 * sizeof(char));

  strcpy(transfer, "sudo cp \"");
  strcat(transfer, source);
  strcat(transfer, "\" ");
  strcat(transfer, dest);

  printf("cp :: %s\n", transfer);
  system(transfer);

  return 0;

}

string dateOfCreation(const char *path){

  // Read the JPEG file into a buffer
  FILE *fp = fopen(path, "rb");
  if (!fp) {
    printf("Can't open file.\n");
    return string(); // EMPTY STRING
  }
  fseek(fp, 0, SEEK_END);
  unsigned long fsize = ftell(fp);
  rewind(fp);
  unsigned char *buf = new unsigned char[fsize];
  if (fread(buf, 1, fsize, fp) != fsize) {
    printf("Can't read file.\n");
    delete[] buf;
    return string(); // EMPTY STRING
  }
  fclose(fp);

  // Parse EXIF
  easyexif::EXIFInfo result;
  int code = result.parseFrom(buf, fsize);
  delete[] buf;
  if (code) {
    printf("Error parsing EXIF: code %d\n", code);
    return string(); // EMPTY STRING
  }

  // Dump EXIF information
  return result.DateTimeOriginal.c_str();
  
}

int isRegularFile(const char *path){

    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);

}

originalDateData *dateReturn(string originalDate){

  string originalYear   ;
  string originalMonth  ; 
  string originalDay    ; 
  string originalHour   ; 
  string originalMinute ; 
  string originalSecond ;

  string div = ":";
  string divt = " ";
  size_t pos = 0;
  
  pos = originalDate.find(div);  
  originalYear = originalDate.substr(0, pos);
  originalDate.erase(0, pos + div.length());

  pos = originalDate.find(div);  
  originalMonth = originalDate.substr(0, pos);
  originalDate.erase(0, pos + div.length());

  pos = originalDate.find(divt);  
  originalDay = originalDate.substr(0, pos);
  originalDate.erase(0, pos + divt.length());

  pos = originalDate.find(div);  
  originalHour = originalDate.substr(0, pos);
  originalDate.erase(0, pos + div.length());

  pos = originalDate.find(div);  
  originalMinute = originalDate.substr(0, pos);
  originalDate.erase(0, pos + div.length());
  
  pos = originalDate.find(div);  
  originalSecond = originalDate.substr(0, pos);
  originalDate.erase(0, pos + div.length());
  
  originalDateData *origin = (originalDateData *) malloc (sizeof(originalDateData));

  origin->year   = atoi  (originalYear.c_str());
  origin->month  = atoi  (originalMonth.c_str()); 
  origin->day    = atoi  (originalDay.c_str()); 
  origin->hour   = atoi  (originalHour.c_str()); 
  origin->minute = atoi  (originalMinute.c_str());
  origin->second = atoi  (originalSecond.c_str());

  return origin;

}

bool dirEx(const char* directory){
    
  DIR* dir = opendir(directory);
  if (dir) {
    closedir(dir);
    return true;
  }

  return false;

}

char *destinationFinder(int year, int month){
  
  char *destinationPath = (char *) malloc (32768 * sizeof(char));
  char *destinationPathFolder = (char *) malloc (32768 * sizeof(char));

  strcpy(destinationPathFolder, "mkdir ");

  if(year >= 1989 && year <= 2040){
    char yearStr[12];
    sprintf(yearStr, "%d", year);

    strcpy(destinationPath, "photosOrganized/");
    strcat(destinationPath, yearStr);
    strcat(destinationPath, "/");
    
    strcat(destinationPathFolder, destinationPath);
    if(!dirEx(destinationPath))
      system(destinationPathFolder);

  }else{
    strcpy(destinationPath, "photosOrganized/unknownDate");
    
    strcat(destinationPathFolder, destinationPath);
    if(!dirEx(destinationPath))
      system(destinationPathFolder);

    return destinationPath;
  }

  if(month == 1){
    strcat(destinationPath, "January");
  }else if(month == 2){
    strcat(destinationPath, "February");
  }else if(month == 3){
    strcat(destinationPath, "March");
  }else if(month == 4){
    strcat(destinationPath, "April");
  }else if(month == 5){
    strcat(destinationPath, "May");
  }else if(month == 6){
    strcat(destinationPath, "June");
  }else if(month == 7){
    strcat(destinationPath, "July");
  }else if(month == 8){
    strcat(destinationPath, "August");
  }else if(month == 9){
    strcat(destinationPath, "September");
  }else if(month == 10){
    strcat(destinationPath, "October");
  }else if(month == 11){
    strcat(destinationPath, "November");
  }else if(month == 12){
    strcat(destinationPath, "December");
  }

  strcpy(destinationPathFolder, "mkdir ");
  strcat(destinationPathFolder, destinationPath);
  if(!dirEx(destinationPath))
    system(destinationPathFolder);

  return destinationPath;

}

void jpegVersion(const char *path){

  string originalDate = dateOfCreation(path);
  
  if(originalDate.empty()){ 
    
    FILE *corruptedFiles = fopen("corruptedFiles.txt", "a");
    if(corruptedFiles == NULL){printf("Error while opening file.\n");}
    fprintf(corruptedFiles,"%s",path);            

    return;

  }

  originalDateData *originalDateStruct = (originalDateData *) malloc (sizeof(originalDateData));
  originalDateStruct = dateReturn(originalDate);
  char *destinationPath = destinationFinder(originalDateStruct->year, originalDateStruct->month);

  transfer(path, destinationPath);
  
  return;

  // cout << "originalYear   : " << originalDateStruct->year   << endl;
  // cout << "originalMonth  : " << originalDateStruct->month  << endl;
  // cout << "originalDay    : " << originalDateStruct->day    << endl;
  // cout << "originalHour   : " << originalDateStruct->hour   << endl;
  // cout << "originalMinute : " << originalDateStruct->minute << endl;
  // cout << "originalSecond : " << originalDateStruct->second << endl;

}

void folderVersion(const char *path){
 
  int j = 1;
  DIR           *d;
  struct dirent *dir;
  d = opendir(path);
  
  string dirFile;
  string jpegO = ".jpg";
  string jpegT = ".JPG";

  size_t foundO;
  size_t foundT;

  char* imagePath   = (char*) malloc (32768 * sizeof(char));
  char* unknownPath = (char*) malloc (32768 * sizeof(char));

  strcpy(imagePath, path);
  strcat(imagePath, "/");

  strcpy(unknownPath, path);
  strcat(unknownPath, "/");

  if (d) {
    
    while ((dir = readdir(d)) != NULL){

      dirFile = dir->d_name;
      foundO = dirFile.find(jpegO);
      foundT = dirFile.find(jpegT);

      strcat(unknownPath, dir->d_name);
      //printf("unknownPath = %s\n", unknownPath);

      if (foundO != string::npos || foundT != string::npos){

        strcat(imagePath, dir->d_name);

        int length = (int) strlen(imagePath);
        for(int i = 0; i < length; i++){
          if(imagePath[i] == '.' && i+1 <= length && imagePath[i+1] == '_'){
            j = 0; break; 
          }
        }

        if(j)
          jpegVersion(imagePath);
        

        j = 1;
        strcpy(imagePath, path);
        strcat(imagePath, "/");

      }else if(!isRegularFile(unknownPath) && (dirFile.find('.') == std::string::npos)){
        //printf("ENTERING->%s)\n",unknownPath);
        folderVersion(unknownPath);
        //printf("  LEAVING<-%s)\n",unknownPath);
      }

      strcpy(unknownPath, path);
      strcat(unknownPath, "/");

    }

    closedir(d);
    return ;
    
  }
  
}