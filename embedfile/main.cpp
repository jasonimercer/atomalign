/**
 * Software License Agreement CC0
 *
 * \file      main.cpp
 * \author    Jason Mercer <jason.mercer@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see http://creativecommons.org/publicdomain/zero/1.0/
 */

#include <stdio.h>

// This program encodes a source file into a constant C string
int main(int argc, char** argv)
{
  if (argc != 4)
  {
    fprintf(stderr, "This is a tool to generate embedded resources in C programs.\n");
    fprintf(stderr, "Expected <source_filename> <dest_filename> <dest_variablename>\n");
    return 1;
  }

  const char* source_filename = argv[1];
  const char* dest_filename = argv[2];
  const char* dest_variablename = argv[3];

  FILE* r = fopen(source_filename, "r");
  FILE* w = fopen(dest_filename, "w");

  if (!r)
  {
    fprintf(stderr, "Failed to open '%s' for reading\n", source_filename);
    return 2;
  }

  if (!w)
  {
    fprintf(stderr, "Failed to open '%s' for writing", dest_filename);
    return 3;
  }

  fprintf(w, "// This file was generated with the command:\n");
  fprintf(w, "// %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
  fprintf(w, "\n");
  fprintf(w, "static const char %s[] = {\n", dest_variablename);

  char letter;
  int line_count = 0;
  while (fread(&letter, 1, 1, r))
  {
    fprintf(w, "0x%02x, ", letter);
    line_count++;
    if (line_count >= 16)
    {
      fprintf(w, "\n");
      line_count = 0;
    }
  }

  fprintf(w, "0x%02x};", 0);
  fclose(w);
  return 0;
}
