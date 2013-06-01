/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file utils.h
 * @brief 
 *  
 **/
#ifndef  __UTILS_H_
#define  __UTILS_H_
#include "bsl/var/IVar.h"
#include "bsl/var/assign.h"
#include "bsl/var/var_traits.h"

namespace bsl{
    namespace var{
        /**
         * @brief �ݹ��ӡIVar���ݵ��ַ���
         *
         * @param [in] var   : const IVar&              ����ӡ��IVar�����
         * @param [in] res   : IVar::string_type&       ��ӡ�����ַ���
         * @param [in] verbose_level   : size_t         �ݹ�������
         * @param [in] line_delimiter   : const char*   ���з�
         * @param [in] per_indent   : size_t            ÿ�������ֽ���
         * @param [in] total_indent   : size_t          �������ֽ���
         * @return  void 
         * @retval   
         * @see 
        **/
        void dump_to_string(const IVar& var, IVar::string_type& res, size_t verbose_level, const char *line_delimiter, size_t per_indent = 4, size_t total_indent = 0 );

        /**
         * @brief �ݹ顢������ӡIVar��STDOUT��GDB����ר��
         *
         * @param [in] var   : const IVar&
         * @param [in] verbose_level   : size_t
         * @return  void 
         * @retval   
         * @see 
        **/
        void print( const IVar& var, size_t verbose_level );
    }
}
#endif  //__UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
