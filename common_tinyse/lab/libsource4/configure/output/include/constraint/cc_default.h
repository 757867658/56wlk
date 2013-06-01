/***************************************************************************
 * 
 * 
 **************************************************************************/



/**
 * @file cc_default.h
 * @brief 
 *  
 **/


#ifndef  __CC_DEFAULT_H_
#define  __CC_DEFAULT_H_

#include "Constraint.h"
#include "ConstraintFunction.h"
#include "idl_conf_if.h"

namespace comcfg{
	class CFdefault{
		public:
			static int cons_array(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_default(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_length(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_enum(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_regexp(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_range(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_ip(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
			static int cons_do_nothing(const confIDL::cons_func_t&, const confIDL::var_t &, ConfigGroup *, ConfigUnit *);
	};
}
















#endif  //__CC_DEFAULT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
