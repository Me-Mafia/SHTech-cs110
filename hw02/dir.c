#include "dir.h"
/* This part is however not implemented by the owner of the autolab account... */
#include "explorer.h"
#include "file.h"
/* This part is however not implemented by the owner of the autolab account... */
#include "node.h"
#include <stdlib.h>
/* This part is however not implemented by the owner of the autolab account... */
#include <string.h>
#include <stdio.h>

/* static bool dir_add_sub(struct directory *dirnode, struct node *sub); */

struct directory *dir_new(char *name) {
  /* Initialization */
  struct directory *dir = NULL;
  /* Check for null pointer */
  if (!name) {
    return NULL;
  }
  /* Allocate memory */
  dir = calloc(1, sizeof(struct directory));
  dir->capacity = DEFAULT_DIR_SIZE;
  dir->subordinates = calloc(dir->capacity, sizeof(struct node));
  dir->parent = NULL;
  /* Create base node */
  dir->base = node_new(true, name, dir);
  return dir;
}

void dir_release(struct directory *dir) {
  /* Initialization */
  int i = 0;
  if (!dir) {
    return;
  }
  /* Release all the subordiniates */
  for (i = 0; i < dir->size; i++) {
    node_release(dir->subordinates[i]);
  }
  /* Release the resources */
  /* Check if base has already been released. Prevent circular call. */
  if (dir->base) {
    dir->base->inner.dir = NULL;
    node_release(dir->base);
  }
  /* Release data and self. */
  free(dir->subordinates);
  free(dir);
}

struct node *dir_find_node(const struct directory *dir, const char *name) {
  /* handle the NULL pointer */ 
  if (dir == NULL || name == NULL)
    return NULL;
  /* traverse the subordinates search for the targeted file / directory */
  for (int i = 0; i < dir->size; i++)
    if(strcmp(dir->subordinates[i]->name, name) == 0)
      return dir->subordinates[i];
  /* requested file not exist */
  return NULL;
}

bool dir_add_file(struct directory *dir, int type, char *name) {
  /* handle the NULL pointer */
  if (dir == NULL || name == NULL)
    return false;
  /* check if a dir / file called `name` already exists */
  for (int i = 0; i < dir->size; i++)
    if(strcmp(dir->subordinates[i]->name, name) == 0)
      return false;
  /* enlarge the capacity if size == capacity */
  if (dir->size == dir->capacity - 1) {
    struct node **tmp = (struct node **)realloc(dir->subordinates, 2*dir->capacity);
    dir->subordinates = tmp;
    dir->capacity *= 2;
  } 
  /* create the node for the new file */
  struct file *newfile = file_new(type, name);
  if (newfile == NULL) return false;
  /* include the new file */
  dir->subordinates[dir->size] = newfile->base;
  (dir->size) += 1;
  return true;
}

bool dir_add_subdir(struct directory *dir, char *name) {
   /* handle the NULL pointer */
  if (dir == NULL || name == NULL)
    return false;
  /* check if a dir / file called `name` already exists */
  for (int i = 0; i < dir->size; i++)
    if(strcmp(dir->subordinates[i]->name, name) == 0)
      return false;
  /* enlarge the capacity if size == capacity */
  if (dir->size == dir->capacity - 1) {
    struct node **tmp = (struct node **)realloc(dir->subordinates, 2*dir->capacity);
    dir->subordinates = tmp;
    dir->capacity *= 2;
  }
  /* create the node for the new file */
  struct directory *newdir = dir_new(name);
  if (newdir == NULL) return false;
  newdir->parent = dir;
  /* include the new file */
  dir->subordinates[dir->size] = newdir->base;
  dir->size += 1;
  /* success */
  return true;
}

bool dir_delete(struct directory *dir, const char *name) {
  /* handle the NULL pointer */
  if (dir == NULL || name == NULL)
    return false;
  for (int i = 0; i != dir->size; i++) {
    /* searching for the target file */
    if (strcmp(dir->subordinates[i]->name, name) == 0) {
      node_release(dir->subordinates[i]);
      dir->size -= 1;    
      /* release the target file */
      for (int j = i; j != dir->size; j++)
        dir->subordinates[j] = dir->subordinates[j+1];
      /* dir->subordinates[dir->size] = NULL; ? */
      /* modify the new subordinates array */
      return true;
    }
  }
  /* else: requested file not found */
  return false;
}
