#include "palm.h"

/* 
   This routine was derived from "the software known as checksum",
   a highly portable c program which illustrates five kinds of hash
   "(c) Copyright, N.M. Maclaren, 1993
    (c) Copyright, University of Cambridge, 1993"
   
   This hash is described as System V sum ("fails to detect the common
   problems of byte swapping or the removal of nulls.")
*/

Short do_sum(Char *hotsync_name)
{
  Short bufflen, i;
  Long checksum = 0;

  bufflen = StrLen(hotsync_name);
  for (i = 0; i < bufflen; ++i)
    if ((checksum += hotsync_name[i]) > 0xffff)
      checksum -= 0xffff;
  return (Short) checksum;
}
