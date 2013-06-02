/***************************************************************************
 * 
 * 
 **************************************************************************/



/**
 * @file check_cast.h
 * @brief 
 *  
 **/
#ifndef  __CHECK_CAST_H_
#define  __CHECK_CAST_H_

#include "bsl/exception/bsl_exception.h"

namespace bsl{
    /**
     * @brief check_cast<>ģ�庯��
     *
     * �ṩ��C/C++��ԭ�������ṩ�����������鹦��ģ�庯����ʹ�÷�ʽ��static_cast<>��ȫ��ͬ��
     *
     * Ŀǰ֧�������з�����ֵ���͡��޷�����ֵ���͡����������������������͵�˫��ת���Լ�C����ַ�����const char *�������ϸ����͵ĵ���ת���������Ǹ�����ת���ɸ��������͵���ֵ������⡣
     *
     * @param [in] value   : SrcType
     * @return  DestType 
     * @retval   
     * @see 
    **/
    template<typename DestType, typename SrcType>
        DestType check_cast( SrcType value );


    /**
     * @brief check_cast<>ģ�庯����DestType == SrcTypeʱ��ƫ�ػ��汾
     *
     * @param [in] value   : DestType
     * @return  DestType 
     * @retval   
     * @see 
    **/
    template<typename DestType>
        inline DestType check_cast( DestType value ){
            return value;
        }

}
#include "bsl/check_cast/check_cast_generated.h"   //generated code
#include "bsl/check_cast/check_cast_cstring.h"
#include "bsl/check_cast/check_cast_bsl_string.h"

#endif  //__CHECK_CAST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
