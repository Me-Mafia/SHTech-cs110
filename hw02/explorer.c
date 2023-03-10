#include "explorer.h"
/* include the head file */
#include "dir.h"
/* include the head file */
#include "file.h"
/* include the head file */
#include "node.h"
#include <stdlib.h>
/* include the head file */ 
#include <string.h>

struct explorer *explorer_new(void) {
  /* Allocate memory */
  struct explorer *exp = calloc(1, sizeof(struct explorer));
  /* Initialization */
  exp->cwd = exp->root = dir_new("root");
  return exp;
}

void explorer_release(struct explorer *exp) {
  /* Check for null pointer */
  if (!exp) {
    return;
  }
  /* Relase the resources */
  dir_release(exp->root);
  free(exp);
}

bool explorer_read(const struct explorer *exp, const char *name, int offset,
                   int bytes, char *buf) {
  /* Initialization */
  struct node *sub = NULL;
  /* Check for null pointer */
  if (!exp) {
    return false;
  }
  /* Check if the file exists */
  sub = dir_find_node(exp->cwd, name);
  if (!sub || sub->is_dir) {
    return false;
  }
  /* Read the file */
  return file_read(sub->inner.file, offset, bytes, buf);
}

bool explorer_write(struct explorer *exp, const char *name, int offset,
                    int bytes, const char *buf) {
  /* Initialization */
  struct node *sub = NULL;
  /* Check for null pointer */
  if (!exp) {
    return false;
  }
  /* Check if the file exisits */
  sub = dir_find_node(exp->cwd, name);
  if (!sub || sub->is_dir) {
    return false;
  }
  /* Write the file */
  return file_write(sub->inner.file, offset, bytes, buf);
}

bool explorer_create(struct explorer *exp, char *name, int type) {
  /* Check for valid arguments */
  if (!exp || dir_find_node(exp->cwd, name)) {
    return false;
  }
  /* Add file */
  return dir_add_file(exp->cwd, type, name);
}

bool explorer_mkdir(struct explorer *exp, char *name) {
  /* Check for valid arguments */
  if (!exp || dir_find_node(exp->cwd, name)) {
    return false;
  }
  /* Add subdir */
  return dir_add_subdir(exp->cwd, name);
}

bool explorer_delete(struct explorer *exp, const char *name) {
  /* Check for null pointer */
  if (!exp) {
    return false;
  }
  /* Delete the node */
  return dir_delete(exp->cwd, name);
}

bool explorer_cdpar(struct explorer *exp) {
  /* null || root directory */
  if (!exp || exp->cwd == exp->root) return false;
  exp->cwd = exp->cwd->parent;
  /* change the directiry */
  return true;
}

bool explorer_chdir(struct explorer *exp, const char *name) {
  struct node *sub = NULL;
  /* handle the null ponter */
  if (!exp || !name) return false;
  sub = dir_find_node(exp->cwd, name);
  /* not found || not a dir */
  if (!sub || !sub->is_dir) return false;
  exp->cwd = sub->inner.dir;
  /* success */
  return true;
}

bool explorer_support_filetype(struct explorer *exp, open_func callback,
                               int filetype) {
  /* invalid arguments */
  if (!exp || callback == NULL || filetype < 0 || filetype >= MAX_FT_NO)
    return false;
  /* do not support */
  if (exp->ft_open[filetype] != NULL) return false;
  /*support file*/
  exp->ft_open[filetype] = callback;
  return true;
}

bool explorer_open(const struct explorer *exp, const char *name) {
  /* create sub & file */
  struct node *sub = NULL;
  struct file *file = NULL;
  /* handle the null pointer */
  if (!exp || name == NULL)  return false;
  sub = dir_find_node(exp->cwd, name);
  /* the name is not found, the name is a dir */
  if (!sub || sub->is_dir) return false;
  file = sub->inner.file;
  /* the file type is not supported */
  if (!exp->ft_open[file->type]) return false;
  /* open the file */
  exp->ft_open[file->type](file);
  return true;
}

static void search(struct node *node, char **path, const char *name,
                   find_func callback) {
  struct directory *dir = NODE_DIR(node);
  int i = 0, pathlen = strlen(*path);
  /* construct the searching path */
  *path = realloc(*path, pathlen + strlen(node->name) + 2);
  strcat(*path, "/");
  strcat(*path, node->name);
  if (node->is_dir) {
    for (i = 0; i < dir->size; i++) 
    /* recur : for a directory */
      search(dir->subordinates[i], path, name, callback);
  } else if (strncmp(node->name, name, MAX_NAME_LEN) == 0) {
    /* the path matches : for a file */
    callback(*path, NODE_FILE(node));
  }
  /* enlarge the searching path */
  *path = realloc(*path, pathlen + 1);
  (*path)[pathlen] = '\0';
}

void explorer_search_recursive(struct explorer *exp, const char *name,
                               find_func callback) {
  char *path = NULL;
  /* create the searching directory */
  if (!exp || !name || !callback)
    return;
  /* allocate memory */
  path = calloc(1, 1);
  search(exp->cwd->base, &path, name, callback);
  /* free the allocated memory */
  free(path);
}

bool explorer_contain(struct explorer *exp, const char *name) {
  /* if the file / dir is found */
  return exp ? dir_find_node(exp->cwd, name) != NULL : false;
}
