/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file Function.h
 * @brief 
 *  
 **/
#ifndef  __BSL_VAR_FUNCTION_H__
#define  __BSL_VAR_FUNCTION_H__
#include "bsl/var/IVar.h"
#include "bsl/var/Ref.h"

namespace bsl{
namespace var{
    class Function: public IVar{
    public:
        typedef IVar::string_type          string_type;
        typedef IVar::field_type           field_type;
        typedef IVar& (* function_type )(IVar&, bsl::ResourcePool& );
        typedef IVar::array_iterator       array_iterator;
        typedef IVar::array_const_iterator array_const_iterator;

        //special methods
        Function( function_type func, const string_type& name )
            :IVar(), _func(func), _name(name){ } 

        Function( const Function& other )
            :IVar(other), _func(other._func), _name(other._name){ }

        virtual ~Function(){ } 

        Function& operator = ( const Function& other ){
            _func = other._func;
            _name = other._name;
            return *this;
        }

        //methods for all
        /**
         * @brief ������
         *
         * @return  void 
         * @retval   
         * @see 
         **/
        virtual void clear(){
            // pass
        }

        /**
         * @brief ��¡����
         *
         * �ú���ֻ��¡�����㣬����¡�ӽ�㣬���������ͣ���¡ָ��Ľ��
         *
         * @param [in] rp   : bsl::ResourcePool&
         * @return  Function& 
         * @retval   
         * @see 
        **/
        virtual Function& clone( bsl::ResourcePool& rp ) const {
            return rp.clone(*this);
        }

        /**
         * @brief ���������Ϣ�������ڵ��ԣ���ͨ��verbose_level���Ƶݹ����
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type dump(size_t /*verbose_level*/ = 0) const {
            return string_type("[bsl::var::Function]").append(_name);
        }

        /**
         * @brief ת��Ϊ�ַ���
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type to_string() const {
            return _name;
        }

        /**
         * @brief ��ȡ�����ַ���������var���Ͷ�֧�֣������ڵ���ʹ�ã������������жϣ�
         *
         * @return  string_type 
         * @retval   
         * @see 
        **/
        virtual string_type get_type() const {
            return "bsl::var::Function";
        }

        /**
         * @brief ��ȡ�������루����var���Ͷ�֧�֣�
         *
         * @return  IVar::mask_type 
         * @retval   
         * @see 
        **/
        virtual IVar::mask_type get_mask() const {
            return IVar::IS_CALLABLE;
        }

        virtual Function& operator = ( IVar& var ){
            Function * vfp = dynamic_cast<Function*>(&var);
            if ( vfp == NULL ){
                throw bsl::InvalidOperationException()<<BSL_EARG<<"cast from "<<var.dump();
            }else{
                _name = vfp->_name;
                _func = vfp->_func;
                return *this;
            }
            
        }

        virtual bool is_callable() const {
            return true;
        }

        //converters
        using IVar::operator =;

        virtual IVar& operator()(IVar& args, bsl::ResourcePool& _rp ){
            return (*_func)(args, _rp);
        }

        virtual IVar& operator()(IVar& /*self*/, IVar& args, bsl::ResourcePool& _rp ){
            return (*_func)(args, _rp);
        }

    private:
        function_type   _func;
        string_type     _name;
    };

}}   //namespace bsl::var
#endif  //__BSL_VAR_FUNCTION_H__

/* vim: set ts=4 sw=4 sts=4 tw=100 */
