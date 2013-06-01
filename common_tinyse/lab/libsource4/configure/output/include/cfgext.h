/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file cfgext.h
 * @brief 
 *  
 **/


#ifndef  __CFGEXT_H_
#define  __CFGEXT_H_

#include "utils/cc_utils.h"

namespace comcfg{
/**
 * @brief ����һ��Լ���ļ����Զ�����һ���������ļ�
 *        ����ʹ�ã�����RD����һ�������ļ�ģ��
 *
 * @param [in] range_file   : const char* Լ���ļ���ַ
 * @return  bsl::string ���������ļ����ı����ݡ�ֱ��write��ĳ���ļ����ɡ�
 *
 * 			������ʱ�׳�ConfigException�쳣
 *
 * @retval   
 * @see 
**/
bsl::string autoConfigGen(const char * range_file);









}//namespace comcfg






#endif  //__CFGEXT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
