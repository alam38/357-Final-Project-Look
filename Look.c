#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Look.h"
#define ALLOCSIZE 1000
#define STRSIZE 50
#define ASCIISHIFT 32

int main(int argc, char *argv[]) {

   LineStruct *lines;
   FILE *infile;

   int lineNum = 0, dFlag = 0, tFlag = 0, fFlag = 0;

   char *comp, *file = NULL;

   char tArg;   

   CheckArgs(&dFlag, &tFlag, &fFlag, &comp, &file, argv, argc, &tArg);   

   lines = calloc(ALLOCSIZE, sizeof(LineStruct));

   if (file != NULL) {
      if (!(infile = fopen(file, "r"))) {
         fprintf(stderr, "%s: %s: No such file or directory\n",
          argv[0], file);
         return 2;
      }
   }
   else {
      fFlag = 1;
      dFlag = 1;
      infile = fopen("/usr/share/dict/words", "r");
   }

   ParseFile(&lines, infile, &lineNum, strlen(comp), dFlag);
   Search(&lines, comp, lineNum, dFlag, tFlag, fFlag, tArg);
   PrintLines(lines, lineNum);

   return 0;
}

void CheckArgs(int *dFlag, int *tFlag, int *fFlag, char **comp, char **file,
 char *arg[], int argc, char *tArg) {

   int ctr = 1, gotWord = 0;

   *comp = NULL;

   for (ctr = 1; ctr < argc; ctr++) {

      if (strcmp(arg[ctr], "-d") == 0) {
         *dFlag = 1;
      }      

      else if (strcmp(arg[ctr], "-t") == 0) {
         *tFlag = 1;
         ctr++;
         
         if (ctr < argc) {
            *tArg = *(arg[ctr]);
         }
         else {
            fprintf(stderr, "look: option requires an argument -- 't'\n");
            fprintf(stderr, "usage: look [-dfa] [-t char] string [file]\n");
         }
      }

      else if (strcmp(arg[ctr], "-f") == 0) {
         *fFlag = 1;
      }

      else if (*(arg[ctr]) == '-') {
         fprintf(stderr, "look: invalid option == 'o'\n");
         fprintf(stderr, "usage: look [-dfa] [-t char] string [file]\n");
         exit(2);
      }

      else if (gotWord == 0) {
         *comp = calloc(strlen(arg[ctr]) + 1, sizeof(char));
         strcpy(*comp, arg[ctr]);
         gotWord = 1;
      }

      else {
         *file = calloc(strlen(arg[ctr]) + 1, sizeof(char));
         strcpy(*file, arg[ctr]);
      }
   }

   if (*comp == NULL) {
      fprintf(stderr, "usage: look [-dfa] [-t char] string [file]\n");
      exit(2);
   }

}

void Search(LineStruct **lines, char *comp, int lineNum, int dFlag,
 int tFlag, int fFlag, char tArg) {

   int bottom = 0, top = lineNum - 1, middle = (top + bottom) / 2;

   while (bottom <= top) {
      middle = (top + bottom) / 2;

      if (Compare((*lines)[middle].word, comp, dFlag, tFlag, fFlag, tArg)
       == 0) {

         (*lines)[middle].matched = 1;
         top = middle;
         bottom = middle;

         while (++top < lineNum &&
          Compare((*lines)[top].word, comp, dFlag, tFlag, fFlag, tArg) == 0) {

            (*lines)[top].matched = 1;
         }

         while (--bottom >= 0 && Compare((*lines)[bottom].word,
          comp, dFlag, tFlag, fFlag, tArg) == 0) {

            (*lines)[bottom].matched = 1;
         }

         break;
      }
      else if (Compare((*lines)[middle].word, comp, dFlag,
       tFlag, fFlag, tArg) < 0) {

         bottom = middle + 1;
      }
      else {
         top = middle - 1;
      }
   }
}

int Compare(char *word, char *comp, int dFlag, int tFlag, int fFlag,
 char tArg) {

   char *temp1 = calloc(strlen(word) + 1, sizeof(char));

   char *temp2 = calloc(strlen(comp) + 1, sizeof(char));

   int ctr = 0, dCtr = 0;


   strcpy(temp1, word);
   strcpy(temp2, comp);

   if (dFlag == 1) {

      for (ctr = 0; ctr < strlen(temp1); ctr++) {
         if (isalnum(word[ctr])) {
            temp1[dCtr] = temp1[ctr];
            dCtr++;
         }
      }
      temp1[strlen(comp)] = '\0';

      if (tFlag == 1) {
         tArg = tArg - ASCIISHIFT;
      }
   }

   if (fFlag == 1) {

      for (ctr = 0; ctr < strlen(temp1); ctr++) {
         temp1[ctr] = toupper(temp1[ctr]);
      }
      temp1[ctr] = '\0';

      for (ctr = 0; ctr < strlen(temp2); ctr++) {
         temp2[ctr] = toupper(temp2[ctr]);
      }
      temp2[ctr] = '\0';

   }

   if (tFlag == 1) {

      for (ctr = 0; ctr < strlen(temp1); ctr++) {
         if (temp1[ctr] == tArg) {
            temp1[++ctr] = '\0';
            break;
         }
      }

      for (ctr = 0; ctr < strlen(temp2); ctr++) {

         if (temp2[ctr] == tArg) {
            temp2[++ctr] = '\0';
            break;
         }
      }     
   }

   return strcmp(temp1, temp2);

}

void PrintLines(LineStruct *lines, int lineNum) {

   int ctr = 0, printed = 0;

   for (ctr = 0; ctr < lineNum; ctr++) {
      if (lines[ctr].matched == 1) {
         fprintf(stderr, "%s\n", lines[ctr].fullLine);
         printed = 1;      
      }
   }

   if (printed == 1) {
      exit(1);
   }

}

void ParseFile(LineStruct **lines, FILE *infile, int *lineNum, int compSize,
 int tFlag) {
   
   char *line = calloc(STRSIZE, 1);

   char letter;

   int lCalloc = 1, sCalloc = 1, lineSize = 0;

   while (((letter = fgetc(infile)) != EOF)) {
      while (letter != EOF && letter != '\n') {

         MakeLine(&lCalloc, lineSize, &line, letter);
         letter = getc(infile);
         lineSize++;    
      }
      line[lineSize] = '\0';

      if (*lineNum > ((sCalloc * ALLOCSIZE) - 2)) {

         ReallocStruct(lines, &sCalloc, *lineNum);
      }
      
      (*lines)[*lineNum] = MakeStruct(line, compSize, tFlag);
      (*lineNum)++;

      lineSize = 0;
      lCalloc = 1;
   }
}

LineStruct MakeStruct(char *line, int compSize, int dFlag) {

   LineStruct tempLine;

   int ctr = 0, nonAlNum = 0;

   tempLine.fullLine = calloc(strlen(line) + 1, sizeof(char));
   strcpy(tempLine.fullLine, line);




   if (dFlag == 1) {
      for (ctr = 0; ctr < strlen(line); ctr++) {
         if (isalnum(line[ctr])) {
            nonAlNum++;
         }
      }

      tempLine.word = calloc(nonAlNum + 1, sizeof(char));
      strncpy(tempLine.word, line, nonAlNum);
   }

   else {
      tempLine.word = calloc(compSize + 1, sizeof(char));
      strncpy(tempLine.word, line, compSize);
   }

   tempLine.matched = 0;


   return tempLine;
}

void MakeLine(int *lCalloc, int lineSize, char **line, char letter) {

   if (lineSize > (*lCalloc * STRSIZE) - 2) {
      *line = ReallocLine(++(*lCalloc), line);
   }

   if (lineSize == 0) {
      **line = letter;
   }
   else {
      (*line)[lineSize] = letter;
   }
}

void ReallocStruct(LineStruct **lines, int *sCalloc, int lineNum) {

   LineStruct *temp = NULL;

   temp = calloc(sizeof(LineStruct) * ALLOCSIZE, ++(*sCalloc));
   memcpy(temp, *lines, lineNum * sizeof(LineStruct));
   *lines = temp;

}

char *ReallocLine(int lCalloc, char **line) {

   char *temp = calloc(STRSIZE, ++lCalloc);

   strcpy(temp, *line);
   return temp;
}
