#include "file.h"
#include "explorer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct file *file_new(int type, char *name)
{
  /* Initialization */
  struct file *file = NULL;
  /* Check for integrity */
  if (!name || type < 0 || type >= MAX_FT_NO)
  {
    return NULL;
  }
  /* Allocate memory and initialze the file. */
  file = calloc(1, sizeof(struct file));
  file->type = type;
  file->size = DEFAULT_FILE_SIZE;
  file->data = calloc(file->size, 1);
  /* Crate associtate node and set it to base. */
  file->base = node_new(false, name, file);
  return file;
}

void file_release(struct file *file)
{
  /* Check for null pointer. */
  if (!file)
  {
    return;
  }
  /* relase the data. */
  /* Check if base has already been released. Prevent circular call. */
  if (file->base)
  {
    file->base->inner.file = NULL;
    node_release(file->base);
  }
  /* Release the resources. */
  free(file->data);
  free(file);
}

bool file_write(struct file *file, int offset, int bytes, const char *buf)
{
  // file of buff is NULL
  if (file == NULL || buf == NULL) {
    return false; 
  }
  // invalid offset
  if (offset < 0) {
    return false; 
  }
  if (offset+bytes > file->size) {
  // offset+bytes exceeds the current size 
    char* temp = (char *)realloc(file->data, offset+bytes);
    if (temp == NULL)
      return false;
    file->data = temp;
  }
  // rewrite
  for (int i = offset; i != offset+bytes; i++) {
    file->data[i] = buf[i-offset];
  }
  // upgrade the size of the new file
  file->size = offset+bytes > file->size ? offset+bytes: file->size;
  return true;
}

bool file_read(const struct file *file, int offset, int bytes, char *buf)
{
  if(file == NULL) {
  // handle the null pointer
    return false;
  }
  if (offset < 0 || offset + bytes > file->size) {
  //handle the invalid arguments
    return false;
  }
  
  for (int i = 0; i!= bytes; i++)
  // read the file
    buf[i] = file->data[i+offset];
  return true;
}
