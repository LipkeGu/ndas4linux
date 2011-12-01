/*
 -------------------------------------------------------------------------
 Copyright (c) 2002-2005, XIMETA, Inc., IRVINE, CA, USA.
 All rights reserved.

 LICENSE TERMS

 The free distribution and use of this software in both source and binary 
 form is allowed (with or without changes) provided that:

   1. distributions of this source code include the above copyright 
      notice, this list of conditions and the following disclaimer;

   2. distributions in binary form include the above copyright
      notice, this list of conditions and the following disclaimer
      in the documentation and/or other associated materials;

   3. the copyright holder's name is not used to endorse products 
      built using this software without specific written permission. 
      
 ALTERNATIVELY, provided that this notice is retained in full, this product
 may be distributed under the terms of the GNU General Public License (GPL),
 in which case the provisions of the GPL apply INSTEAD OF those given above.
 
 DISCLAIMER

 This software is provided 'as is' with no explcit or implied warranties
 in respect of any properties, including, but not limited to, correctness 
 and fitness for purpose.
 -------------------------------------------------------------------------
*/
#ifndef _NDAS_ROKU_GUI_PTHREAD_CLASS_H
#define _NDAS_ROKU_GUI_PTHREAD_CLASS_H
#include <pthread.h>
#include "defs.h"

class PThreadClass;

extern void _r_(PThreadClass *thread);
        
class PThreadClass 
{
private:    
    pthread_t tid;
    pthread_attr_t attr;

public:    
    virtual void run() = 0;
    
    int start() 
    {
    	
        int err;
        debug("ing");
        err = pthread_attr_init(&attr);
        if ( err ) {
            debug("fail to init thread attr");
            return -1;    
        }
        debug("creating thread");
        err = pthread_create(&tid,&attr, (void*(*)(void*)) _r_,this);
        if ( err ) {
            debug("fail to create thread");
            return -1;
        }    
        debug("ed");
        return 0;
    }

    
};
#endif

