/*
 * dbg.hpp
 *
 *  Created on: 2010/11/28
 *      Author: pino
 */

#ifndef _DBG_HPP_
#define _DBG_HPP_

#include "3069f_sci.h"

#define ISR(str) ISR_(str)
#define ISR_(str) #str

#define dbg_l() sci_write_str(SCI_NO_1,ISR(__LINE__))
#define dbg_n() sci_write_str(SCI_NO_1,__FUNCTION__)
#define dbg() dbg_n(); sci_write(SCI_NO_1, ':'); dbg_l(); sci_write_str(SCI_NO_1, "\n\r");


#endif
