#ifndef __COMMON_H
#define __COMMON_H

#define DEBUG
#ifdef DEBUG
  #define debug(fmt,args...)  printf (fmt ,##args)
#else
  #define debug(fmt,args...)
#endif
#endif /*__COMMON_H*/
