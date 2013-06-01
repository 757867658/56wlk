/***************************************************************************
 * 
 * 
 **************************************************************************/



/**
 * @file IVar.h
 * @brief 
 *  
 **/
#ifndef  __BSL_VAR_IVAR_H__
#define  __BSL_VAR_IVAR_H__

#include "bsl/containers/string/bsl_string.h"
#include "bsl/exception/bsl_exception.h"
#include "bsl/ResourcePool.h"
#include "bsl/var/ArrayIterator.h"
#include "bsl/var/DictIterator.h"

namespace bsl{
namespace var{

    /**
    * @brief ΪIVar֧�������Ͷ��������ݶ�����Ľṹ��
    *  
    * �൱��ͷָ���볤�ȵļ򵥼��ϣ�����ʱ�������data��ָ�ڴ档
    */
    struct raw_t{
        /**
        * @brief ����������ͷָ��
        *  
        *  
        */
        const void  *data;
        /**
        * @brief ���������ݳ��ȡ�
        *  
        *  
        */
        size_t      length;

        /**
         * @brief Ĭ�Ϲ��캯��
         *
         * @see 
        **/
        raw_t()
            :data(NULL), length(0) {}
        /**
         * @brief ���õĹ��캯��
         *
         * @param [in] data_   : const void*
         * @param [in] len   : size_t
         * @see 
        **/
        raw_t( const void *data_, size_t len)
            :data(data_), length(len) {}
    };

    //forward declaration
    class IVar{
    public:
        /**
        * @brief �ַ�������
        *  
        *  
        */
        typedef bsl::string         string_type;

        /**
        * @brief �ֶ�������
        *  
        *  
        */
        typedef bsl::string         field_type;

        /**
        * @brief �����Ͷ�������������
        *  
        *  
        */
        typedef raw_t               raw_type;

        /**
        * @brief �������������
        *  
        *  
        */
        typedef ArrayIterator       array_iterator;

        /**
        * @brief ֻ���������������
        *  
        *  
        */
        typedef ArrayConstIterator  array_const_iterator;

        /**
        * @brief �ֵ����������
        *  
        *  
        */
        typedef DictIterator        dict_iterator;

        /**
        * @brief ֻ���ֵ����������
        *  
        *  
        */
        typedef DictConstIterator   dict_const_iterator;

        /**
        * @brief ��������
        *  
        *  
        */
        typedef unsigned short mask_type;

        // constant definition
        static const mask_type IS_BOOL = 1 << 0;          /**< �Ƿ񲼶�����       */
        static const mask_type IS_NUMBER = 1 << 1;        /**< �Ƿ���ֵ����       */
        static const mask_type IS_STRING = 1 << 2;        /**< �Ƿ��ַ�������       */
        static const mask_type IS_RAW = 1 << 3;        /**< �Ƿ����������       */
        static const mask_type IS_ARRAY = 1 << 4;        /**< �Ƿ���������       */
        static const mask_type IS_DICT = 1 << 5;        /**< �Ƿ��ֵ�����       */
        static const mask_type IS_CALLABLE = 1 << 6;        /**< �Ƿ�ɵ�������       */
        static const mask_type IS_OTHER = 1 << 7;        /**< �Ƿ���������       */
        static const mask_type IS_REF = 1 << 8;        /**< �Ƿ���������       */
        static const mask_type IS_MUTABLE = 1 << 9;        /**< �Ƿ�ɱ�����       */
        static const mask_type IS_FLOATING = 1 << 10;        /**< �Ƿ񸡵�����       */
        static const mask_type IS_SIGNED = 1 << 11;        /**< �Ƿ��з�������       */
        static const mask_type IS_ONE_BYTE = 1 << 12;        /**< �Ƿ��ֽ�����       */
        static const mask_type IS_TWO_BYTE = 1 << 13;        /**< �Ƿ�˫�ֽ�����       */
        static const mask_type IS_FOUR_BYTE = 1 << 14;        /**< �Ƿ����ֽ�����       */
        static const mask_type IS_EIGHT_BYTE = 1 << 15;        /**< �Ƿ���ֽ�����       */
        static const mask_type NONE_MASK = 0;        /**< ������ */
        static const mask_type ALL_MASK = ~0;        /**< ������ */



        //methods for all
        /**
         * @brief ��������
         *
         * @see 
        **/
        virtual ~IVar(){ } 

        /**
         * @brief ��ֵ�����
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( IVar& ) = 0;

        /**
         * @brief ��ӡ��Ҫ���ڲ�״̬��������IVar�����״̬
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         * �÷��������ڵ�������٣�������Ӧ�����ױ�����ʶ�����ʽ���ܾ����仯����Ӧ�������ݽ��м�ء�
         * 
         * ��ѡ��verbose_level������ʾ�ݹ���ȡ�0��ʾ���ݹ���IVar����ʵ����Ӧ��֤�ú����㷨���Ӷ�ΪO(1)��1��ʾ�ݹ�����ֱ����IVar����ʵ����Ӧ��֤�ú����㷨���Ӷ�ΪO(size())�������ơ�
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type dump(size_t verbose_level=0) const = 0;

        /**
         * @brief ��պ���
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  void 
         * @retval   
         * @see 
        **/
        virtual void clear() = 0;

        /**
         * @brief ת��Ϊ�ַ�����
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         * ��BSL 1.0.5�󣬸ú���ֻ����ת��Ϊ�ַ���������/����Ӧʹ��dump()
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type to_string() const = 0;

        /**
         * @brief ����ʵ�����͵��ַ�����ʾ
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type get_type() const = 0; 

        /**
         * @brief ����ʵ�����͵���������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         * Ŀǰ����ֵ����Ŀǰ��unsigned short���Ժ���ܻ�ı䣬���ᱣ����unsigned short����
         *
         * @return  mask_type 
         * @retval   
         * @see 
        **/
        virtual mask_type get_mask() const = 0;

        /**
         * @brief ����һ��var���
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& clone(bsl::ResourcePool& ) const = 0;

        //methods for all, test methods
        /**
         * @brief �����Ƿ�Ϊ������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_null() const {
            return get_mask() & 0xFF == 0;
        }

        /**
         * @brief �����Ƿ�Ϊ��������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_ref() const {
            return get_mask() & IS_REF;
        }

        /**
         * @brief �����Ƿ�Ϊ��������
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_bool() const {
            return get_mask() & IS_BOOL;
        }
        /**
         * @brief �����Ƿ�Ϊ��ֵ����
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_number() const {
            return get_mask() & IS_NUMBER;
        }

        /**
         * @brief �����Ƿ�Ϊ8λ�з�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_int8() const {
            return check_mask( get_mask(), IS_NUMBER|IS_ONE_BYTE|IS_SIGNED, IS_FLOATING );
        }
        
        /**
         * @brief �����Ƿ�Ϊ8λ�޷�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_uint8() const {
            return check_mask( get_mask(), IS_NUMBER|IS_ONE_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ16λ�з�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_int16() const {
            return check_mask( get_mask(), IS_NUMBER|IS_TWO_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ16λ�޷�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_uint16() const {
            return check_mask( get_mask(), IS_NUMBER|IS_TWO_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ32λ��������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_int32() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ32λ�޷�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_uint32() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ64λ��������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_int64() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE|IS_SIGNED, IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ64λ�޷�����������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_uint64() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE, IS_SIGNED|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ�����ȸ���������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_float() const {
            return check_mask( get_mask(), IS_NUMBER|IS_FOUR_BYTE|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ˫���ȸ���������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_double() const {
            return check_mask( get_mask(), IS_NUMBER|IS_EIGHT_BYTE|IS_FLOATING );
        }

        /**
         * @brief �����Ƿ�Ϊ�ַ�������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_string() const {
            return get_mask() & IS_STRING;
        }

        /**
         * @brief �����Ƿ�Ϊ��������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_array() const {
            return get_mask() & IS_ARRAY;
        }

        /**
         * @brief �����Ƿ�Ϊ�ֵ�����
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_dict() const {
            return get_mask() & IS_DICT;
        }

        /**
         * @brief �����Ƿ�Ϊ�ɵ�������
         *
         * ����IVarʵ���඼����֧�ָ÷�����
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_callable() const {
            return get_mask() & IS_CALLABLE;
        }

        /**
         * @brief �����Ƿ�Ϊ�����Ͷ�������������
         *
         * ����IVarʵ���඼����֧�ָ÷���
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool is_raw() const {
            return get_mask() & IS_RAW;
        }

        /**
         * @brief ����is_bool()����true��IVarʵ���඼����֧�ָ÷���
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool to_bool() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }
        //methods for value

        /**
         * @brief ת��Ϊλ�з�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  signed char 
         * @retval   
         * @see 
        **/
        virtual signed char to_int8() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊλ�޷�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  unsigned char 
         * @retval   
         * @see 
        **/
        virtual unsigned char to_uint8() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊλ�з�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  signed short 
         * @retval   
         * @see 
        **/
        virtual signed short to_int16() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊλ�޷�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  unsigned short 
         * @retval   
         * @see 
        **/
        virtual unsigned short to_uint16() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊλ�з�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  signed int 
         * @retval   
         * @see 
        **/
        virtual signed int to_int32() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊλ�޷�������
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  unsigned int 
         * @retval   
         * @see 
        **/
        virtual unsigned int to_uint32() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊlong long
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  signed long long 
         * @retval   
         * @see 
        **/
        virtual signed long long to_int64() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊunsigned long long
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  unsigned long long 
         * @retval   
         * @see 
        **/
        virtual unsigned long long to_uint64() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊfloat����
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  float 
         * @retval   
         * @see 
        **/
        virtual float to_float() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊ˫���ȸ�����
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  double 
         * @retval   
         * @see 
        **/
        virtual double to_double() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ת��Ϊ�����Ͷ�������������
         *
         * ����is_raw()����true��ʵ���඼����֧�ָ÷���
         *
         * @return  raw_type 
         * @retval   
         * @see 
        **/
        virtual raw_type to_raw() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����is_bool()����true��IVarʵ���඼����֧�ָ÷���
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( bool value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��8λ�з���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( signed char value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��8λ�޷���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( unsigned char value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         * @brief ��16λ�з���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( signed short value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��16λ�޷���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( unsigned short value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         * @brief ��32λ�з���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( signed int value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��32λ�޷���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( unsigned int value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }


        /**
         * @brief ��64λ�з���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( signed long long value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��64λ�޷���������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( unsigned long long value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief �õ����ȸ�������ֵ
         *
         * @param [in] value_   : float
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( float value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��˫���ȸ�������ֵ
         *
         * ����is_number()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( double value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ��C����ַ�����ֵ
         *
         * ����is_string()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( const char * value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ���ַ�����ֵ
         *
         * ����is_string()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��ת��������ֵ������׳�bsl::UnderflowException / bsl::OverflowException
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( const string_type& value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from["<<value_<<"]";
        }

        /**
         * @brief ����C����ַ�����ʾ
         *
         * ����is_string()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  const char* 
         * @retval   
         * @see 
        **/
        virtual const char * c_str() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����C����ַ�����ʾ�ĳ��ȣ�������ĩβ��'\0'
         *
         * ����is_string()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  size_t 
         * @retval   
         * @see 
        **/
        virtual size_t c_str_len() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ģ�����(unboxing)����
         *
         * �ú��������Ը�д
         *
         * @return  template<typename T>             T 
         * @retval   
         * @see 
        **/
        template<typename T>
            T to() const;

        /**
         * @brief �������Ͷ������������͸�ֵ
         *
         * ����is_raw()����true��ʵ���඼����ʵ�ָ÷���
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  IVar&  operator 
         * @retval   
         * @see 
        **/
        virtual IVar& operator = ( const raw_type& value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] assign from raw[data:"<<value_.data<<", length:"<<value_.length<<"]";
        }

        //methods for array and dict
        /**
         * @brief ���ؼ��ϵ�Ԫ�ظ�����
         *
         * is_array()����true����is_dict()����true��ʵ�������֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  size_t 
         * @retval   
         * @see 
        **/
        virtual size_t size() const{
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        //methods for array
        /**
         * @brief ��ȡ�±�index����IVar��������ö���
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���±�Խ�磬����bsl::var::Null::null;
         * һ��������̽�Ի�ȡ
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& get( size_t idx ) {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         * @brief ��ȡ�±�index����IVar��������ö���
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���±�Խ�磬����Ĭ��ֵ
         * һ��������̽�Ի�ȡ������������Ĭ��ֵҪ��
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& get( size_t idx, IVar& default_value ) {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         * @brief ��ȡ�±�index����IVar��������ö���
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��index >= size()������bsl::var::Null::null;
         * һ��������̽�Ի�ȡ
         *
         * @return  const IVar& 
         * @retval   
         * @see 
        **/
        virtual const IVar& get( size_t idx ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         * @brief ��ȡ�±�index����IVar��������ö���
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���±�Խ�磬����Ĭ��ֵ
         * һ��������̽�Ի�ȡ������������Ĭ��ֵҪ��
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual const IVar& get( size_t idx, const IVar& default_value ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         * @brief �����±��
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��index >= size()����������Զ�������size() + 1
         *
         * @return  void
         * @retval   
         * @see 
        **/
        virtual void set( size_t idx, IVar& value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"] value["<<value_.dump(0)<<"]";
        }

        /**
         * @brief ɾ���±��
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���󶨲����ڣ�����false�����򣬷���true��
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool del( size_t idx ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         * @brief ����ֻ����ʼ���������
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  array_const_iterator 
         * @retval   
         * @see 
        **/
        virtual array_const_iterator array_begin() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ������ʼ���������
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  array_iterator 
         * @retval   
         * @see 
        **/
        virtual array_iterator array_begin() {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����ֻ��ĩβ���������
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  array_const_iterator 
         * @retval   
         * @see 
        **/
        virtual array_const_iterator array_end() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ��������ĩβ������
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  array_iterator 
         * @retval   
         * @see 
        **/
        virtual array_iterator array_end() {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����/�����±��
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��index >= size()���׳�bsl::OutOfBoundException�쳣
         * һ������"ȷ���Ի�ȡ"����ȡ����ֱ�����쳣��
         * ������֧�֣���index < 0���ȼ���size() + index
         *
         * @param [in] index   : int
         * @return  const IVar& [] 
         * @retval   
         * @see 
        **/
        virtual const IVar& operator []( int idx ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        /**
         * @brief ����/�����±��
         *
         * ����is_array()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ��index >= size()��������Զ�������index + 1
         * һ�����������±�󶨻���ȷ���Ի�ȡ����ȡ�����ᵼ�������Զ�������
         * ������֧�֣���index < 0���ȼ���size() + index
         *
         * @param [in] index   : int
         * @return  IVar& [] 
         * @retval   
         * @see 
        **/
        virtual IVar& operator []( int idx ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] index["<<idx<<"]";
        }

        //methods for dict
        /**
         * @brief �����ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���ڲ����ڵ��ֶ����󶨣�����bsl::var::Null::null��
         * һ��������̽�Ի�ȡ��
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& get( const field_type& name ) {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         * @brief �����ֶ����󶨣����ֶ��������ڣ�����Ĭ��ֵ
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���ڲ����ڵ��ֶ����󶨣�����ָ����Ĭ��ֵ��
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& get( const field_type& name, IVar& default_value ) {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         * @brief �����ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * �������ڸ��ֶ����󶨣�����bsl::var::Null::null;
         * һ��������̽�Ի�ȡ
         *
         * @return  const IVar& 
         * @retval   
         * @see 
        **/
        virtual const IVar& get( const field_type& name ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         * @brief �����ֶ����󶨣����ֶ��������ڣ�����Ĭ��ֵ
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * �������ڸ��ֶ����󶨣�����ָ����Ĭ��ֵ
         * һ��������̽�Ի�ȡ
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual const IVar& get( const field_type& name, const IVar& default_value ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] default["<<default_value.dump(0)<<"]";
        }

        /**
         * @brief �����ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual void set( const field_type& name, IVar& value_ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"] value["<<value_.dump(0)<<"]";
        }

        /**
         * @brief ɾ���ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���󶨲����ڣ�����false�����򣬷���true��
         *
         * @return  bool 
         * @retval   
         * @see 
        **/
        virtual bool del( const field_type& name ) {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         * @brief ��ȡ/�����ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���󶨲����ڣ��׳�bsl::KeyNotFoundException�쳣
         * һ������"ȷ���Ի�ȡ"����ȡ����ֱ�����쳣��
         *
         * @param [in] name   : const field_type&
         * @return  const IVar& [] 
         * @retval   
         * @see 
        **/
        virtual const IVar& operator []( const field_type& name ) const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         * @brief ��ȡ/�����ֶ�����
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * ���󶨲����ڣ��½��󶨡�
         * һ�����ڰ󶨻���ȷ���Ի�ȡ����ȡ�����ᵼ���½��󶨣�
         *
         * @param [in] name   : const field_type&
         * @return  IVar& [] 
         * @retval   
         * @see 
        **/
        virtual IVar& operator []( const field_type& name ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] name["<<name<<"]";
        }

        /**
         * @brief ����ֻ����ʼ�ֵ������
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  dict_const_iterator 
         * @retval   
         * @see 
        **/
        virtual dict_const_iterator dict_begin() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ������ʼ�ֵ������
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  dict_iterator 
         * @retval   
         * @see 
        **/
        virtual dict_iterator dict_begin() {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����ֻ��ĩβ�ֵ������
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  dict_const_iterator 
         * @retval   
         * @see 
        **/
        virtual dict_const_iterator dict_end() const {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����ĩβ�ֵ������
         *
         * ����is_dict()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         *
         * @return  dict_iterator 
         * @retval   
         * @see 
        **/
        virtual dict_iterator dict_end() {
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"]";
        }

        /**
         * @brief ����IVar���� 
         *
         * ����is_callable()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * paramsӦ����һ��is_array()����true��IVar����
         * ע������һ�������Է�����bsl::ResourcePool�п��ܱ�bsl::var::IFactory����
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& operator()(IVar& params, bsl::ResourcePool& /*rp*/ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] params["<<params.dump(1)<<"]";
        }

        /**
         * @brief ����IVar����
         *
         * ����is_callable()����true��IVarʵ���඼����֧�ָ÷�����
         * ��ʵ���಻֧�ָò������׳�bsl::InvalidOperationException�쳣
         * self����������IVar����
         * paramsӦ����һ��is_array()����true��IVar����
         * ע������һ�������Է�����bsl::ResourcePool�п��ܱ�bsl::var::IFactory����
         *
         * @return  IVar& 
         * @retval   
         * @see 
        **/
        virtual IVar& operator()(IVar& self, IVar& params, bsl::ResourcePool& /*rp*/ ){
            throw bsl::InvalidOperationException()<<BSL_EARG<<"type["<<typeid(*this).name()<<"] this["<<dump(0)<<"] self["<<self.dump(0)<<"] params["<<params.dump(1)<<"]";
        }

        /**
         * @brief ���ߺ���������mask�Ƿ�����set_bits�е�����λ������û����unset_bits����һλ
         *
         * @param [in] mask   : mask_type
         * @param [in] set_bits   : mask_type
         * @param [in] unset_bits   : mask_type
         * @return  inline bool 
         * @retval   
         * @see 
        **/
        static inline bool check_mask( mask_type mask, mask_type set_bits, mask_type unset_bits = 0 ){
            return ((mask & set_bits) == set_bits) && ((mask & unset_bits) == 0);
        }
    };

    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline bool 
     * @retval   
     * @see 
    **/
    template<>
        inline bool IVar::to<bool>() const{
            return this->to_bool();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline signed char IVar::to<signed 
     * @retval   
     * @see 
    **/
    template<>
        inline signed char IVar::to<signed char>() const{
            return this->to_int8();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline unsigned char IVar::to<unsigned 
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned char IVar::to<unsigned char>() const{
            return this->to_uint8();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline signed short IVar::to<signed 
     * @retval   
     * @see 
    **/
    template<>
        inline signed short IVar::to<signed short>() const{
            return this->to_int16();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline unsigned short IVar::to<unsigned 
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned short IVar::to<unsigned short>() const{
            return this->to_uint16();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline signed int IVar::to<signed 
     * @retval   
     * @see 
    **/
    template<>
        inline signed int IVar::to<signed int>() const{
            return this->to_int32();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline unsigned int IVar::to<unsigned 
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned int IVar::to<unsigned int>() const{
            return this->to_uint32();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline signed long long IVar::to<signed long 
     * @retval   
     * @see 
    **/
    template<>
        inline signed long long IVar::to<signed long long>() const{
            return this->to_int64();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline unsigned long long IVar::to<unsigned long 
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned long long IVar::to<unsigned long long>() const{
            return this->to_uint64();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline float 
     * @retval   
     * @see 
    **/
    template<>
        inline float IVar::to<float>() const{
            return this->to_float();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline double 
     * @retval   
     * @see 
    **/
    template<>
        inline double IVar::to<double>() const{
            return this->to_double();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline bsl::string 
     * @retval   
     * @see 
    **/
    template<>
        inline bsl::string IVar::to<bsl::string>() const{
            return this->to_string();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline raw_t 
     * @retval   
     * @see 
    **/
    template<>
        inline raw_t IVar::to<raw_t>() const{
            return this->to_raw();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return  template<>         inline const char*  IVar::to<const 
     * @retval   
     * @see 
    **/
    template<>
        inline const char * IVar::to<const char *>() const{
            return this->c_str();
        }

#if __WORDSIZE == 64 
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return signed long
     * @retval   
     * @see 
    **/
    template<>
        inline signed long IVar::to<signed long>() const{
            return this->to_int64();
        }
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return unsigned long
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned long IVar::to<unsigned long>() const{
            return this->to_uint64();
        }
#else
    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return signed long
     * @retval   
     * @see 
    **/
    template<>
        inline signed long IVar::to<signed long>() const{
            return this->to_int32();
        }

    /**
     * @brief IVar::to<T>���ػ�ʵ�֣��÷������ܱ���д��
     *
     * @return unsigned long
     * @retval   
     * @see 
    **/
    template<>
        inline unsigned long IVar::to<unsigned long>() const{
            return this->to_uint32();
        }
#endif

}}   //  namespace bsl::var

#endif  //__BSL_VAR_IVAR_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
