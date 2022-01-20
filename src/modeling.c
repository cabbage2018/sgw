#ifndef _MSC_VER
#include <stdint.h>
#include <unistd.h>
#else
#include "stdint.h"
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "list.h"
#include "lol.h"
#include "modeling.h"

// from static file
int buildTable(struct list* List, void *data, iterator itBefore, void(*assign)(void*, const void*)) {}

// resp is Json String
int resp2List(const char *key, char *instr, struct list** outlet, int(*assignresp)(const void*, const void*)) {

}

// look up the Json table to parse resp
int lookupVars(const struct list* outlet) {}

/*

  physical addr
    ip
    port
    subordinator

*/ 

struct stat {
  char *addr;
  char *buffer;

  union val{ 
    bool b;
    int i;
    };

  char*unit;
  enum enumAsString {};

  int qc;
  char desc[16];
};

int presentVars() {
  /*
    addr:
      fc:register:count:offset:bits
    
    datatype
      bool
      int
      long
      float
      double
      string[n]

    buffer
      byte[]

    value
      union:
        bool
        int
        long
        float
        double


      unit
        char[]

      enumAsString
        char[][]

      qc

      desc
  */
  //

}