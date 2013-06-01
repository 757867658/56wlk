/**
 * @file iterator.h
 * @brief
 *
 **/

#ifndef  _COMPACK_ITERATOR_H_
#define  _COMPACK_ITERATOR_H_

#include <cstdlib>
#include <cerrno>
#include <bsl/check_cast.h>
#include <compack/protocol.h>
#include <compack/exception.h>


#undef compack_likely
#undef compack_unlikely
#if 0
#define compack_unlikely(x) x
#define compack_likely(x) x
#else
#define compack_unlikely(x) __builtin_expect((x),0)
#define compack_likely(x) __builtin_expect((x),1)
#endif

namespace compack {

template<typename T>
class Reader;

class ObjectIterator;

class Iterator {
public:
    Iterator() {
    }
    explicit Iterator(const ObjectIterator& it);
    /**
     * @brief
     * �����ֶ�ֵ���ڵ��ڴ�
     * @return  const void*
     * @retval
     * @see
     * @note
    **/
    inline const void* value() const {
        return _value;
    }
    /**
     * @brief
     * �����ֶ�����
     * @return  unsigned char
     * @retval
     * @see
     * @note
    **/
    inline unsigned char getType() const {
        return _type;
    }
    /**
     * @brief
     * �Ƿ�����
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isNumber() const {
        switch(_type) {
            case protocol::INT_8:
            case protocol::INT_16:
            case protocol::INT_32:
            case protocol::INT_64:
            case protocol::UINT_8:
            case protocol::UINT_16:
            case protocol::UINT_32:
            case protocol::UINT_64:
            case protocol::FLOAT:
            case protocol::DOUBLE:
                return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ����ַ���
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isString() const {
        switch(_type) {
            case protocol::STRING | protocol::SHORT_ITEM:
            case protocol::STRING:
                return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ��Ƕ���������
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isBinary() const {
        switch(_type) {
            case protocol::BINARY | protocol::SHORT_ITEM:
            case protocol::BINARY:
                return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ���object
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isObject() const {
        if(_type == protocol::OBJECT) {
            return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ���ͬ����������
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isObjectIsoArray() const {
        if(_type == protocol::OBJECTISOARRAY) {
            return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ���array
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isArray() const {
        if(_type == protocol::ARRAY || _type == protocol::ISOARRAY) {
            return true;
        }
        return false;
    }
    template<typename T>
    inline bool isIsoArray() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(_type == protocol::ISOARRAY && _value_length >= 1 && *(const char*)_value == protocol::Mcpack_type_id_traits<T>::TYPE_ID) {
            return true;
        }
        return false;
    }
    template<typename T>
    inline const T* getAsIsoArray() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(compack_unlikely(_type != protocol::ISOARRAY || _value_length == 0 || *(const char*)_value != protocol::Mcpack_type_id_traits<T>::TYPE_ID)) {
            throw_mcpack_type("not a isoarray");
        }
        return reinterpret_cast<const T*>((const char*)_value + 1);
    }
    inline unsigned int getIsoArrayLength() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(compack_unlikely(_type != protocol::ISOARRAY || _value_length == 0)) {
            throw_mcpack_type("not a isoarray");
        }
        return (_value_length - 1) / (*(const char*)_value & protocol::FIXED_ITEM);
    }
    /**
     * @brief
     * �Ƿ�Ϊnull
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isNull() const {
        if(_type == protocol::MCPACK_NULL) {
            return true;
        }
        return false;
    }
    /**
     * @brief
     * �Ƿ���booleanֵ
     * @return  bool
     * @retval
     * @see
     * @note
    **/
    inline bool isBool() const {
        if(_type == protocol::BOOL) {
            return true;
        }
        return false;
    }
    /**
     * @brief
     * ֵ�ĳ���(�ֽ�)
     * @return  unsigned int
     * @retval
     * @see
     * @note
    **/
    inline unsigned int length() const {
        return _value_length;
    }
    /**
     * @brief
     * ���ַ�����ȡ
     * @return  const char*
     * @retval  ���Ͳ�ƥ��ʱ���쳣
     * @see
     * @note
    **/
    inline const char* getAsString() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(_type == protocol::STRING || _type == (protocol::STRING | protocol::SHORT_ITEM)) {
            if(compack_unlikely(reinterpret_cast<const char*>(_value)[_value_length - 1] != 0)) {
                throw_mcpack_data("bad data");
            }
            return reinterpret_cast<const char*>(_value);
        } else {
            throw_mcpack_type("not a string");
            //__builtin_unreachable ();
            return 0;  //this should never reach
        }
    }
    inline const void* getAsBinary() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(compack_unlikely(_type == protocol::BINARY || _type == (protocol::BINARY | protocol::SHORT_ITEM))) {
            return reinterpret_cast<const void*>(_value);
        } else {
            throw_mcpack_type("not a binary");
            //__builtin_unreachable ();
            return 0;  //this should never reach
        }
    }
    /**
     * @brief
     * ��bool��ȡ
     * @return  bool
     * @retval  ���Ͳ�ƥ��ʱ���쳣
     * @see
     * @note
    **/
    inline bool getAsBool() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(compack_unlikely(_type == protocol::BOOL)) {
            return *reinterpret_cast<const bool*>(_value) == true;
        } else {
            throw_mcpack_type("not a number");
        }
        //__builtin_unreachable ();
        return false;  //this should never reach
    }
    template <typename T>
    inline T getAsNum() const {
        if(compack_unlikely(_value==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(_type == protocol::Mcpack_type_id_traits<T>::TYPE_ID) {
            return *(T*)_value;
        }
        switch(_type) {
            case protocol::INT_8:
                return bsl::check_cast<T, protocol::mc_int8_t>(
                           *reinterpret_cast<const protocol::mc_int8_t*>(_value));
            case protocol::INT_16:
                return bsl::check_cast<T, protocol::mc_int16_t>(
                           *reinterpret_cast<const protocol::mc_int16_t*>(_value));
            case protocol::INT_32:
                return bsl::check_cast<T, protocol::mc_int32_t>(
                           *reinterpret_cast<const protocol::mc_int32_t*>(_value));
            case protocol::INT_64:
                return bsl::check_cast<T, protocol::mc_int64_t>(
                           *reinterpret_cast<const protocol::mc_int64_t*>(_value));
            case protocol::UINT_8:
                return bsl::check_cast<T, protocol::mc_uint8_t>(
                           *reinterpret_cast<const protocol::mc_uint8_t*>(_value));
            case protocol::UINT_16:
                return bsl::check_cast<T, protocol::mc_uint16_t>(
                           *reinterpret_cast<const protocol::mc_uint16_t*>(_value));
            case protocol::UINT_32:
                return bsl::check_cast<T, protocol::mc_uint32_t>(
                           *reinterpret_cast<const protocol::mc_uint32_t*>(_value));
            case protocol::UINT_64:
                return bsl::check_cast<T, protocol::mc_uint64_t>(
                           *reinterpret_cast<const protocol::mc_uint64_t*>(_value));
            case protocol::FLOAT:
                return bsl::check_cast<T, double>(*reinterpret_cast<const float*>(_value));
            case protocol::DOUBLE:
                return bsl::check_cast<T, double>(*reinterpret_cast<const double*>(_value));
            case protocol::STRING:
            case protocol::STRING | protocol::SHORT_ITEM:
                return bsl::check_cast<T, const char*>(
                           reinterpret_cast<const char*>(_value));
        }
        throw_mcpack_type("not a number");
        //__builtin_unreachable ();
        return T(); //this shoud never reach
    }

    /**
     * @brief ���ص�ǰ��ȡ�Ľ���.
     * @return unsigned int
     */
    unsigned int offset() const {
        return _offset;
    }

protected:
    unsigned int _value_length;
    unsigned char _type;
    const void* _value;
    unsigned int _offset;
    template<typename BUFFER> friend class Reader;

};

class ObjectIterator : public Iterator {
public:
    /**
     * @brief
     * �����ֶε�����
     * @return  const char*
     * @retval
     * @see
     * @note
    **/
    inline const char* getName() const {
        if(compack_unlikely(_name==0x0)) {
            throw_mcpack_data("bad data");
        }

        if(compack_unlikely(reinterpret_cast<const char*>(_name)[_name_length - 1] != 0)) {
            throw_mcpack_data("bad data");
        }
        return _name;
    }
    /**
     * @brief
     * �����ֶ����ĳ���
     * @return  unsigned int
     * @retval
     * @see
     * @note
    **/
    inline unsigned int getNameLength() const {
        return _name_length - 1;
    }
protected:
    const char* _name;
    unsigned char _name_length;
    template<typename BUFFER> friend class Reader;
};

inline Iterator::Iterator(const ObjectIterator& it) :
    _value_length(it._value_length),
    _type(it._type),
    _value(it._value),
    _offset(it._offset) {
    //����_value���ܹ�����������ػ�..
    //��ΪReader����Ԫ,�ܹ���д_value�ֶ�...
    //Reader����û�ж�ȡ_value�ֶεĲٲ���,ֻ�����_value����.
    //�������еĲ������ܹ���accessor�ķ�����������ػ�..
}

}












#endif  //__ITERATOR_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100 */
