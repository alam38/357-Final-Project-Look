#ifndef LOOK_H
#define LOOK_H

typedef struct {
   char *fullLine;
   char *word;
   int matched;
} LineStruct;

void ParseFile(LineStruct **lines, FILE *infile, int *lineNum, int compSize,
 int dFlag);

void MakeLine(int *lCalloc, int lineSize, char **line, char letter);
void PrintLines(LineStruct *lines, int lineNum);

void Search(LineStruct **lines, char *comp, int lineNum, int dFlag,
 int tFlag, int fFlag, char tArg);

void CheckArgs(int *dFlag, int *tFlag, int *fFlag, char **comp, char **file,
 char *argv[], int argc, char *tArg);

void ReallocStruct(LineStruct **lines, int *sCalloc, int lineNum);

LineStruct MakeStruct(char *line, int compSize, int dFlag);

char *ReallocLine(int lCalloc, char **line);

int Compare(char *word, char *comp, int dFlag, int tFlag, int fFlag, char tArg);

#endif
