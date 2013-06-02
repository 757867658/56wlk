////===================================================================
//
// df_btree.h Pyramid / DFS / df-lib
//
//
//
// -------------------------------------------------------------------
//
// Description
//
//    declaration and implementation of dfs_btree_t (btree) template
//
// -------------------------------------------------------------------
//
// Change Log
//
//
////====================================================================


#ifndef __DFS_BTREE_INCLUDE_H_
#define __DFS_BTREE_INCLUDE_H_

//������Ϣ��ӡ�������
//#ifndef DF_BT_PRINT_DEBUG_INFO
//#define DF_BT_PRINT_DEBUG_INFO
//#endif

# if defined __x86_64__
# else
#  error "MUST COMPILED ON x86_64"
# endif

#include <unistd.h>

//#include "ul_def.h"
#include "df_common.h"
#include "df_misc.h"
#include "df_2d_ary.h"

//��ǰ�������get_mutation_counter() > _bt_get_max_cow_mutation_counter()��(���ܻ�����������)
//��ֱ���޸�B���ڵ���ܡ�ʹ��һ�����ڶ��Ľڵ㱻���ա������ڸ�Ϊ���е��޸Ķ�һֱ���������ڵ㡣
//#ifndef BT_ALLOW_SERIAL_READ_WRITE
//#define BT_ALLOW_SERIAL_READ_WRITE
//#endif
//�ú���Լ���B�����һ��uint64_t(���ڼ�¼mutation_cnt)����ͬʱҲ������robust��
//#ifndef BT_NO_BT_NODE_BASE_MUTATION_COUNTER
//#define BT_NO_BT_NODE_BASE_MUTATION_COUNTER
//#endif

//  ���ܣ����ļ�λ��filepos����дdata_len�ֽ����ݣ�Ȼ����ļ���ǰλ���ƶ�������д���λ��
//  ������file_info���ļ��Ŀ�����Ϣ�������ļ����������store��loadʱ�ļ����ݵ�λ�õȡ�
//        filepos���ļ�������д�����ݵ���ʼλ��(���ֵ)����λ����store��load����ʱΪ��׼��
//        data_len�����ݳ��ȣ��������ĳ���>=data_len
//  ���أ�0 for success, other values for error (�����д�����û�ж�д��ָ����С������)
typedef int (*dfs_write_proc_t)(
    void * file_info,
    const int64_t filepos,
    const void * buf,
    const uint64_t data_len);
typedef int (*dfs_read_proc_t)(
    void * file_info,
    const int64_t filepos,
    void * buf,
    const uint64_t data_len);

//�û���������T��Ҫ֧�����½ӿڣ�
//  ���ܣ���ø�ʵ��store�����ݳ���(�ֽ�)����ֵ���ܳ���MAX_T_STORE_SIZE
//  uint64_t get_store_size(void) const;
//
//  ���ܣ��洢��ʵ����buf
//  ���룺data_posΪ�洢���ݵ���ʼλ��
//  �����data_posΪ�洢T���µ���ʼλ��
//  ���أ�0 for success, other values for error
//  int store(char * buf, const uint32_t buf_size, uint32_t & data_pos) const;
//
//  ���ܣ���ø�ʵ��store�����ݳ���(�ֽ�)����ֵ���ܳ���MAX_T_STORE_SIZE
//  ���룺data_posΪ�洢���ݵ���ʼλ��
//  �����data_posΪ�洢T���µ���ʼλ��
//  ���أ�0 for success, other values for error
//  int load(char * buf, const uint32_t buf_size, uint32_t & data_pos);
//


class dfs_file_rw_info_t
{
public:
    int fd;
    int reserved;
    int64_t orig_offset; //typically zero
};


//int dfs_write_proc(void * file_info, const int64_t filepos, const void * buf, const uint64_t data_len);
//int dfs_read_proc(void * file_info, const int64_t filepos, void * buf, uint64_t & data_len);

inline int dfs_write_proc(void * file_info, const int64_t filepos, const void * buf, const uint64_t data_len)
{
    int err = 0;

    if (NULL == file_info) {
        err = -1;
    } else {
        dfs_file_rw_info_t & rw_info = *((dfs_file_rw_info_t *)file_info);
        int64_t written_len = pwrite(rw_info.fd, buf, data_len, rw_info.orig_offset+filepos);

        err = (data_len == (uint64_t)written_len) ? 0 : -1;
    }

    return err;
};

inline int dfs_read_proc(void * file_info, const int64_t filepos, void * buf, const uint64_t data_len)
{
    int err = 0;

    if (NULL == file_info) {
        err = -1;
    } else {
        dfs_file_rw_info_t & rw_info = *((dfs_file_rw_info_t *)file_info);
        int64_t read_len = pread(rw_info.fd, buf, data_len, rw_info.orig_offset+filepos);

        err = (data_len == (uint64_t)read_len) ? 0 : -1;
    }

    return err;
};



template<uint32_t FANOUT> class dfs_btree_node_base_t;
template<uint32_t FANOUT> class dfs_btree_leaf_node_t;
template<uint32_t FANOUT> class dfs_btree_mid_node_t;

template<uint32_t FANOUT>
class dfs_btree_node_base_t : public dfs_bt_const_t
{
    friend class dfs_btree_leaf_node_t<FANOUT>;
    friend class dfs_btree_mid_node_t<FANOUT>;
protected:
    enum cconst_protected {
        BT_HALF_FANOUT = FANOUT/2,
        //leaf or mid
        ATTR_MIDNODE_FLAG       = 1,    //this is a middle node instead of leaf node
        MAX_KEY_NUM             = FANOUT-1  ,
    };
private:
    uint16_t _attr;
    uint16_t _subkey_num;
    uint32_t _reserved;      //����
    uint64_t _mutation_counter;   //mutation counter��������ʱ��ã��Ժ󶼲��޸�
protected:
    uint64_t _subkey_index_ary[MAX_KEY_NUM]; //the first unit also servers as the next index when this unit is free
private:
    //���಻����ʹ�ã�����乹�켰��������Ϊprivate
    dfs_btree_node_base_t() {
        init();
    };
    ~dfs_btree_node_base_t() {
    };
    dfs_btree_node_base_t(const dfs_btree_node_base_t<FANOUT> & src) : dfs_btree_node_base_t<FANOUT>() {
        *this = src;
    };
    dfs_btree_node_base_t<FANOUT> & operator=(const dfs_btree_node_base_t<FANOUT> & src) {
        if (&src != this) {
            memcpy(this, &src, sizeof(*this));
        }
        return *this;
    };
protected:
    void init(void) {
        int64_t j = 0;

        _attr = 0;
        _subkey_num = 0;
        _reserved = 0;
        _mutation_counter = 0;
        for (j = 0; j < (int64_t)len_of_ary(_subkey_index_ary); ++j) {
            _subkey_index_ary[j] = UNDEF_INDEX;
        }
        return;
    };
public:
    const uint64_t * get_subkey_ary(void) const {
        return _subkey_index_ary;
    };
    uint32_t get_subkey_num(void) const {
        return _subkey_num;
    };
    //return: 0 for success, other value for error
    int set_subkey_num(const uint32_t subkey_num) {
        int err = 0;

        if (subkey_num <= MAX_KEY_NUM) {
            _subkey_num = subkey_num;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_SUBKEY_NUM);
        }

        return err;
    };
    void set_mutation_counter(const uint64_t mutation_counter) {
        _mutation_counter = mutation_counter;
    };
    uint64_t get_mutation_counter(void) const {
        return _mutation_counter;
    };
    bool is_leaf_node(void) const {
        return ((_attr & ATTR_MIDNODE_FLAG) == 0);
    };
    bool is_mid_node(void) const {
        return ((_attr & ATTR_MIDNODE_FLAG) != 0);
    };
public:
    //insert an item into scr_buff but save result to tag_buff
    //There should be no overlap between tag_buff and src_buff.
    //return: 0 for success, other value for error
    static int insert(
        uint64_t * tag_buff,
        const uint32_t tag_buff_len,
        const uint64_t * src_buff,
        const uint32_t src_data_len,
        const uint32_t ins_pos,
        const uint64_t ins_data) {
        int err = 0;

        if (src_data_len < tag_buff_len && ins_pos <= src_data_len) {
            memcpy(tag_buff, src_buff, ins_pos*sizeof(tag_buff[0]));
            tag_buff[ins_pos] = ins_data;
            memcpy(tag_buff+ins_pos+1, src_buff+ins_pos, (src_data_len-ins_pos)*sizeof(tag_buff[0]));
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //delete an item from scr_buff but save result to tag_buff
    //There should be no overlap between tag_buff and src_buff.
    //return: 0 for success, other value for error
    static int del(
        uint64_t * tag_buff,
        const uint32_t tag_buff_len,
        const uint64_t * src_buff,
        const uint32_t src_data_len,
        const uint32_t del_pos) {
        int err = 0;

        if (del_pos < src_data_len && (src_data_len-1) <= tag_buff_len) {
            memcpy(tag_buff, src_buff, del_pos*sizeof(tag_buff[0]));
            memcpy(
                tag_buff+del_pos,
                src_buff+(del_pos+1),
                (src_data_len-(del_pos+1))*sizeof(tag_buff[0]));
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_NODE_POS);
        }

        return err;
    };
public:
    bool is_full(void) const {
        return (this->get_subkey_num() >= len_of_ary(this->_subkey_index_ary));
    };
    //return: key_index, UNDEF_INDEX for invalid pos
    uint64_t get_subkey_index(const uint32_t pos) const {
        uint64_t key_index = UNDEF_INDEX;
        int err = 0;

        if (pos < this->get_subkey_num()) {
            key_index = this->_subkey_index_ary[pos];
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
            key_index = UNDEF_INDEX;
        }

        return key_index;
    };
    int get_subkeys(
        uint64_t * buff,
        const uint32_t buff_len) const {
        int err = 0;

        if (this->get_subkey_num() < buff_len) {
            memcpy(buff, this->_subkey_index_ary, this->get_subkey_num()*sizeof(buff[0]));
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_BUF_SIZE);
        }

        return err;
    };
    //insert a key into this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int insert_subkey(
        uint64_t * tag_buff,
        const uint32_t buff_len,
        const uint32_t ins_pos,
        const uint64_t ins_key_index) const {
        return insert(
                   tag_buff,
                   buff_len,
                   this->_subkey_index_ary,
                   this->get_subkey_num(),
                   ins_pos,
                   ins_key_index);
    };
    //delete a key from this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int del_subkey(
        uint64_t * tag_buff,
        const uint32_t buff_len,
        const uint32_t del_pos) const {
        return del(
                   tag_buff,
                   buff_len,
                   this->_subkey_index_ary,
                   this->get_subkey_num(),
                   del_pos);
    };
};


template<uint32_t FANOUT>
class dfs_btree_leaf_node_t : public dfs_btree_node_base_t<FANOUT>
{
protected:
    enum cconst_protected {
        //FANOUT = dfs_btree_node_base_t<IN_FANOUT>::FANOUT,
    };
public:
    friend class dfs_btree_leaf_node_t<FANOUT-2>;
    dfs_btree_leaf_node_t() {
        init();
    };
    dfs_btree_leaf_node_t(const dfs_btree_leaf_node_t<FANOUT> & src) {
        *this = src;
    };
    dfs_btree_leaf_node_t<FANOUT> & operator=(const dfs_btree_leaf_node_t<FANOUT> & src) {
        if (&src != this) {
            memcpy(this, &src, sizeof(*this));
        }
        return *this;
    };
    ~dfs_btree_leaf_node_t() {
    };
    void init(void) {
        dfs_btree_node_base_t<FANOUT>::init();
        this->_attr &= ~(this->ATTR_MIDNODE_FLAG);
    };
public:
    //insert a key into this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int insert_subkey(
        uint64_t * tag_buff,
        const uint32_t buff_len,
        const uint32_t ins_pos,
        const uint64_t ins_key_index) const {
        return insert(
                   tag_buff,
                   buff_len,
                   this->_subkey_index_ary,
                   this->get_subkey_num(),
                   ins_pos,
                   ins_key_index);
    };
    //insert a key into this obj, but save the result to tag_node
    //return: 0 for success, other value for error
    int insert_subkey(
        dfs_btree_leaf_node_t<FANOUT> & tag_node,
        const uint32_t ins_pos,
        const uint64_t ins_key_index) const {
        int err = 0;

        err = insert(
                  tag_node._subkey_index_ary,
                  len_of_ary(tag_node._subkey_index_ary),
                  this->_subkey_index_ary,
                  this->get_subkey_num(),
                  ins_pos,
                  ins_key_index);

        if (0 == err) {
            err = tag_node.set_subkey_num(this->get_subkey_num()+1);
        }

        return err;
    };
    //insert a key into this obj, but save the result to tag_node
    //return: 0 for success, other value for error
    int insert_subkey(
        dfs_btree_leaf_node_t<FANOUT+2> & tag_node,
        const uint32_t ins_pos,
        const uint64_t ins_key_index) const {
        int err = 0;

        err = insert(
                  tag_node._subkey_index_ary,
                  len_of_ary(tag_node._subkey_index_ary),
                  this->_subkey_index_ary,
                  this->get_subkey_num(),
                  ins_pos,
                  ins_key_index);

        if (0 == err) {
            err = tag_node.set_subkey_num(this->get_subkey_num()+1);
        }

        return err;
    };
    //delete a key from this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int del_subkey(
        uint64_t * tag_buff,
        const uint32_t buff_len,
        const uint32_t del_pos) const {
        return del(
                   tag_buff, buff_len,
                   this->_subkey_index_ary,
                   this->get_subkey_num(),
                   del_pos);
    };
    //delete a key from this obj, but save the result to tag_node
    //return: 0 for success, other value for error
    int del_subkey(
        dfs_btree_leaf_node_t<FANOUT> & tag_node,
        const uint32_t del_pos) const {
        int err = del(
                      tag_node._subkey_index_ary,
                      len_of_ary(tag_node._subkey_index_ary),
                      this->_subkey_index_ary,
                      this->get_subkey_num(),
                      del_pos);

        if (0 == err) {
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        }

        return err;
    };
    //delete a key from this obj, but save the result to tag_node
    //return: 0 for success, other value for error
    int del_subkey(
        dfs_btree_leaf_node_t<FANOUT+2> & tag_node,
        const uint32_t del_pos) const {
        int err = del(
                      tag_node._subkey_index_ary,
                      len_of_ary(tag_node._subkey_index_ary),
                      this->_subkey_index_ary,
                      this->get_subkey_num(),
                      del_pos);

        if (0 == err) {
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        }

        return err;
    };
    //update a key of this obj
    //return: 0 for success, other value for error
    int update_subkey(
        const uint32_t update_key_pos,
        const uint64_t new_key_index,
        uint64_t & old_key_index) {
        int err = 0;

        if (update_key_pos < this->get_subkey_num()) {
            old_key_index = atomic_exchange(this->_subkey_index_ary+update_key_pos, new_key_index);
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a key of this obj, but save the result to the tag obj
    //return: 0 for success, other value for error
    int update_subkey(
        dfs_btree_leaf_node_t<FANOUT> & tag_node,
        const uint32_t update_key_pos,
        const uint64_t new_key_index) const {
        int err = 0;

        if (update_key_pos < this->get_subkey_num()) {
            tag_node = *this;
            tag_node._subkey_index_ary[update_key_pos] = new_key_index;
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //return: 0 for success, other value for error
    int put_subkeys(
        const uint64_t * buff,
        const uint32_t subkey_num) {
        int err = 0;

        if (subkey_num <= len_of_ary(this->_subkey_index_ary)) {
            memcpy(this->_subkey_index_ary, buff, subkey_num*sizeof(this->_subkey_index_ary[0]));
            err = this->set_subkey_num(subkey_num);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
public:
    //����ʱ�����Ĳ���
    //���copyҶ�ӽڵ�Ļ�,,��ô���Բ������κβ���...
    inline int action_while_added(void * pgc_info, const uint64_t node_index) {
        return 0;
    };
    //ɾ��ǰ�����Ĳ���
    inline int action_before_gc(void * pgc_info, const uint64_t node_index);
};


template<uint32_t FANOUT>
class dfs_btree_mid_node_t : public dfs_btree_node_base_t<FANOUT>
{
protected:
    enum cconst_protected {
        //FANOUT = dfs_btree_node_base_t<IN_FANOUT>::FANOUT,
    };
public:
    friend class dfs_btree_mid_node_t<FANOUT-2>;
    dfs_btree_mid_node_t() {
        init();
    };
    dfs_btree_mid_node_t(const dfs_btree_mid_node_t<FANOUT> & src) : dfs_btree_node_base_t<FANOUT>() {
        *this = src;
    };
    dfs_btree_mid_node_t<FANOUT> & operator=(const dfs_btree_mid_node_t<FANOUT> & src) {
        if (&src != this) {
            memcpy(this, &src, sizeof(*this));
        }
        return *this;
    };
    ~dfs_btree_mid_node_t() {
    };
    void init(void) {
        int64_t j = 0;

        dfs_btree_node_base_t<FANOUT>::init();
        this->_attr |= this->ATTR_MIDNODE_FLAG;
        for (j = 0; j < (int64_t)len_of_ary(_subnode_index_ary); ++j) {
            _subnode_index_ary[j] = UNDEF_INDEX;
        }
        return;
    };
protected:
    uint64_t _subnode_index_ary[FANOUT];
public:
    //return: key_index, UNDEF_INDEX for invalid pos
    uint64_t get_subnode_index(const uint32_t pos) const {
        uint64_t node_index = UNDEF_INDEX;
        int err = 0;

        if (pos <= this->get_subkey_num()) {
            node_index = this->_subnode_index_ary[pos];
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
            node_index = UNDEF_INDEX;
        }

        return node_index;
    };
public:
    //˵����update_node_index����update_pos����Ȼ����update_pos��߲���(left_key_index,left_son_node_index)�ԣ�
    //      ���������tag_node��
    //return: 0 for success, other value for error
    int update_then_ins_to_left(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_pos,
        const uint64_t left_key_index,
        const uint64_t left_node_index,
        const uint64_t update_node_index) const {
        int err = 0;

        if (this->get_subkey_num() < len_of_ary(this->_subkey_index_ary)
            && update_pos <= this->get_subkey_num()) {
            if ((err = insert(
                           tag_node._subkey_index_ary,
                           len_of_ary(tag_node._subkey_index_ary),
                           this->_subkey_index_ary,
                           this->get_subkey_num(),
                           update_pos,
                           left_key_index)) == 0
                && (err = insert(
                              tag_node._subnode_index_ary,
                              len_of_ary(tag_node._subnode_index_ary),
                              this->_subnode_index_ary,
                              this->get_subkey_num()+1,
                              update_pos,
                              left_node_index)) == 0) {
                tag_node._subnode_index_ary[update_pos+1] = update_node_index;
                err = tag_node.set_subkey_num(this->get_subkey_num()+1);
            }
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //˵����update_node_index����update_pos����Ȼ����update_pos��߲���(left_key_index,left_son_node_index)�ԣ�
    //      ���������tag_node��
    //return: 0 for success, other value for error
    int update_then_ins_to_left(
        dfs_btree_mid_node_t<FANOUT+2> & tag_node,
        const uint32_t update_pos,
        const uint64_t left_key_index,
        const uint64_t left_node_index,
        const uint64_t update_node_index) const {
        int err = 0;

        if (update_pos <= this->get_subkey_num()) {
            if ((err = insert
                       (tag_node._subkey_index_ary,
                        len_of_ary(tag_node._subkey_index_ary),
                        this->_subkey_index_ary,
                        this->get_subkey_num(),
                        update_pos,
                        left_key_index)) == 0
                && (err = insert(
                              tag_node._subnode_index_ary,
                              len_of_ary(tag_node._subnode_index_ary),
                              this->_subnode_index_ary,
                              this->get_subkey_num()+1,
                              update_pos,
                              left_node_index)) == 0) {
                tag_node._subnode_index_ary[update_pos+1] = update_node_index;
                err = tag_node.set_subkey_num(this->get_subkey_num()+1);
            }
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //˵����update_node_index����update_pos����Ȼ����update_pos��߲���(left_key_index,left_son_node_index)�ԣ�
    //      ���������tag_key_buff��tag_node_buff��
    //return: 0 for success, other value for error
    int update_then_ins_to_left(
        uint64_t * tag_key_buff,
        uint64_t * tag_node_buff,
        const uint32_t key_buff_len, //(node_buff_len >= key_buff_len+1)
        const uint32_t update_pos,
        const uint64_t left_key_index,
        const uint64_t left_node_index,
        const uint64_t update_node_index) const {
        int err = 0;

        if (this->get_subkey_num() < key_buff_len && update_pos <= this->get_subkey_num()) {
            if ((err = insert(
                           tag_key_buff,
                           key_buff_len,
                           this->_subkey_index_ary,
                           this->get_subkey_num(),
                           update_pos,
                           left_key_index)) == 0
                && (err = insert(
                              tag_node_buff,
                              key_buff_len+1,
                              this->_subnode_index_ary,
                              this->get_subkey_num()+1,
                              update_pos,
                              left_node_index)) == 0) {
                tag_node_buff[update_pos+1] = update_node_index;
                err = 0;
            }
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //return: 0 for success, other value for error
    int put_pairs(
        const uint64_t * key_buff,
        const uint64_t * node_buff,
        const uint32_t subkey_num) {
        int err = 0;

        if (subkey_num <= len_of_ary(this->_subkey_index_ary)) {
            memcpy(
                this->_subkey_index_ary,
                key_buff,
                subkey_num*sizeof(this->_subkey_index_ary[0]));
            memcpy(
                this->_subnode_index_ary,
                node_buff,
                (subkey_num+1)*sizeof(this->_subnode_index_ary[0]));
            err = this->set_subkey_num(subkey_num);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_BUF_SIZE);
        }

        return err;
    };
    //export nodes to a buff
    //return: 0 for success, other value for error
    int export_subnode_index(
        uint64_t * tag_node_buff,
        const uint32_t node_buff_len) const {
        int err = 0;

        if ((this->get_subkey_num()+1) <= node_buff_len) {
            memcpy(
                tag_node_buff,
                this->_subnode_index_ary,
                (this->get_subkey_num()+1)*sizeof(tag_node_buff[0]));
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_BUF_SIZE);
        }

        return err;
    };
    //insert a key into this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int insert_subkey(
        uint64_t * tag_buff,
        const uint32_t buff_len,
        const uint32_t ins_pos,
        const uint64_t ins_key_index) const {

        return insert(
                   tag_buff,
                   buff_len,
                   this->_subkey_index_ary,
                   this->get_subkey_num(),
                   ins_pos,
                   ins_key_index);
    };
    //update a key of this obj
    //return: 0 for success, other value for error
    int update_subkey(
        const uint32_t update_key_pos,
        const uint64_t new_key_index,
        uint64_t & old_key_index) {
        int err = 0;

        if (update_key_pos < this->get_subkey_num()) {
            old_key_index = atomic_exchange(this->_subkey_index_ary+update_key_pos, new_key_index);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a subnode of this obj
    //return: 0 for success, other value for failure
    int update_subnode(
        const uint32_t update_subnode_pos,
        const uint64_t new_subnode_index,
        uint64_t & old_subnode_index) {
        int err = 0;

        if (update_subnode_pos <= this->get_subkey_num()) {
            old_subnode_index = atomic_exchange(this->_subnode_index_ary+update_subnode_pos,
                                                new_subnode_index);
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a key of this obj, but save the result to the tag obj
    //return: 0 for success, other value for error
    int update_subkey(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_key_pos,
        const uint64_t new_key_index) const {
        int err = 0;

        if (update_key_pos < this->get_subkey_num()) {
            tag_node = *this;
            tag_node._subkey_index_ary[update_key_pos] = new_key_index;
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a subnode of this obj, but save the result to the tag obj
    //return: 0 for success, other value for failure
    int update_subnode(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_subnode_index) const {
        int err = 0;

        if (update_pos <= this->get_subkey_num()) {
            tag_node = *this;
            tag_node._subnode_index_ary[update_pos] = new_subnode_index;
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a (key,node) pair as well as a followed node of this obj, but save the result to the tag obj
    //return: 0 for success, other value for error
    int update_brothers(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_key_index,
        const uint64_t new_node_index,
        const uint64_t new_right_node_index) const {
        int err = 0;

        if (update_pos < this->get_subkey_num()) {
            tag_node = *this;
            tag_node._subkey_index_ary[update_pos] = new_key_index;
            tag_node._subnode_index_ary[update_pos] = new_node_index;
            tag_node._subnode_index_ary[update_pos+1] = new_right_node_index;
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a (key,node) pair as well as a followed node of this obj, but save the result to the tag buff
    //return: 0 for success, other value for error
    int update_brothers(
        uint64_t * tag_key_buff,
        uint64_t * tag_node_buff,
        const uint32_t key_buff_len, //(node_buff_len >= key_buff_len+1)
        const uint32_t update_pos,
        const uint64_t new_key_index,
        const uint64_t new_node_index,
        const uint64_t new_right_node_index) const {
        int err = 0;

        if (update_pos < this->get_subkey_num() && key_buff_len >= this->get_subkey_num()) {
            memcpy(
                tag_key_buff,
                this->_subkey_index_ary,
                this->get_subkey_num()*sizeof(tag_key_buff[0]));
            memcpy(
                tag_node_buff,
                this->_subnode_index_ary,
                (this->get_subkey_num()+1)*sizeof(tag_node_buff[0]));
            tag_key_buff[update_pos] = new_key_index;
            tag_node_buff[update_pos] = new_node_index;
            tag_node_buff[update_pos+1] = new_right_node_index;
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    ////update a (key,node) pair, but save the result to the tag obj
    ////return: 0 for success, other value for error
    //int update_pair(
    //        dfs_btree_mid_node_t<FANOUT> & tag_node,
    //        const uint32_t update_pos,
    //        const uint64_t new_key_index,
    //        const uint64_t new_node_index) const
    //{
    //    int err = 0;

    //    if (update_pos < this->get_subkey_num())
    //    {
    //        tag_node = *this;
    //        tag_node._subkey_index_ary[update_pos] = new_key_index;
    //        tag_node._subnode_index_ary[update_pos] = new_node_index;
    //        err = 0;
    //    }
    //    else
    //    {
    //        err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
    //    }

    //    return err;
    //};
    //delete a pair from this obj, but save the result to a buff
    //return: 0 for success, other value for error
    int del_pair(
        uint64_t * tag_key_buff,
        uint64_t * tag_node_buff,
        const uint32_t key_buff_len, //(node_buff_len >= key_buff_len+1)
        const uint32_t del_pos) const {
        int err = 0;

        if (del_pos < this->get_subkey_num() && (key_buff_len+1) >= this->get_subkey_num()) {
            del(
                tag_key_buff, key_buff_len,
                this->_subkey_index_ary,
                this->get_subkey_num(),
                del_pos);
            del(
                tag_node_buff, key_buff_len+1,
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                del_pos);
            err = 0;
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //delete a pair from this obj, but save the result to a tag obj
    //return: 0 for success, other value for error
    int del_pair(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t del_pos) const {
        int err = 0;

        if (del_pos < this->get_subkey_num()) {
            del(
                tag_node._subkey_index_ary,
                len_of_ary(tag_node._subkey_index_ary),
                this->_subkey_index_ary,
                this->get_subkey_num(),
                del_pos);
            del(
                tag_node._subnode_index_ary,
                len_of_ary(tag_node._subnode_index_ary),
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                del_pos);
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };

    //update a subnode pointed by update_pos and delete its left pair, but save the result to a tag obj
    int update_then_del_left(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_node_index) const {
        int err = 0;

        if (0 < update_pos && update_pos <= this->get_subkey_num()) {
            del(
                tag_node._subkey_index_ary,
                len_of_ary(tag_node._subkey_index_ary),
                this->_subkey_index_ary,
                this->get_subkey_num(),
                update_pos-1);
            del(
                tag_node._subnode_index_ary,
                len_of_ary(tag_node._subnode_index_ary),
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                update_pos-1);
            tag_node._subnode_index_ary[update_pos-1] = new_node_index;
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update a subnode pointed by update_pos and delete its left pair, but save the result to a tag obj
    int update_then_del_left(
        dfs_btree_mid_node_t<FANOUT+2> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_node_index) const {
        int err = 0;

        if (0 < update_pos && update_pos <= this->get_subkey_num()) {
            del(
                tag_node._subkey_index_ary,
                len_of_ary(tag_node._subkey_index_ary),
                this->_subkey_index_ary,
                this->get_subkey_num(),
                update_pos-1);
            del(
                tag_node._subnode_index_ary,
                len_of_ary(tag_node._subnode_index_ary),
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                update_pos-1);
            tag_node._subnode_index_ary[update_pos-1] = new_node_index;
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };

    //update the left pair and the right subnode and delete myself pair, but save the result to a tag obj
    int update_both_sides_then_del_mid(
        dfs_btree_mid_node_t<FANOUT> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_left_key_index,
        const uint64_t new_left_subnode_index,
        const uint64_t new_right_subnode_index) const {
        int err = 0;

        if (0 < update_pos && (update_pos+1) <= this->get_subkey_num()) {
            del(
                tag_node._subkey_index_ary,
                len_of_ary(tag_node._subkey_index_ary),
                this->_subkey_index_ary,
                this->get_subkey_num(),
                update_pos);
            del(
                tag_node._subnode_index_ary,
                len_of_ary(tag_node._subnode_index_ary),
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                update_pos);
            tag_node._subkey_index_ary[update_pos-1]  = new_left_key_index;
            tag_node._subnode_index_ary[update_pos-1] = new_left_subnode_index;
            tag_node._subnode_index_ary[update_pos]   = new_right_subnode_index;
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
    //update the left pair and the right subnode and delete myself pair, but save the result to a tag obj
    int update_both_sides_then_del_mid(
        dfs_btree_mid_node_t<FANOUT+2> & tag_node,
        const uint32_t update_pos,
        const uint64_t new_left_key_index,
        const uint64_t new_left_subnode_index,
        const uint64_t new_right_subnode_index) const {
        int err = 0;

        if (0 < update_pos && (update_pos+1) <= this->get_subkey_num()) {
            del(
                tag_node._subkey_index_ary,
                len_of_ary(tag_node._subkey_index_ary),
                this->_subkey_index_ary,
                this->get_subkey_num(),
                update_pos);
            del(
                tag_node._subnode_index_ary,
                len_of_ary(tag_node._subnode_index_ary),
                this->_subnode_index_ary,
                this->get_subkey_num()+1,
                update_pos);
            tag_node._subkey_index_ary[update_pos-1]  = new_left_key_index;
            tag_node._subnode_index_ary[update_pos-1] = new_left_subnode_index;
            tag_node._subnode_index_ary[update_pos]   = new_right_subnode_index;
            err = tag_node.set_subkey_num(this->get_subkey_num()-1);
        } else {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NODE_POS);
        }

        return err;
    };
public:
    //����ʱ�����Ĳ���
    inline int action_while_added(void * pgc_info, const uint64_t node_index) {
        return 0;
    };
    //ɾ��ǰ�����Ĳ���
    inline int action_before_gc(void * pgc_info, const uint64_t node_index);
};


//��һB���ĸ��ڵ�
class dfs_sbt_root_t    //single btree root
{
private:
    uint64_t _root_node_index;
    uint64_t _next_allocate_id;
    uint64_t _mutation_counter;
public:
    dfs_sbt_root_t() {
        init();
    };
    ~dfs_sbt_root_t() {
    };
public:
    inline const dfs_sbt_root_t & operator=(const dfs_sbt_root_t & src) {
        if (&src != this) {
            _root_node_index        = src._root_node_index      ;
            _next_allocate_id       = src._next_allocate_id     ;
            _mutation_counter       = src._mutation_counter     ;
        }
        return *this;
    };
public:
    inline void set_value(const dfs_sbt_root_t & src) {
        *this = src;
    };
public:
    dfs_sbt_root_t(const dfs_sbt_root_t & src) :
        _root_node_index(src._root_node_index),
        _next_allocate_id(src._next_allocate_id),
        _mutation_counter(src._mutation_counter) {
    };
    void init(void) {
        _root_node_index = UNDEF_INDEX;
        //�ӱ��1��ʼ���з���...
        _next_allocate_id = 1;  //skip NULL_ID
        _mutation_counter = 0;
        return;
    };
public:
    //���룺new_root_index
    //return: 0 for success, other values for error
    int set_root_node_index(uint64_t & old_root_index, const uint64_t new_root_index) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (UNDEF_INDEX == new_root_index) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "UNDEF_INDEX == new_root_index");
            err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_INVALID_INDEX);
        } else {
            old_root_index = atomic_exchange(&_root_node_index, new_root_index);
        }

        return 0;
    };
    uint64_t get_root_node_index(void) const {
        return _root_node_index;
    };
public:
    bool is_invalid_id(const uint64_t id) const {
        return (NULL_ID == id || UNDEF_ID == id
                || _next_allocate_id > 1 && id >= _next_allocate_id);
    };
    uint64_t alloc_id(void) {
        return atomic_inc(&_next_allocate_id)-1;
    };
    void set_next_allocate_id(const uint64_t next_allocate_id) {
        atomic_exchange(&_next_allocate_id, next_allocate_id);
        return;
    };
    uint64_t get_next_allocate_id(void) const {
        return _next_allocate_id;
    };
    uint64_t get_mutation_counter(void) const {
        return _mutation_counter;
    };
    void set_mutation_counter(const uint64_t mutation_counter) {
        _mutation_counter = mutation_counter;
        return ;
    };
    //uint64_t inc_mutation_counter(void)
    //{
    //    return ++_mutation_counter;
    //};
};

class dfs_btree_t;

//
//���ڵ���Ϣ
class dfs_bt_root_t : public dfs_bt_const_t
{
private:
    //dfs_btree_t * _pbtree;
    //����������ʵ��...:).
    //fp��߿��ܵ�4.
    dfs_sbt_root_t _sbt_root_ary[MAX_BT_INSTANCE_NUM];
public:
    dfs_bt_root_t() { // : _pbtree(NULL)
        //init();
    };
    ~dfs_bt_root_t() {
    };
    const dfs_bt_root_t & operator=(const dfs_bt_root_t & src) {
        if (&src != this) {
            //memcpy(this, &src, sizeof(*this));
            for (int64_t j = 0; j < (int64_t)len_of_ary(_sbt_root_ary); ++j) {
                _sbt_root_ary[j] = src._sbt_root_ary[j];
            }
        }
        return *this;
    };
    dfs_bt_root_t(const dfs_bt_root_t & src) {
        //memcpy(this, &src, sizeof(*this));
        for (int64_t j = 0; j < (int64_t)len_of_ary(_sbt_root_ary); ++j) {
            _sbt_root_ary[j] = src._sbt_root_ary[j];
        }
    };
    void init(void) {
        for (int64_t j = 0; j < (int64_t)len_of_ary(_sbt_root_ary); ++j) {
            _sbt_root_ary[j].init();
        }
        return;
    };
public:
    //void set_btree(dfs_btree_t * pbtree)
    //{
    //    _pbtree = pbtree;
    //};
    //���룺new_root_node_index(might be UNDEF_INDEX)
    //return: 0 for success, other values for error
    int set_root(const dfs_sbt_root_t & sbt_root, const uint32_t pos) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            DF_WRITE_LOG_US(log_level, "pos=%d", pos);
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
        }
        _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)].set_value(sbt_root);

        return err;
    };
    const dfs_sbt_root_t & get_root(const uint32_t pos) const {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            DF_WRITE_LOG_US(log_level, "pos=%d", pos);
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
        }

        return _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)];
    };
public:
    //���룺new_root_index
    //return: 0 for success, other values for error
    int set_root_node_index(uint64_t & old_root_index, const uint64_t new_root_index, const uint32_t pos) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
            DF_WRITE_LOG_US(log_level,
                            "new_root_index=0x%lx, pos=%u",
                            new_root_index, pos);
        } else if (UNDEF_INDEX == new_root_index) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "UNDEF_INDEX == new_root_index");
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_INVALID_INDEX);
            DF_WRITE_LOG_US(log_level,
                            "new_root_index=0x%lx, pos=%u",
                            new_root_index, pos);
        } else if ((err = _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)].set_root_node_index(
                              old_root_index,
                              new_root_index)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_sbt_root_ary[].set_root_node_index() returns 0x%x", err);
            DF_WRITE_LOG_US(log_level,
                            "new_root_index=0x%lx, pos=%u",
                            new_root_index, pos);
        }

        return err;
    };
    uint64_t get_root_node_index(const uint32_t pos) const {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            DF_WRITE_LOG_US(log_level, "pos=%d", pos);
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
        }

        return _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)].get_root_node_index();
    };
    //int set_mutation_counter(const uint64_t mutation_counter, const uint32_t pos)
    //{
    //    int log_level = DF_UL_LOG_NONE;
    //    int err = 0;
    //
    //    if (pos >= len_of_ary(_root_info_ary))
    //    {
    //        log_level = DF_UL_LOG_FATAL;
    //        DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_root_info_ary)");
    //        DF_WRITE_LOG_US(log_level, "pos=%d", pos);
    //        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
    //    }
    //
    //    return _root_info_ary[pos%len_of_ary(_root_info_ary)].set_mutation_counter(mutation_counter);
    //};
    //uint64_t get_mutation_counter(const uint32_t pos) const
    //{
    //    int log_level = DF_UL_LOG_NONE;
    //    int err = 0;
    //
    //    if (pos >= len_of_ary(_root_info_ary))
    //    {
    //        log_level = DF_UL_LOG_FATAL;
    //        DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_root_info_ary)");
    //        DF_WRITE_LOG_US(log_level, "pos=%d", pos);
    //        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
    //    }
    //
    //    return _root_info_ary[pos%len_of_ary(_root_info_ary)].get_mutation_counter();
    //};
    bool is_invalid_id(const uint64_t id, const uint32_t pos) const {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            DF_WRITE_LOG_US(log_level, "pos=%d", pos);
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
        }

        return _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)].is_invalid_id(id);
    };
    uint64_t alloc_id(const uint32_t pos) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (pos >= len_of_ary(_sbt_root_ary)) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "pos >= len_of_ary(_sbt_root_ary)");
            DF_WRITE_LOG_US(log_level, "pos=%d", pos);
            err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
        }

        return _sbt_root_ary[pos%len_of_ary(_sbt_root_ary)].alloc_id();
    };
public:
    //����/���ٸ��ڵ����ü���
    //????.
    //????.
    inline int action_while_added(void * pgc_info, const uint64_t node_index);
    inline int action_before_gc(void * pgc_info, const uint64_t node_index);
};



//��ɾ�������������м�ڵ�ʱ��dfs_btree_drill_t�ж�Ӧ��node_index���滻��һ���µģ�
//��Ӧλ�õ�key_index���滻��������������key_index
class dfs_btree_drill_t : public dfs_bt_const_t
{
protected:
    int _drill_ptr;
    int _del_mid_drill_ptr;         //ɾ�������м�ڵ�
    uint64_t _del_mid_old_node_index; //ɾ�������м�ڵ�ԭ����node_index
    uint64_t _btree_mutation_counter; //inc whenever there is an update,�޸ĵļ�����Χ...
    uint64_t _subnode_index_ary[MAX_BT_DEPTH]; //the node history for drilling down...node to drill down..
    uint32_t _ins_pos_ary[MAX_BT_DEPTH]; //insert pos in the node (_subnode_index_ary[]) of btree
public:
    dfs_btree_drill_t() {
        init();
    };
    ~dfs_btree_drill_t() {
    };
    dfs_btree_drill_t & operator=(const dfs_btree_drill_t & src) {
        if (&src != this) {
            memcpy(this, &src, sizeof(*this));
        }
        return *this;
    };
    bool operator==(const dfs_btree_drill_t & src) const {
        return (_drill_ptr == src._drill_ptr &&
                memcmp(
                    _subnode_index_ary,
                    src._subnode_index_ary,
                    (_drill_ptr+1)*sizeof(_subnode_index_ary[0])) == 0
                && memcmp(
                    _ins_pos_ary,
                    src._ins_pos_ary,
                    (_drill_ptr+1)*sizeof(_ins_pos_ary[0])) == 0);
    };
    bool operator!=(const dfs_btree_drill_t & src) const {
        return !(operator==(src));
    };
    void init(void) {
        int64_t j = 0;

        _drill_ptr = -1;
        _del_mid_drill_ptr = MAX_BT_DEPTH; //????.
        _del_mid_old_node_index = UNDEF_INDEX;
        _btree_mutation_counter = 0;

        for (j = 0; j < (int64_t)len_of_ary(_subnode_index_ary); ++j) {
            _subnode_index_ary[j] = UNDEF_INDEX;
        }
        for (j = 0; j < (int64_t)len_of_ary(_ins_pos_ary); ++j) {
            _ins_pos_ary[j] = 0;
        }

        return;
    };
public:
    //return: 0 for success, other value for error
    int push(const uint32_t ins_pos, const uint64_t node_index) {
        int err = 0;

        if (_drill_ptr < ((int)len_of_ary(_subnode_index_ary)-1)) {
            ++_drill_ptr;
            _subnode_index_ary[_drill_ptr] = node_index;
            _ins_pos_ary[_drill_ptr] = ins_pos;
            err = 0;
        } else {
            DF_WRITE_LOG_US(DF_UL_LOG_FATAL, "error: push drill overflow _drill_ptr=%d", _drill_ptr);
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_OVERFLOW);
        }

        return err;
    };
    //return: 0 for success, other value for error
    int pop(void) {
        int err = 0;

        if (_drill_ptr >= 0) {
            _subnode_index_ary[_drill_ptr] = UNDEF_INDEX;
            --_drill_ptr;
            err = 0;
        } else {
            DF_WRITE_LOG_US(DF_UL_LOG_FATAL, "error: pop drill underflow _drill_ptr=%d", _drill_ptr);
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_UNDERFLOW);
        }

        return err;
    };
public:
    int get_drill_ptr(void) const {
        return _drill_ptr;
    };
    //return: node_index��UNDEF_INDEX for invalid drill_pos
    uint64_t get_node_index(const int drill_pos) const {
        uint64_t node_index = UNDEF_INDEX;
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (drill_pos >= 0 && drill_pos <= _drill_ptr) {
            if ((node_index = _subnode_index_ary[drill_pos]) == UNDEF_INDEX) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "node_index == UNDEF_INDEX");
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_NODE_INDEX);
            }
        } else {
            log_level = DF_UL_LOG_TRACE;
            DF_WRITE_LOG_US(log_level, "drill_pos < 0 || drill_pos > _drill_ptr");
            err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_DRILL_POS);
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "drill_pos=%d, _drill_ptr=%d, node_index=0x%lx",
                            drill_pos, _drill_ptr, node_index)
        }

        return node_index;
    };
    //return: node_index��UNDEF_INDEX for invalid drill_pos
    uint64_t get_last_node_index(void) const {
        uint64_t node_index = UNDEF_INDEX;
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_drill_ptr >= 0 && _drill_ptr < (int)len_of_ary(_subnode_index_ary)) {
            if ((node_index = _subnode_index_ary[_drill_ptr]) == UNDEF_INDEX) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "node_index == UNDEF_INDEX");
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_NODE_INDEX);
            }
        } else {
            log_level = DF_UL_LOG_TRACE;
            DF_WRITE_LOG_US(log_level, "_drill_ptr < 0 || _drill_ptr >= len_of_ary(_subnode_index_ary)");
            err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_DRILL_POS);
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "_drill_ptr=%d, node_index=0x%lx",
                            _drill_ptr, node_index)
        }

        return node_index;
    }
    //return: ins_pos, UNDEF_POS for invalid drill_pos
    uint32_t get_ins_pos(const int drill_pos) const {
        uint32_t ins_pos = UNDEF_POS;
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (drill_pos >= 0 && drill_pos <= _drill_ptr) {
            if ((ins_pos = _ins_pos_ary[drill_pos]) >= BT_FANOUT) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "ins_pos >= BT_FANOUT");
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_INS_POS);
            }
        } else {
            log_level = DF_UL_LOG_TRACE;
            DF_WRITE_LOG_US(log_level, "drill_pos < 0 || drill_pos > _drill_ptr");
            err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_DRILL_POS);
            ins_pos = UNDEF_POS;
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "drill_pos=%d, _drill_ptr=%d, ins_pos=%d",
                            drill_pos, _drill_ptr, ins_pos)
        }

        return ins_pos;
    };
    //return: ins_pos, UNDEF_POS for invalid drill_pos
    uint32_t get_last_ins_pos(void) const {
        uint32_t ins_pos = UNDEF_POS;
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_drill_ptr >= 0 && _drill_ptr < (int)len_of_ary(_ins_pos_ary)) {
            if ((ins_pos = _ins_pos_ary[_drill_ptr]) >= BT_FANOUT) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "ins_pos >= BT_FANOUT");
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_INS_POS);
            }
        } else {
            log_level = DF_UL_LOG_TRACE;
            DF_WRITE_LOG_US(log_level, "_drill_ptr < 0 || _drill_ptr >= len_of_ary(_ins_pos_ary)");
            err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_DRILL_POS);
            ins_pos = UNDEF_POS;
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "_drill_ptr=%d, ins_pos=%d",
                            _drill_ptr, ins_pos)
        }

        return ins_pos;
    };
    //return: 0 for success, otherwise error
    int inc_last_ins_pos(void) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_drill_ptr >= 0 && _drill_ptr < (int)len_of_ary(_ins_pos_ary)) {
            ++_ins_pos_ary[_drill_ptr];
            if (_ins_pos_ary[_drill_ptr] >= BT_FANOUT) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "_ins_pos_ary[_drill_ptr] >= BT_FANOUT");
                DF_WRITE_LOG_US(log_level, "_ins_pos_ary[_drill_ptr]=%d", _ins_pos_ary[_drill_ptr]);
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_INS_POS);
            }
        } else {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_drill_ptr < 0 || _drill_ptr >= len_of_ary(_ins_pos_ary)");
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_POS);
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "_drill_ptr=%d",
                            _drill_ptr);
        }

        return err;
    }
    //return: 0 for success, otherwise error
    int dec_last_ins_pos(void) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_drill_ptr >= 0 && _drill_ptr < (int)len_of_ary(_ins_pos_ary)) {
            if (_ins_pos_ary[_drill_ptr] <= 0) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "_ins_pos_ary[_drill_ptr] <= 0");
                DF_WRITE_LOG_US(log_level, "_ins_pos_ary[_drill_ptr]=%d", _ins_pos_ary[_drill_ptr]);
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_INS_POS);
            } else {
                --_ins_pos_ary[_drill_ptr];
            }
        } else {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_drill_ptr < 0 || _drill_ptr >= len_of_ary(_ins_pos_ary)");
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_POS);
        }

        if (DF_UL_LOG_NONE != log_level) {
            DF_WRITE_LOG_US(log_level,
                            "_drill_ptr=%d",
                            _drill_ptr);
        }

        return err;
    };
    //void empty(void)
    //{
    //    init();
    //};
    //bool is_empty(void) const
    //{
    //    return (_drill_ptr < 0);
    //};
    //bool is_only_one_step(void) const
    //{
    //    return (0 == _drill_ptr);
    //};
public:
    void set_btree_mutation_counter(const uint64_t btree_mutation_counter) {
        _btree_mutation_counter = btree_mutation_counter;
    };
    uint64_t get_btree_mutation_counter(void) const {
        return _btree_mutation_counter;
    };
public:
    //Ҫɾ����key���ڵ�B���ڵ���drill_info�е�λ��
    //��ɾ����λ���м�ڵ�ʱ��ԭ����node_index
    int set_del_mid_drill_info(const int del_mid_drill_ptr, const uint64_t del_mid_new_node_index) {
        int err = 0;

        if ((_del_mid_drill_ptr = del_mid_drill_ptr) >= 0 && _del_mid_drill_ptr < _drill_ptr) {
            _del_mid_old_node_index = _subnode_index_ary[_del_mid_drill_ptr];
            _subnode_index_ary[_del_mid_drill_ptr] = del_mid_new_node_index;
        } else {
            DF_WRITE_LOG_US(DF_UL_LOG_FATAL, "error del_mid_drill_ptr=%d, _drill_ptr=%d", del_mid_drill_ptr, _drill_ptr);
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_DRILL_DEL_NODE_POS);
        }

        return err;
    };
    //Ҫɾ����key���ڵ�B���ڵ���drill_info�е�λ��
    int get_del_mid_drill_ptr(void) const {
        return _del_mid_drill_ptr;
    };
    //��ɾ����λ���м�ڵ�ʱ��ԭ����node_index
    uint64_t get_del_mid_old_node_index(void) const {
        return _del_mid_old_node_index;
    };
public:
    //bool is_match_tree(const uint64_t root_index, const uint64_t mutation_counter) const
    //{
    //    return (get_drill_ptr() >= 0
    //            && get_btree_mutation_counter() == mutation_counter
    //            && get_node_index(0) == root_index);
    //};
    //???what doest it mean??.
    bool not_match_tree(const dfs_sbt_root_t & sbt_root) const {
        return (get_drill_ptr() < 0
                || get_btree_mutation_counter() != sbt_root.get_mutation_counter()
                || get_node_index(0) != sbt_root.get_root_node_index());
    };
};


//�Խڵ����(����/ɾ��)��Ľ��
class dfs_btree_node_mutate_t : public dfs_bt_const_t
{
public:
    enum cconst_public {
        //�ڵ��������
        MUTATE_NOTHING      = 0 ,   //û�и��²���
        UPDATE_NODE             ,   //�����ɾ�������²������½ڵ�
        SPLIT_NODE              ,   //�ڵ������(�����������½ڵ�)
        REBALANCE_LEFT          ,   //�����ֵܽ�����ƽ��(�������µ���ڵ���µı��ڵ�)
        REBALANCE_RIGHT         ,   //�����ֵܽ�����ƽ��(�������µı��ڵ���µ��ҽڵ�)
        MERGE_LEFT              ,   //��Ϊɾ���������ֵܽ��кϲ�(���ֵܱ�ɾ��)
        MERGE_RIGHT             ,   //��Ϊɾ���������ֵܽ����˺ϲ�(���ڵ㱻ɾ��)
        MERGE_BOTH              ,   //��Ϊɾ����ͬʱ�������ֵܽ����˺ϲ�(���ڵ㱻ɾ��)
    };
protected:
    int _mutate_type;
    int _unused;
    uint64_t _new_key_index          ;
    uint64_t _new_1st_node_index ;
    uint64_t _new_2nd_node_index  ;
public:
    dfs_btree_node_mutate_t() {
        _mutate_type = 0;
        _unused = 0;
        _new_key_index = UNDEF_INDEX;
        _new_1st_node_index = UNDEF_INDEX;
        _new_2nd_node_index = UNDEF_INDEX;
    };
    ~dfs_btree_node_mutate_t() {
    };
    dfs_btree_node_mutate_t(const dfs_btree_node_mutate_t & src) {
        *this = src;
    };
    dfs_btree_node_mutate_t & operator=(const dfs_btree_node_mutate_t & src) {
        if (&src != this) {
            memcpy(this, &src, sizeof(*this));
        }
        return *this;
    };
public:
    int get_update_type(void) const {
        return _mutate_type;
    };
    void set_mutate_nothing(void) {
        _mutate_type = MUTATE_NOTHING;
        _new_key_index = UNDEF_INDEX;
        _new_1st_node_index  = UNDEF_INDEX;
        _new_2nd_node_index = UNDEF_INDEX;
    };
    void set_update_node_info(const uint64_t new_node_index) {
        _mutate_type = UPDATE_NODE;
        _new_key_index = UNDEF_INDEX;
        _new_1st_node_index = new_node_index;
        _new_2nd_node_index  = UNDEF_INDEX;
    };
    int get_update_node_info(uint64_t & node_index) const {
        int err = 0;

        if (get_update_type() != UPDATE_NODE) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            node_index = UNDEF_INDEX;
        } else {
            node_index = _new_1st_node_index;
            err = 0;
        }

        return err;
    };
    void set_split_node_info(
        const uint64_t new_left_key_index,
        const uint64_t new_left_node_index,
        const uint64_t new_right_node_index) {
        _mutate_type = SPLIT_NODE;
        _new_key_index = new_left_key_index;
        _new_1st_node_index = new_left_node_index;
        _new_2nd_node_index  = new_right_node_index;
    };
    int get_split_node_info(
        uint64_t & new_left_key_index,
        uint64_t & new_left_node_index,
        uint64_t & new_right_node_index) const {
        int err = 0;

        if (get_update_type() != SPLIT_NODE) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_left_key_index = UNDEF_INDEX;
            new_left_node_index  = UNDEF_INDEX;
            new_right_node_index = UNDEF_INDEX;
        } else {
            new_left_key_index = _new_key_index;
            new_left_node_index  = _new_1st_node_index;
            new_right_node_index = _new_2nd_node_index;
            err = 0;
        }
        return err;
    };

    void set_rebalance_left_info(
        const uint64_t new_left_key_index,
        const uint64_t new_left_node_index,
        const uint64_t new_myself_node_index) {
        _mutate_type = REBALANCE_LEFT;
        _new_key_index = new_left_key_index;
        _new_1st_node_index = new_left_node_index;
        _new_2nd_node_index  = new_myself_node_index;
    };
    int get_rebalance_left_info(
        uint64_t & new_left_key_index,
        uint64_t & new_left_node_index,
        uint64_t & new_myself_node_index) const {
        int err = 0;

        if (get_update_type() != REBALANCE_LEFT) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_left_key_index = UNDEF_INDEX;
            new_left_node_index     = UNDEF_INDEX;
            new_myself_node_index   = UNDEF_INDEX;
        } else {
            new_left_key_index = _new_key_index;
            new_left_node_index     = _new_1st_node_index;
            new_myself_node_index   = _new_2nd_node_index;
            err = 0;
        }

        return err;
    };
    void set_rebalance_right_info(
        const uint64_t new_myself_key_index,
        const uint64_t new_myself_node_index,
        const uint64_t new_right_node_index) {
        _mutate_type = REBALANCE_RIGHT;
        _new_key_index = new_myself_key_index;
        _new_1st_node_index = new_myself_node_index;
        _new_2nd_node_index  = new_right_node_index;
    };
    int get_rebalance_right_info(
        uint64_t & new_myself_key_index,
        uint64_t & new_myself_node_index,
        uint64_t & new_right_node_index) const {
        int err = 0;

        if (get_update_type() != REBALANCE_RIGHT) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_myself_key_index = UNDEF_INDEX;
            new_myself_node_index   = UNDEF_INDEX;
            new_right_node_index    = UNDEF_INDEX;
        } else {
            new_myself_key_index = _new_key_index;
            new_myself_node_index   = _new_1st_node_index;
            new_right_node_index    = _new_2nd_node_index;
            err = 0;
        }

        return err;
    };
    void set_merge_left_info(const uint64_t new_myself_node_index) {
        _mutate_type = MERGE_LEFT;
        _new_key_index = UNDEF_INDEX;
        _new_1st_node_index = new_myself_node_index;
        _new_2nd_node_index  = UNDEF_INDEX;
    };
    int get_merge_left_info(uint64_t & new_myself_node_index) const {
        int err = 0;

        if (get_update_type() != MERGE_LEFT) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_myself_node_index = UNDEF_INDEX;
        } else {
            new_myself_node_index = _new_1st_node_index;
            err = 0;
        }

        return err;
    };
    void set_merge_right_info(const uint64_t new_right_node_index) {
        _mutate_type = MERGE_RIGHT;
        _new_key_index = UNDEF_INDEX;
        _new_1st_node_index = new_right_node_index;
        _new_2nd_node_index  = UNDEF_INDEX;
    };
    int get_merge_right_info(uint64_t & new_right_node_index) const {
        int err = 0;

        if (get_update_type() != MERGE_RIGHT) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_right_node_index = UNDEF_INDEX;
        } else {
            new_right_node_index = _new_1st_node_index;
            err = 0;
        }

        return err;
    };
    void set_merge_both_info(
        const uint64_t new_left_key_index,
        const uint64_t new_left_node_index,
        const uint64_t new_right_node_index) {
        _mutate_type = MERGE_BOTH;
        _new_key_index = new_left_key_index;
        _new_1st_node_index = new_left_node_index ;
        _new_2nd_node_index  = new_right_node_index;
    };
    int get_merge_both_info(
        uint64_t & new_left_key_index,
        uint64_t & new_left_node_index,
        uint64_t & new_right_node_index) const {
        int err = 0;

        if (get_update_type() != MERGE_BOTH) {
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_MUTATE_TYPE);
            new_left_key_index = UNDEF_INDEX;
            new_left_node_index = UNDEF_INDEX;
            new_right_node_index = UNDEF_INDEX;
        } else {
            new_left_key_index = _new_key_index;
            new_left_node_index  = _new_1st_node_index ;
            new_right_node_index = _new_2nd_node_index  ;
            err = 0;
        }

        return err;
    };
};

class dfs_btree_fhead_base_t
{
public:
    uint32_t size;   //sizeof of the head (not only the base part)
    uint32_t ver;
    int64_t   total_data_len; //including the head
    uint32_t major_tag;  //�������������ͣ���'fpbt', 'idbt'
    uint32_t minor_tag;  //��С���������ͣ���'fpbt'����'fpbt'��'idbt'
    uint64_t reserve10;
};

//btree�Լ������ͷ��Ϣ�����ֲ�ͬbtree(ID,FP,NS,XZ)���ᱣ���Լ���ͷ��Ϣ
class dfs_btree_fhead_ext_t
{
public:
    uint64_t total_leaf_node_num;
    uint64_t total_mid_node_num;
    uint64_t total_key_num;
    int32_t   store_t_type; //STORE_T_INDEX, STORE_T_VALUE or STORE_T_VALUE_KEEP_T_INDEX
    uint32_t reserve20;
    uint64_t reserve21[24];
};

//btree�Լ������ͷ��Ϣ�����ֲ�ͬbtree(ID,FP,NS,XZ)���ᱣ���Լ���ͷ��Ϣ
class dfs_btree_fhead_t : public dfs_btree_fhead_base_t, public dfs_btree_fhead_ext_t
{
public:
    enum cconst_public {
        STORE_T_INDEX               = 0 ,
        STORE_T_VALUE                   ,
        STORE_T_VALUE_KEEP_T_INDEX      ,   //STORE_T_AND_KEEP_T_INDEX shoud > STORE_T_ONLY

        CORE_BT_VER = 0x00000002,     //Hihg-16-bit: main ver;
        CORE_BT_TAG = (((int)'b')<<24) | (((int)'t')<<16) | (((int)'r')<<8) | (((int)'e')), //"btree"
        ATTR_MIDNODE_FLAG   = 1,    //this is a middle node instead of leaf node
    };
};




class dfs_btree_store_info_t :
    virtual public dfs_bt_const_t,
public dfs_s_ary_t<uint64_t>
{
public:
    enum cconst_public {
        STORE_T_INDEX               = dfs_btree_fhead_t::STORE_T_INDEX              ,
        STORE_T_VALUE               = dfs_btree_fhead_t::STORE_T_VALUE              ,
        STORE_T_VALUE_KEEP_T_INDEX  = dfs_btree_fhead_t::STORE_T_VALUE_KEEP_T_INDEX ,
    };
private:
    dfs_init_t _init_state;
    const dfs_write_proc_t _write_proc;
    const pvoid _file_info;
    int64_t _file_offset;
    char * _buf;
    const uint64_t _buf_size;
    uint64_t _data_pos;
    uint64_t _total_leaf_node_num;
    uint64_t _total_mid_node_num;
    uint64_t _total_key_num;
    uint32_t _major_tag;
    int32_t _store_t_type; //STORE_T_INDEX, STORE_T_VALUE or STORE_T_VALUE_KEEP_T_INDEX
    uint64_t _kept_key_num;
    //uint64_t _max_kept_key_index;
public:
    dfs_btree_store_info_t(
        const dfs_write_proc_t write_proc,
        const pvoid file_info,
        const uint64_t buf_size) :
        dfs_s_ary_t<uint64_t>(UNDEF_INDEX),
        _write_proc(write_proc),
        _file_info(file_info),
        _buf_size(buf_size)

    {
        _buf = NULL;
        init();
    };
    ~dfs_btree_store_info_t() {
        flush_data();
        if (NULL != _buf) {
            delete [] _buf;
            _buf = NULL;
        }
    };
    int init(void) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_init_state.is_not_init_yet()) {
            _file_offset = 0;
            _data_pos = 0;
            _total_leaf_node_num = 0;
            _total_mid_node_num = 0;
            _total_key_num = 0;
            _store_t_type = STORE_T_VALUE;
            _kept_key_num = 0;
            //_max_kept_key_index = 0;

            if ((err = dfs_s_ary_t<uint64_t>::init()) != 0) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "dfs_s_ary_t<uint64_t>::init() returns 0x%x", err);
            } else if (NULL != _buf) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NOT_NULL_BUF_POINTER);
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "NULL != _buf");
            } else if ((_buf = new char[_buf_size]) == NULL) {
                err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_NOMEM);
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, ERRINFO_BT_NOMEM, "_buf");
            }

            if (DF_UL_LOG_NONE != log_level) {
                DF_WRITE_LOG_US(log_level,
                                "_buf=0x%p, _buf_size=%ld",
                                _buf, _buf_size);
            }

            _init_state.set_init_result(err);
        } else {
            err = _init_state.get_init_result();
        }

        return err;
    };
    void init_counters(void) {
        _total_leaf_node_num = 0;
        _total_mid_node_num = 0;
        _total_key_num = 0;
    };
public:
    void set_major_tag(const uint32_t major_tag) {
        _major_tag = major_tag;
    };
    uint32_t get_major_tag(void) const {
        return _major_tag;
    };
    //uint64_t get_mutation_counter(void) const
    //{
    //    return _mutation_counter;
    //};
    //void set_mutation_counter(const uint64_t mutation_counter)
    //{
    //    _mutation_counter = mutation_counter;
    //};
    uint64_t get_total_leaf_node_num(void) const {
        return _total_leaf_node_num;
    };
    uint64_t get_total_mid_node_num(void) const {
        return _total_mid_node_num;
    };
    uint64_t get_total_key_num(void) const {
        return _total_key_num;
    };
public:
    uint64_t inc_total_leaf_node_num(void) {
        return ++_total_leaf_node_num;
    };
    uint64_t inc_total_mid_node_num(void) {
        return ++_total_mid_node_num;
    };
    uint64_t add_total_key_num(const uint32_t key_num) {
        _total_key_num += key_num;
        return _total_key_num;
    };
public:
    //void set_file_offset(const int64_t file_offset)
    //{
    //    _file_offset = file_offset;
    //};
    int64_t get_cur_file_offset(void) const {
        return (_file_offset+(int64_t)_data_pos);
    };
    int32_t get_store_t_type(void) const {
        return _store_t_type;
    };
    void set_store_t_index(void) {
        _store_t_type = STORE_T_INDEX;
    };
    void set_store_t_value(void) {
        _store_t_type = STORE_T_VALUE;
    };
    void set_store_t_value_keep_t_index(void) {
        _store_t_type = STORE_T_VALUE_KEEP_T_INDEX;
    };
public:
    int flush_data(void) {
        int err = 0;

        if (_data_pos > 0) {
            if ((err = _write_proc(_file_info, _file_offset, _buf, _data_pos)) != 0) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_WRITE_FILE);
            }
            _file_offset += _data_pos;
            _data_pos = 0;
        }

        return err;
    };
    int padding(void) {
        //to purpose for padding: 1. makes decode_num() always has at least 9 bytes; 2. alignment
        uint64_t padding_num = 0x20 -(get_cur_file_offset() & 0xf);
        int err = require_space(padding_num);

        if (0 == err) {
            memset(_buf+_data_pos, 0, padding_num);
            _data_pos += padding_num;
            //err = flush_data();
        }

        return err;
    };
    int rewrite_data(const void * buf, const uint64_t data_len, const int64_t file_offset) {
        //refill the head
        int err = flush_data();

        if (0 == err) {
            if ((err = _write_proc(_file_info, file_offset, buf, data_len)) != 0) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_WRITE_FILE);
            }
        }

        return err;
    };
    uint64_t get_avail_space(void) const {
        return (_buf_size-_data_pos);
    };
    int require_space(const uint64_t space_len) {
        int err = 0;
        if (get_avail_space() <= space_len) {
            if ((err = flush_data()) == 0 && get_avail_space() <= space_len) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_BUF_POS);
            }
        }
        return err;
    };
    int encode_num(const uint64_t num) {
        int err = require_space(9);

        if (0 == err) {
            err = dfs_bt_const_t::encode_num(num,_buf,_buf_size,_data_pos);
        }

        return err;
    };
    //encode the length of data and also guarantee there enough buf space for the data
    int encode_data_len(const uint64_t data_len) {
        int err = require_space(9+data_len);

        if (0 == err) {
            err = dfs_bt_const_t::encode_num(data_len,_buf,_buf_size,_data_pos);
        }

        return err;
    };
    int store_data(const uint64_t data) {
        int err = require_space(sizeof(data));

        if (0 == err) {
            *((uint64_t *)(_buf+_data_pos)) = data;
            _data_pos += sizeof(data);
        }

        return err;
    };
    int store_data(const uint32_t data) {
        int err = require_space(sizeof(data));

        if (0 == err) {
            *((uint32_t *)(_buf+_data_pos)) = data;
            _data_pos += sizeof(data);
        }

        return err;
    };
    int store_buf(const void * buf, const uint32_t data_len) {
        int err = require_space(data_len);

        if (0 == err) {
            memcpy(_buf+_data_pos, buf, data_len);
            _data_pos += data_len;
        }

        return err;
    };
public:
    char * get_buf(void) const {
        return _buf;
    };
    uint64_t get_buf_size(void) const {
        return _buf_size;
    };
    uint64_t & get_data_pos(void) {
        return _data_pos;
    };
public:
    int keep_obj_index(const uint64_t key_index) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (UNDEF_INDEX == key_index) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "UNDEF_INDEX == key_index");
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_INVALID_INDEX);
        } else if ((err = dfs_s_ary_t<uint64_t>::put_t(key_index, _kept_key_num)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "dfs_s_ary_t<uint64_t>::put_t() returns 0x%x", err);
            DF_WRITE_LOG_US(log_level,
                            "key_index=0x%lx, _kept_key_num=%ld",
                            key_index, _kept_key_num);
        } else {
            ++_kept_key_num;
        }

        return err;
    };
    int get_kept_obj_seq(const uint64_t key_index, uint64_t & key_seq) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        key_seq = UNDEF_INDEX;
        if (UNDEF_INDEX == key_index) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "UNDEF_INDEX == key_index");
            err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_INVALID_INDEX);
        } else if ((err = dfs_s_ary_t<uint64_t>::get_t(key_index, key_seq)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "dfs_s_ary_t<uint64_t>::get_t() returns 0x%x", err);
            DF_WRITE_LOG_US(log_level,
                            "key_index=0x%lx, _kept_key_num=%ld, key_seq=%ld",
                            key_index, _kept_key_num, key_seq);
        }

        return err;
    }
    //int clear_kept_key_index(void)
    //{
    //    return dfs_s_ary_t<uint64_t>::clear();
    //};
    int clear_session_nums(void) {
        _total_leaf_node_num = 0;
        _total_mid_node_num = 0;
        _total_key_num = 0;
        _kept_key_num = 0;
        //_max_kept_key_index = 0;

        return dfs_s_ary_t<uint64_t>::clear();
    };
};



class dfs_btree_load_info_t :
    virtual public dfs_bt_const_t,
public dfs_s_ary_t<uint64_t>
{
public:
    enum cconst_public {
        STORE_T_INDEX               = dfs_btree_fhead_t::STORE_T_INDEX              ,
        STORE_T_VALUE               = dfs_btree_fhead_t::STORE_T_VALUE              ,
        STORE_T_VALUE_KEEP_T_INDEX  = dfs_btree_fhead_t::STORE_T_VALUE_KEEP_T_INDEX ,
    };
private:
    dfs_init_t _init_state;
    const dfs_read_proc_t _read_proc;
    const pvoid _file_info;
    int64_t _file_offset;
    int64_t _max_file_offset;  //����total_data_len�����������������λ��
    char * _buf;
    const uint64_t _buf_size;
    uint64_t _data_len;
    uint64_t _data_pos;
    uint64_t _total_leaf_node_num;
    uint64_t _total_mid_node_num;
    uint64_t _total_key_num;
    uint32_t _major_tag;
    int _store_t_type; //STORE_T_INDEX, STORE_T_VALUE or STORE_T_VALUE_KEEP_T_INDEX
    uint64_t _kept_key_num;
    //uint64_t _max_kept_key_index;
public:
    dfs_btree_load_info_t(
        const dfs_read_proc_t read_proc,
        const pvoid file_info,
        const uint64_t buf_size) :
        dfs_s_ary_t<uint64_t>(UNDEF_INDEX),
        _read_proc(read_proc),
        _file_info(file_info),
        _buf_size(buf_size) {
        _buf = NULL;
        init();
    };
    ~dfs_btree_load_info_t() {
        if (NULL != _buf) {
            delete [] _buf;
            _buf = NULL;
        }
    };
    int init(void) {
        int log_level = DF_UL_LOG_NONE;
        int err = 0;

        if (_init_state.is_not_init_yet()) {
            _file_offset = 0;
            _max_file_offset = 0;
            _data_len = 0;
            _data_pos = 0;
            _total_leaf_node_num = 0;
            _total_mid_node_num = 0;
            _total_key_num = 0;
            _store_t_type = STORE_T_VALUE;
            _kept_key_num = 0;
            //_max_kept_key_index = 0;

            if ((err = dfs_s_ary_t<uint64_t>::init()) != 0) {
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "dfs_s_ary_t<uint64_t>::init() returns 0x%x", err);
            } else if (NULL != _buf) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_NOT_NULL_BUF_POINTER);
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, "NULL != _buf");
            } else if ((_buf = new char[_buf_size]) == NULL) {
                err = DF_BT_SET_NORMAL_ERR(this->ERRNO_BT_NOMEM);
                log_level = DF_UL_LOG_FATAL;
                DF_WRITE_LOG_US(log_level, ERRINFO_BT_NOMEM, "_buf");
            }

            if (DF_UL_LOG_NONE != log_level) {
                DF_WRITE_LOG_US(log_level,
                                "_buf=0x%p, _buf_size=%ld",
                                _buf, _buf_size);
            }

            _init_state.set_init_result(err);
        } else {
            err = _init_state.get_init_result();
        }

        return err;
    };
public:
    void set_major_tag(const uint32_t major_tag) {
        _major_tag = major_tag;
    };
    uint32_t get_major_tag(void) const {
        return _major_tag;
    };
    //uint64_t get_mutation_counter(void) const
    //{
    //    return _mutation_counter;
    //};
    //void set_mutation_counter(const uint64_t mutation_counter)
    //{
    //    _mutation_counter = mutation_counter;
    //};
    uint64_t get_total_leaf_node_num(void) const {
        return _total_leaf_node_num;
    };
    uint64_t get_total_mid_node_num(void) const {
        return _total_mid_node_num;
    };
    uint64_t get_total_key_num(void) const {
        return _total_key_num;
    };
public:
    uint64_t dec_total_leaf_node_num(void) {
        return --_total_leaf_node_num;
    };
    uint64_t dec_total_mid_node_num(void) {
        return --_total_mid_node_num;
    };
    uint64_t sub_total_key_num(const uint32_t key_num) {
        _total_key_num -= key_num;
        return _total_key_num;
    };
public:
    void set_total_nums(
        const uint64_t total_leaf_node_num,
        const uint64_t total_mid_node_num,
        const uint64_t total_key_num) {
        _total_leaf_node_num = total_leaf_node_num   ;
        _total_mid_node_num  = total_mid_node_num    ;
        _total_key_num       = total_key_num         ;
    };
public:
    //void set_file_offset(const int64_t file_offset)
    //{
    //    _file_offset = file_offset;
    //};
    void set_max_file_offset(const int64_t max_file_offset) {
        _max_file_offset = max_file_offset;
    };
    int64_t get_cur_file_offset(void) const {
        return (_file_offset-(int64_t)get_avail_data_len());
    };
    int64_t get_max_file_offset(void) const {
        return _max_file_offset;
    };
    int file_seek(const int64_t file_offset) {
        int err = 0;

        if (file_offset == _file_offset) {
            _data_pos = 0;
            _data_len = 0;
        } else if (file_offset != get_cur_file_offset()) {
            if (file_offset <= 0) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_FILE_POS);
            } else {
                _file_offset = file_offset;
                if ((err = _read_proc(_file_info, _file_offset-1, _buf, 1)) != 0) {
                    err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_READ_FILE);
                }
                _data_pos = 0;
                _data_len = 0;
            }
        }

        return err;
    };
    int32_t get_store_t_type(void) const {
        return _store_t_type;
    };
    void set_store_t_type(const int store_t_type) {
        _store_t_type = store_t_type;
    };
public:
    int read_data(void) {
        uint64_t read_len = 0;
        int err = 0;

        if (_file_offset < _max_file_offset) {
            if (_data_pos > 0) {
                memmove(_buf, _buf+_data_pos, (_data_len-_data_pos));
                _data_len -= _data_pos;
                _data_pos = 0;
            }
            read_len = MIN((int64_t)(_buf_size-_data_len),(_max_file_offset-_file_offset));
            if ((err = _read_proc(_file_info, _file_offset, _buf+_data_len, read_len)) != 0) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_READ_FILE);
            }
            _file_offset += read_len;
            _data_len += read_len;
        }

        return err;
    };
    uint64_t get_avail_data_len(void) const {
        return (_data_len-_data_pos);
    };
    int require_data_len(const uint64_t data_len) {
        int err = 0;
        if (get_avail_data_len() < data_len) {
            if ((err = read_data()) == 0 && get_avail_data_len() < data_len) {
                err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_FILE_POS);
            }
        }
        return err;
    };
    int decode_num(uint64_t & num) {
        int err = require_data_len(9);

        if (0 == err) {
            err = dfs_bt_const_t::decode_num(num,_buf,_data_len,_data_pos);
        }

        return err;
    };

    //decode the length of data and also guarantee the data in the buf
    int decode_data_len(uint64_t & data_len) {
        int err = require_data_len(9);

        if (0 == err) {
            err = dfs_bt_const_t::decode_num(data_len,_buf,_data_len,_data_pos);
            if (0 == err) {
                err = require_data_len(data_len);
            }
        }

        return err;
    };
    int load_data(uint64_t & data) {
        int err = require_data_len(sizeof(data));

        if (0 == err) {
            data = *((uint64_t *)(_buf+_data_pos));
            _data_pos += sizeof(data);
        }

        return err;
    };
    int load_data(uint32_t & data) {
        int err = require_data_len(sizeof(data));

        if (0 == err) {
            data = *((uint32_t *)(_buf+_data_pos));
            _data_pos += sizeof(data);
        }

        return err;
    };
    int load_buf(void * buf, const uint32_t data_len) {
        int err = 0;

        if ((err = require_data_len(data_len)) == 0) {
            memcpy(buf, _buf+_data_pos, data_len);
            _data_pos += data_len;
        }

        return err;
    };
    int load_data_len(uint32_t & data_len) {
        int err = load_data(data_len);

        if (0 == err) {
            err = require_data_len(data_len);
        }

        return err;
    };
    int skip_data_len(const uint32_t data_len) {
        int err = require_data_len(data_len);

        if (0 == err) {
            _data_pos += data_len;
        }

        return err;
    }
public:
    char * get_buf(void) const {
        return _buf;
    };
    uint64_t get_data_len(void) const {
        return _data_len;
    };
    uint64_t & get_data_pos(void) {
        return _data_pos;
    };
public:
    int keep_obj_index(const uint64_t key_index) {
        int err = dfs_s_ary_t<uint64_t>::put_t(_kept_key_num, key_index);
        //if (key_index > _max_kept_key_index)
        //{
        //    _max_kept_key_index = key_index;
        //}
        ++_kept_key_num;
        return err;
    };
    int get_kept_key_index(const uint64_t key_seq, uint64_t & key_index) {
        int err = 0;

        key_index = UNDEF_INDEX;
        //if (key_seq >= _kept_key_num)
        //{
        //    err = DF_BT_SET_INTERNAL_ERR(this->ERRNO_BT_SUBKEY_NUM);
        //}
        //else
        //{
        err = dfs_s_ary_t<uint64_t>::get_t(key_seq, key_index);
        //}

        return err;
    }
    //int clear_kept_key_index(void)
    //{
    //    return dfs_s_ary_t<uint64_t>::clear();
    //};
    int clear_session_nums(void) {
        _total_leaf_node_num = 0;
        _total_mid_node_num = 0;
        _total_key_num = 0;
        _kept_key_num = 0;
        //_max_kept_key_index = 0;

        return dfs_s_ary_t<uint64_t>::clear();
    };
};

//enum bt_ckp_state_t
//{
//    BT_CKP_STATE_OFF        = 0 ,
//    BT_CKP_STATE_ON         = 1 ,
//    BT_CKP_STATE_STORING    = 2 ,
//    BT_CKP_STATE_LOADING    = 3 ,
//};


//
//dfs_btree_t��B���Ļ������֣����B��Ԫ�صĲ����ɾ���Լ��ɴ˵��µ�����ƽ�������
class dfs_btree_t :
    virtual public dfs_bt_const_t,
public dfs_bt2d_ary_t<
    dfs_btree_leaf_node_t<dfs_bt_const_t::BT_FANOUT>,
    dfs_bt_const_t::BT_LEAF_ROW_SIZE>,
public dfs_bt2d_ary_t<
    dfs_btree_mid_node_t<dfs_bt_const_t::BT_FANOUT>,
    dfs_bt_const_t::BT_MID_ROW_SIZE>
{
    friend class dfs_bt_root_t;
private:
    enum cconst_private {
        STATE_INIT_FAIL         = -2    ,
        STATE_INIT_NOT_YES      = -1    ,
        STATE_INIT_SUCCEED      = 0     ,

        LEAF_NODE_FLAG_BIT      = 63    ,
        //COW_NUM_BITS            = 10    ,   //lowest bits
    };
    static const uint32_t BT_FANOUT = dfs_bt_const_t::BT_FANOUT;
    static const uint32_t BT_LEAF_ROW_SIZE = dfs_bt_const_t::BT_LEAF_ROW_SIZE;
    static const uint32_t BT_MID_ROW_SIZE = dfs_bt_const_t::BT_MID_ROW_SIZE;
    static const uint32_t BT_HALF_FANOUT = (BT_FANOUT-1)/2;
    static const uint64_t MAX_MID_INDEX = MAX_U_INT64/4-2;
    static const uint64_t MAX_LEAF_INDEX = MAX_U_INT64/4-2;
    static const uint64_t LEAF_NODE_FLAG = ((uint64_t)0x1)<<LEAF_NODE_FLAG_BIT;
private:
    //typedef T *PT;
    //typedef const T *PCT;
    typedef dfs_btree_node_base_t<BT_FANOUT> node_base_t;
    typedef dfs_btree_leaf_node_t<BT_FANOUT> leaf_node_t;
    typedef dfs_btree_mid_node_t<BT_FANOUT> mid_node_t;
    typedef dfs_bt2d_ary_t<dfs_btree_leaf_node_t<BT_FANOUT>, BT_LEAF_ROW_SIZE> leaf_ary_t;
    typedef dfs_bt2d_ary_t<dfs_btree_mid_node_t<BT_FANOUT>, BT_MID_ROW_SIZE> mid_ary_t;
protected:
    typedef void * pvoid;
private:
    static uint64_t _add_leaf_flag_bit(const uint64_t node_index) {
        return (node_index|LEAF_NODE_FLAG);
    };
    static uint64_t _add_mid_flag_bit(const uint64_t node_index) {
        return node_index;
    };
    static uint64_t _remove_node_flag_bit(const uint64_t node_index) {
        return (node_index&~LEAF_NODE_FLAG);
    };
protected:
    static bool _is_leaf_node(const uint64_t node_index) {
        return ((node_index&LEAF_NODE_FLAG) != 0);
    };
    static bool _is_mid_node(const uint64_t node_index) {
        return ((node_index&LEAF_NODE_FLAG) == 0);
    };

protected:
    enum cconst_protected {
        COW_LOG_LEVEL       = DF_UL_LOG_NOTICE,
        CKP_LOG_LEVEL       = COW_LOG_LEVEL,
        VERIFY_LOG_LEVEL    = COW_LOG_LEVEL,
    };
private:
    dfs_init_t _init_state;
    //dfs_sbt_root_t _wr_sbt_root;  //�޸ĵ�root
    volatile uint64_t _mutation_counter;
    //��Ҫcopy-on-write�����mutation_counter
    volatile uint64_t _max_cow_mutation_counter;
    volatile uint32_t _is_reserved_leaf_node_acquired;
    volatile uint32_t _is_reserved_mid_node_acquired;
    volatile uint32_t _is_cancel_checkpointing; //ȡ�����ڽ����е�store/load checkpointing
    //��ǰʵ�����...
    uint32_t _bt_instance_pos; //������һϵ��ʵ���е���ţ�һ��Ϊ0��fp_btree��Ϊ0~3��ns_btreeΪ0~1
public:
    dfs_btree_t() {
        //_wr_sbt_root.init();
        _mutation_counter = 0;
        //��Ҫcopy-on-write�����mutation_counter???
        _max_cow_mutation_counter = 0;
        _is_reserved_leaf_node_acquired = 0;
        _is_reserved_mid_node_acquired = 0;
        _is_cancel_checkpointing = 0;
        _bt_instance_pos = UNDEF_POS;

        //init();
    };
    virtual ~dfs_btree_t() {
        _init_state.set_destructed();
    };
    //�������ʼ�����ڵ�
    int init(const uint32_t bt_instance_pos, dfs_bt_root_t & new_wr_bt_root);
protected:
    inline uint32_t _bt_get_instance_pos(void) const;
    //int _bt_set_bt_instance_pos(const uint32_t bt_instance_pos);
    inline const dfs_sbt_root_t & _bt_get_sbt_root(const dfs_bt_root_t & bt_root) const;
    inline int _bt_set_sbt_root(
        dfs_bt_root_t & bt_root,
        const dfs_sbt_root_t & sbt_root) const;
private:
    //Disable operator=() and copy constructor
    const dfs_btree_t & operator=(const dfs_btree_t & src);
    dfs_btree_t(const dfs_btree_t & src);
    //const dfs_btree_t & operator=(const dfs_btree_t & src)
    //{
    //    return *this;
    //};
    //dfs_btree_t(const dfs_btree_t & src) :
    //            leaf_ary_t(), mid_ary_t()
    //{
    //};
protected:
    ////���wr_root
    //const dfs_sbt_root_t & _bt_get_wr_root(void);
    ////�ع����Ӽ���Ӧ���ڵ����ü���(����load checkpoint��)
    //int _bt_rollback_wr_root(const dfs_sbt_root_t & sbt_root);
protected:
    //inline uint64_t _bt_alloc_id(void);
    //inline bool _bt_is_invalid_id(const uint64_t id) const;
protected:
    //inline uint64_t _bt_inc_mutation_counter(void);
    inline uint64_t _bt_update_mutation_counter(const bool is_batch_mode);
    inline uint64_t _bt_get_mutation_counter(void) const;
    inline uint64_t _bt_get_max_cow_mutation_counter(void) const;
    //��_max_cow_mutation_counter����Ϊ��ǰ��mutation_counter
    inline void _bt_update_max_cow_mutation_counter(void) const;
    inline bool _bt_is_cancel_checkpointing(void) const;
    //return previous state
    inline bool _bt_set_cancel_checkpointing(const bool is_cancel);
    inline int _bt_inc_root_ref_counter(const dfs_bt_root_t & bt_root) const;
    inline int _bt_dec_root_ref_counter(const dfs_bt_root_t & bt_root) const;
protected:
    //store all contents of a tree to file
    int _bt_store_tree(
        const dfs_bt_root_t & bt_root,
        dfs_btree_store_info_t & store_info,
        dfs_btree_fhead_t & btree_fhead) const;
    //
    int _bt_load_tree(
        dfs_bt_root_t & new_wr_bt_root,
        dfs_btree_load_info_t & load_info,
        dfs_btree_fhead_t & btree_fhead);

private:
    //store all contents of a subtree to file
    int _store_subtree(
        const uint64_t subtree_root,
        dfs_btree_store_info_t & store_info) const;
    int _store_subkeys(
        const uint64_t node_index,
        dfs_btree_store_info_t & store_info) const;
    int _store_fhead(
        dfs_btree_store_info_t & store_info,
        int64_t & file_head_offset,
        dfs_btree_fhead_t & btree_fhead) const;
    int _update_stored_fhead(
        dfs_btree_store_info_t & store_info,
        const int64_t file_head_offset,
        dfs_btree_fhead_t & btree_fhead) const;
    int _load_fhead(
        dfs_btree_load_info_t & load_info,
        int64_t & file_head_offset,
        dfs_btree_fhead_t & btree_fhead) const;
    int _load_subtree(
        uint64_t & subtree_root,
        dfs_btree_load_info_t & load_info);
    int _load_subkeys(
        uint64_t * subkey_index_ary,
        const uint32_t ary_len,
        uint32_t & subkey_num,
        bool & is_mid_node,
        dfs_btree_load_info_t & load_info);
private:
    //return: 0 for success(be a valid index), other value for error
    inline int _get_leaf_node_for_mutate(const uint64_t final_node_index, leaf_node_t ** ppleaf) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _get_mid_node_for_mutate(const uint64_t final_node_index, mid_node_t ** ppmid) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _get_leaf_node(const uint64_t final_node_index, const leaf_node_t ** ppleaf) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _get_mid_node(const uint64_t final_node_index, const mid_node_t ** ppmid) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _get_node(const uint64_t final_node_index, const node_base_t ** ppbase) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _valid_leaf_node_index_verify(const uint64_t final_node_index) const;
    //return: 0 for success(be a valid index), other value for error
    inline int _valid_mid_node_index_verify(const uint64_t final_node_index) const;
    //���ܣ����node(leaf or mid)��pos����key_index
    //Return: 0 for success, other value for error
    inline int _get_key_of_node(const uint64_t node_index, const uint32_t pos, uint64_t & key_index) const;
    //���ܣ����mid node��pos����subnode
    //Return: 0 for success, other value for error
    inline int _get_subnode_of_mid_node(const uint64_t node_index, const uint32_t pos, uint64_t & subnode_index) const;
private:
    //relase node��dfs_2d_ary�Զ�����(����ref_counter)
    int _acquire_leaf_node(uint64_t & final_node_index, leaf_node_t ** ppleaf_node);
    int _acquire_reserved_leaf_node(uint64_t & final_node_index, leaf_node_t ** ppleaf_node);
    //return: 0 for success, other values for failure
    int _release_reserved_leaf_node(const uint64_t final_node_index);
    //relase node��dfs_2d_ary�Զ�����(����ref_counter)
    int _acquire_mid_node(uint64_t & final_node_index, mid_node_t ** ppmid_node);
    int _acquire_reserved_mid_node(uint64_t & final_node_index, mid_node_t ** ppmid_node);
    //return: 0 for success, other values for failure
    int _release_reserved_mid_node(const uint64_t final_node_index);
    int _get_leaf_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter) const;
    int _get_mid_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter) const;
    int _get_node_ref_counter(const uint64_t node_index, uint64_t & ref_counter) const {
        return ((_is_leaf_node(node_index)) ?
                _get_leaf_node_ref_counter(node_index, ref_counter) :
                _get_mid_node_ref_counter(node_index, ref_counter));
    };
    //return: 0 for success, other value for error
    int _inc_leaf_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter);
    int _inc_leaf_node_ref_counter_if_not_zero(const uint64_t final_node_index, uint64_t & ref_counter);
    int _inc_mid_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter);
    int _inc_mid_node_ref_counter_if_not_zero(const uint64_t final_node_index, uint64_t & ref_counter);
    int _inc_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter) {
        return  ((_is_mid_node(final_node_index)) ?
                 _inc_mid_node_ref_counter(final_node_index, ref_counter)
                 : _inc_leaf_node_ref_counter(final_node_index, ref_counter));
    };
    int _inc_node_ref_counter_if_not_zero(const uint64_t final_node_index, uint64_t & ref_counter) {
        return  ((_is_mid_node(final_node_index)) ?
                 _inc_mid_node_ref_counter_if_not_zero(final_node_index, ref_counter)
                 : _inc_leaf_node_ref_counter_if_not_zero(final_node_index, ref_counter));
    };
    int _dec_leaf_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter);
    int _dec_mid_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter);
    int _dec_node_ref_counter(const uint64_t final_node_index, uint64_t & ref_counter) {
        return ((_is_mid_node(final_node_index)) ?
                _dec_mid_node_ref_counter(final_node_index, ref_counter) :
                _dec_leaf_node_ref_counter(final_node_index, ref_counter));
    };
    //���ܣ���������һ���ڵ������(key_index��B���ڵ�)���ü���
    //Return: 0 for success, other value for error
    int _inc_pointed_ref_counter(const uint64_t final_node_index);
    //���ܣ�����ɾ��һ���ڵ������(key_index��B���ڵ�)���ü���
    //Return: 0 for success, other value for error
    int _dec_pointed_ref_counter(const uint64_t final_node_index);
public:
    int action_before_leaf_node_gc(const uint64_t internal_node_index);
    int action_before_mid_node_gc(const uint64_t internal_node_index);
private:
    ////���ܣ����ӻ�ɾ��һ��ֵ�������������ʱ��ȷ������������ֵ�ƽ�⣬���Ƕ��Է��ѳ�Ϊ�����ڵ�
    ////      left_brother_node_index������UNDEF_INDEX����֮ƽ�⣬right_brother_node_index������UNDEF_INDEX����֮ƽ�⣬
    ////      ������Է��ѳ����ڵ�
    //int _choose_rebalance_brother(const uint32_t new_key_number,
    //                               uint64_t & left_brother_node_index, uint64_t & right_brother_node_index,
    //                               const dfs_btree_drill_t & drill_info, const int drill_ptr);
    //���ܣ����ӻ�ɾ��һ��ֵ�������������ʱ��ȷ������������ֵ�ƽ�⣬���Ƕ��Է��ѳ�Ϊ�����ڵ�
    //      left_brother_key_num,right_brother_key_num�������ֵܵ�key�ĸ�����Ϊ0��ʾ������
    //���أ�0 for success, other values for failure
    int _get_left_right_brothers_info(
        uint32_t & left_brother_key_num,
        uint32_t & right_brother_key_num,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr);
private:
    //���ܣ�����mutate_result��B�������ҷ����µĸ��ڵ㣬�����滻��ǰ���ĸ��ڵ�
    //���룺mutate_result��root_info
    //���أ�0 for success, other value for failure
    int _op_tree(
        const dfs_bt_root_t & old_wr_bt_root,
        dfs_bt_root_t & new_wr_bt_root,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
private:
    //���ܣ���һ���ڵ�������ɾ�������������ֵܽڵ�ƽ�⣬�����¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _leaf_rebalance_left(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ�������ɾ����������ұ��ֵܽڵ�ƽ�⣬�����¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _leaf_rebalance_right(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ��������󣬽��з��ѣ������¼��mutate_result
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _leaf_split(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ�����������������ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _leaf_merge_left(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ��������������ұ��ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _leaf_merge_right(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ����������������ұ��ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _leaf_merge_both(
        const dfs_btree_leaf_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
private:
    //���ܣ����ӽڵ�����ɾ���������½ڵ�(û�з��ѻ�����ƽ��)���Ա��ڵ����
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_update(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ��ӽڵ�������ɾ����������ֵ�ƽ��󣬸��±��ڵ㡣
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_rebalance_left(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ��ӽڵ�������ɾ������������ֵ�ƽ��󣬸��±��ڵ㡣
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_rebalance_right(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ����ӽڵ���뵼�·��Ѷ��Ա��ڵ����
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_split(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);

    //���ܣ����ӽڵ������ֵܺϲ����Ա��ڵ����
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_merge_left(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ����ӽڵ������ֵܺϲ����Ա��ڵ����
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_merge_right(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ����ӽڵ��������ֵܺϲ����Ա��ڵ����
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _op_mid_node_of_sub_merge_both(
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
private:
    //���ܣ�����һ��Ҷ�ڵ��һ��subkey�����д��mutate_result
    //���أ�0 for success, other value for failure
    int _update_leaf_node_subkey(
        const uint64_t node_index,
        const uint32_t ins_pos,
        const uint64_t new_key_index,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ�����һ���м�ڵ��һ��subkey�����д��mutate_result
    //���أ�0 for success, other value for failure
    int _update_mid_node_subkey(
        const uint64_t node_index,
        const uint32_t ins_pos,
        const uint64_t new_key_index,
        dfs_btree_node_mutate_t & mutate_result);
private:
    //���ܣ����ڵ�(������ɾ�������)����������ֵܽڵ�ƽ�⣬�����¼��mutate_result
    //      updated_myself_node�������ɾ�������Ľڵ�
    //���أ�0 for success, other values for failure
    int _mid_rebalance_left(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ����ڵ�(������ɾ�������)�������ұ��ֵܽڵ�ƽ�⣬�����¼��mutate_result
    //      updated_myself_node�������ɾ�������Ľڵ�
    //���أ�0 for success, other values for failure
    int _mid_rebalance_right(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ����ڵ����������󣬽��з��ѣ������¼��mutate_result
    //      updated_myself_node�������ɾ�������Ľڵ�
    //���أ�0 for success, other values for failure
    int _mid_split(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ�����������������ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _mid_merge_left(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ��������������ұ��ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���룺updated_myself_node�������ɾ����ı��ڵ�
    //���أ�0 for success, other values for failure
    int _mid_merge_right(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
    //���ܣ���һ���ڵ���ɾ����������������ұ��ֵܽڵ�ϲ��������¼��mutate_result
    //�������룺mutate_result
    //���أ�0 for success, other values for failure
    int _mid_merge_both(
        const dfs_btree_mid_node_t<BT_FANOUT+2> & updated_myself_node,
        const dfs_btree_drill_t & drill_info,
        const int drill_ptr,
        dfs_btree_node_mutate_t & mutate_result);
private:
    //���ܣ���B��Ҷ�ڵ��в����ɾ��һ����(������Ҷ�ڵ�)������Ҫ���丸�ڵ�Ĳ�����¼��mutate_result
    //      ins_key_indexΪ�������key_index����UNDEF_INDEX(ɾ����ʱ)
    //      ���drill_info.del_drill_ptr������-1���������ɾ����������del_drill_ptr < drill_ptrʱ��
    //      ��ʾɾ���������м�ڵ��ϣ�����Ҫ�Ѷ�Ӧ���������key_index�������м�ڵ�Ķ�Ӧλ�á�
    //���أ�0 for success, other value for failure
    int _ins_del_leaf_node(
        const uint64_t ins_key_index,
        const dfs_btree_drill_t & drill_info,
        dfs_btree_node_mutate_t & mutate_result);
protected:
    //���ܣ���B���в���һ�����ҷ����µĸ��ڵ㣬�����滻��ǰ���ĸ��ڵ�
    //���룺root_info
    //���أ�0 for success, other value for failure
    int _insert_to_btree(
        const dfs_bt_root_t & old_wr_bt_root,
        dfs_bt_root_t & new_wr_bt_root,
        const uint64_t ins_obj_index,
        const dfs_btree_drill_t & drill_info);

    //���ܣ���B���и���һ���drill_ptr >= 0������ָ��Ҷ�ڵ���м�ڵ㡣
    //���룺root_info
    //˵������������Ҫ��֤����ǰ���������ͬ��key��
    //���أ�0 for success, other value for failure
    int _update_of_btree(
        const dfs_bt_root_t & old_wr_bt_root,
        dfs_bt_root_t & new_wr_bt_root,
        const uint64_t new_obj_index,
        const dfs_btree_drill_t & drill_info);
    //���ܣ���B����ɾ��һ���drill_ptr >= 0������ָ��Ҷ�ڵ���м�ڵ㡣
    //���룺root_info
    //���أ�0 for success, other value for failure
    int _del_from_btree(
        const dfs_bt_root_t & old_wr_bt_root,
        dfs_bt_root_t & new_wr_bt_root,
        const dfs_btree_drill_t & drill_info);
    //���ܣ�����һ������Ϊ�յĸ��ڵ�(�Ա�����ͨ��_update_root()�����)
    //  return: 0 for success, other values for error
    int _acquire_empty_root(
        dfs_bt_root_t & new_wr_bt_root);
protected:
    //  ���ܣ����B����СԪ�ز����drill_info
    //  �����������(B���ǿ�)����pctָ������key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹ����)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_smallest(
        const dfs_bt_root_t & bt_root,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    //
    //  ���ܣ����B�����Ԫ�ز����drill_info
    //  �����������(B���ǿ�)����pctָ������key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹ����)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_largest(
        const dfs_bt_root_t & bt_root,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const ;
    //
    //  ���ܣ����һ����������СԪ�أ���׷��drill_info��(��searchһ��)
    //  �����������(�����ǿ�)����pctָ������key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹ��������Ϊ���ڵ�)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_subtree_smallest(
        const uint64_t subtree_root,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    //
    //  ���ܣ����һ�����������Ԫ�أ���׷��drill_info��(��searchһ��)
    //  �����������(�����ǿ�)����pctָ������key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹ��������Ϊ���ڵ�)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_subtree_largest(
        const uint64_t subtree_root,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    //
    //  ���ܣ�����drill_info��ñȵ�ǰ��С���������drill_infoָ��֮
    //        ��������pctָ��֮��key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹû�и�СԪ��)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_smaller(dfs_btree_drill_t & drill_info, uint64_t & obj_indext) const;
    //
    //  ���ܣ�����drill_info��ñȵ�ǰ�����������drill_infoָ��֮
    //        ��������pctָ��֮��key_indexΪ��ֵ������pctΪNULL��key_indexΪUNDEF_INDEX
    //  ����: �ɹ�(��ʹû�и���Ԫ��)����0, ����ֵ����(ERRNO_BT_DRILL_OBSOLETE�������µ��޸�)
    int _get_larger(dfs_btree_drill_t & drill_info, uint64_t & obj_index) const;
    //
    //  ���ܣ���øպñ�������srctС��Ԫ��
    //  ���룺srct����Ҫ��B���д���
    //  ����������СԪ�ش��ڣ�pctָ����������pctΪNULL��drill_infoָ������srct�Ľ����
    //  ���أ�0 for no error, other values for error
    int _get_smaller_by_key(
        const dfs_bt_root_t & bt_root,
        const void * pkey,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _get_smaller_by_obj(
        const dfs_bt_root_t & bt_root,
        const void * pobj,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    //  ���ܣ���øպñ������srct���Ԫ��
    //  ���룺srct����Ҫ��B���д���
    //  ���������ϴ�Ԫ�ش��ڣ�pctָ����������pctΪNULL��drill_infoָ������srct�Ľ����
    //  ���أ�0 for no error, other values for error
    int _get_larger_by_key(
        const dfs_bt_root_t & bt_root,
        const void * pkey,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _get_larger_by_obj(
        const dfs_bt_root_t & bt_root,
        const void * pobj,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
protected:
    //  ���ܣ����������������д�뵽drill_info�С�drill_info�л�������ǰ��mutation��Ϣ
    //        �����ڣ���drill_info�Լ�pct��ָ������key_indexΪ��key_index��
    //        ����drill_infoָ����������λ�ã�key_index��pct�ֱ�ΪUNDEF_INDEX��NULL��
    //  return: 0 for success, other values for error
    //  this object may or may not in the btree
    int _search_by_index(
        const dfs_bt_root_t & bt_root,
        const uint64_t i_obj_index,
        dfs_btree_drill_t & drill_info,
        uint64_t & o_obj_index) const;
    int _search_by_obj(
        const dfs_bt_root_t & bt_root,
        const void * pobj,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _search_by_key(
        const dfs_bt_root_t & bt_root,
        const void * pkey,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
protected:
    //  ���ܣ�����ĳ�����������׷�ӵ�drill_info��
    //      �����Ԫ�ش��ڣ���drill_info����Ϣָ����������ָ����������λ�á�
    //  ���أ�0 for success, other values for error
    int _search_subtree_by_index(
        const dfs_bt_root_t & bt_root,
        const uint64_t subtree_root,
        const uint64_t src_index,    //this object may or may not in the btree
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _search_subtree_by_obj(
        const dfs_bt_root_t & bt_root,
        const uint64_t subtree_root,
        const void * pobj,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _search_subtree_by_key(
        const dfs_bt_root_t & bt_root,
        const uint64_t subtree_root,
        const void * pkey,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    //  ���ܣ�����B����һ���ڵ㣬������뵽drill_info��
    //  ���أ�0 for success, other values for error
    int _search_node_by_index(
        const dfs_bt_root_t & bt_root,
        const uint64_t node_index,
        const uint64_t src_index,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _search_node_by_obj(
        const dfs_bt_root_t & bt_root,
        const uint64_t node_index,   //this object may or may not in the btree
        const void * pobj,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
    int _search_node_by_key(
        const dfs_bt_root_t & bt_root,
        const uint64_t node_index,
        const void * pkey,
        dfs_btree_drill_t & drill_info,
        uint64_t & obj_index) const;
protected:
    //return: 0 for success, other values for error
    int _get_drill_last_obj(
        const dfs_btree_drill_t & drill_info,
        uint64_t & last_obj_index) const;
protected:
    ////  Return: 0 for success, other values for error
    //virtual int vir_get_total_obj_ref_counter_in_ary(
    //        const dfs_bt_root_t & bt_root,
    //        uint64_t & total_ref_counter) const = 0;
    //  return: 0 for success, other values for error
    virtual int vir_get_total_in_use_obj_num_in_ary(uint64_t & total_in_use_obj_num) const = 0;
    //  Return: 0 for success, other values for error
    virtual int vir_obj_store_size(
        const uint64_t obj_index,
        uint64_t & store_size,
        dfs_btree_store_info_t & store_info) const = 0;
    //  return: 0 for success, other values for error
    virtual int vir_store_obj(
        const uint64_t obj_index,
        dfs_btree_store_info_t & store_info) const = 0;
    //  return: 0 for success, other values for error
    virtual int vir_load_obj(
        uint64_t & obj_index,
        dfs_btree_load_info_t & load_info) = 0;
    //  return: 0 for success, other values for error
    //virtual int vir_get_obj_ref_counter(const uint64_t obj_index, uint64_t & ref_counter) const = 0;
    virtual int vir_inc_obj_ref_counter(const uint64_t obj_index) = 0;
    virtual int vir_dec_obj_ref_counter(const uint64_t obj_index) = 0;
    virtual int vir_inc_obj_ary_ref_counter(const uint64_t * obj_index_ary, const int32_t obj_num) = 0;
    virtual int vir_dec_obj_ary_ref_counter(const uint64_t * obj_index_ary, const int32_t obj_num) = 0;
    //
    //  return: 0 for success, other values for error
    //  cmp: -1, 0, 1 for less than, equal, great than
    //virtual int vir_compare_index_index(
    //        const dfs_bt_root_t & bt_root,
    //        const uint64_t obj1_index,
    //        const uint64_t obj2_index,
    //        int & cmp) const = 0;
    virtual int vir_compare_index_obj(
        const dfs_bt_root_t & bt_root,
        const uint64_t obj1_index,
        const void * pobj2,
        int & cmp) const = 0;
    virtual int vir_compare_index_key(
        const dfs_bt_root_t & bt_root,
        const uint64_t obj1_index,
        const void * pkey2,
        int & cmp) const = 0;
    virtual int vir_search_ary_by_index(
        const dfs_bt_root_t & bt_root,
        const uint64_t * obj_index_ary,
        const int32_t obj_num,
        const uint64_t obj2_index,
        int & ins_pos,
        uint64_t & obj_index) const = 0;
    virtual int vir_search_ary_by_obj(
        const dfs_bt_root_t & bt_root,
        const uint64_t * obj_index_ary,
        const int32_t obj_num,
        const void * pobj2,
        int & ins_pos,
        uint64_t & obj_index) const = 0;
    virtual int vir_search_ary_by_key(
        const dfs_bt_root_t & bt_root,
        const uint64_t * obj_index_ary,
        const int32_t obj_num,
        const void * pkey2,
        int & ins_pos,
        uint64_t & obj_index) const = 0;
    virtual int vir_verify_index_ary(
        const dfs_bt_root_t & bt_root,
        const uint64_t * obj_index_ary,
        const int32_t obj_num,
        bool & is_inc_order) const = 0;
protected:
    //����/���ٸ��ڵ����ü���
    virtual int vir_inc_root_ref_counter(const dfs_bt_root_t & bt_root) const;
    virtual int vir_dec_root_ref_counter(const dfs_bt_root_t & bt_root) const;
private:
    //uint64_t _get_max_verified_mutaiont_counter(void) const;
    //int _set_max_verified_mutaiont_counter(const uint64_t mutaiont_counter) const;
    //  ���ܣ���֤ĳ��������˳���ÿ���ڵ��֧����
    //  ���룺subtree_root���������ڵ�
    //      left_brother_key_index�����������ֵܣ����������UNDEF_INDEX����<=������������
    //      right_brother_key_index�����������ֵܣ����������UNDEF_INDEX����>=������������
    //  ���أ�����0�����֤ͨ�������ط�0(�������)�����֤ʧ��
    int _verify_subtree(
        const dfs_bt_root_t & bt_root,
        const uint64_t father_mutation_counter,
        const uint64_t subtree_root,
        const uint64_t left_brother_obj_index,
        const uint64_t right_brother_obj_index) const;
public:
    //  ���ܣ���֤��������˳���ÿ���ڵ��֧����
    //  ���أ�����0�����֤ͨ�������ط�0(�������)�����֤ʧ��
    int bt_verify_tree(const dfs_bt_root_t & bt_root) const;
private:
    //  return: 0 for success, other values for error
    int _get_subtree_total_num(
        const uint64_t subtree_root,
        uint64_t & total_key_num,
        uint64_t & total_leaf_node_num,
        uint64_t & total_mid_node_num) const;
public:
    //  return: 0 for success, other values for error
    int bt_get_total_num(
        const dfs_bt_root_t & bt_root,
        uint64_t & total_key_num,
        uint64_t & total_leaf_node_num,
        uint64_t & total_mid_node_num) const;
    //  return: 0 for success, other values for error
    int bt_get_in_use_node_num_in_2d_ary(
        uint64_t & in_use_leaf_node_num,
        uint64_t & in_use_mid_node_num) const;
protected:
    //  return: 0 for success, other values for error
    //  Should be called in locked state and without checkpointing or other COW(load or store)
    int _bt_sanitary_check(const dfs_bt_root_t & bt_root) const;
protected:
    void _bt_clear_statistic_info(void) {
        leaf_ary_t::clear_statistic_info();
        mid_ary_t::clear_statistic_info();
        return;
    };
    void _bt_log_statistic_info(
        const int log_level,
        const char * filename,
        const int lineno,
        const char * funcname,
        const char * btreename) const {
        leaf_ary_t::log_statistic_info(log_level, filename, lineno, funcname, btreename, "leaf");
        mid_ary_t::log_statistic_info(log_level, filename, lineno, funcname, btreename, "mid");
        return;
    };
    void _bt_log_debug_info(
        const int log_level,
        const char * filename,
        const int lineno,
        const char * funcname,
        const char * btreename) const {
        DF_WRITE_LOG_US(log_level, "%s,%d,%s", filename, lineno, funcname);
        DF_WRITE_LOG_US(log_level, "%s", btreename);
        DF_WRITE_LOG_US(log_level, "mutation_counter=%ld", _bt_get_mutation_counter());
        DF_WRITE_LOG_US(log_level, "max_cow_mutaiont_counter=%ld", _bt_get_max_cow_mutation_counter());
    };
    uint64_t _bt_get_mem_size(void) const {
        return (leaf_ary_t::get_mem_size()+mid_ary_t::get_mem_size());
    };
};

inline uint32_t dfs_btree_t::_bt_get_instance_pos(void) const
{
    uint32_t bt_instance_pos = _bt_instance_pos;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (bt_instance_pos >= MAX_BT_INSTANCE_NUM) {
        bt_instance_pos = DEF_BT_INSTANCE_POS;
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "bt_instance_pos >= MAX_BT_INSTANCE_NUM");
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_BUF_POS);
    }

    return bt_instance_pos;
};

//inline int dfs_btree_t::_bt_set_bt_instance_pos(const uint32_t bt_instance_pos)
//{
//    int log_level = DF_UL_LOG_NONE;
//    int err = 0;
//
//    if (UNDEF_POS != _bt_instance_pos)
//    {
//        log_level = DF_UL_LOG_FATAL;
//        DF_WRITE_LOG_US(log_level, "UNDEF_POS != _bt_instance_pos");
//        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_BUF_POS);
//    }
//    else if (bt_instance_pos >= MAX_BT_INSTANCE_NUM)
//    {
//        log_level = DF_UL_LOG_FATAL;
//        DF_WRITE_LOG_US(log_level, "bt_instance_pos >= MAX_BT_INSTANCE_NUM");
//        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_BUF_POS);
//    }
//    else
//    {
//        _bt_instance_pos = bt_instance_pos;
//    }
//
//    return err;
//};

inline const dfs_sbt_root_t & dfs_btree_t::_bt_get_sbt_root(const dfs_bt_root_t & bt_root) const
{
    return bt_root.get_root(_bt_get_instance_pos());
};

inline int dfs_btree_t::_bt_set_sbt_root(
    dfs_bt_root_t & bt_root,
    const dfs_sbt_root_t & sbt_root) const
{
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if ((err = bt_root.set_root(sbt_root, _bt_get_instance_pos())) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "bt_root.set_root() returns 0x%x", err);
    }

    return err;
};



//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_get_leaf_node_for_mutate(const uint64_t final_node_index, leaf_node_t ** ppleaf) const
{
    leaf_node_t * pleaf = NULL;
    const uint64_t inter_node_index = _remove_node_flag_bit(final_node_index);
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL != ppleaf) {
        *ppleaf = NULL;
    }

    if ((err = leaf_ary_t::get_exist_t_unit(inter_node_index, pleaf)) != 0 || NULL == pleaf) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "leaf_ary_t::get_exist_t_unit() returns 0x%x", err);
        DF_WRITE_LOG_US(log_level,
                        "inter_node_index=0x%lx, final_node_index=0x%lx, pleaf=0x%p",
                        inter_node_index, final_node_index, pleaf);
        err = DF_BT_SET_NORMAL_ERR(ERRNO_BT_INVALID_INDEX);
    } else {
        if (NULL != ppleaf) {
            *ppleaf = pleaf;
        }
    }

    return err;
};

//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_get_mid_node_for_mutate(const uint64_t final_node_index, mid_node_t ** ppmid) const
{
    mid_node_t * pmid = NULL;
    const uint64_t inter_node_index = _remove_node_flag_bit(final_node_index);
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL != ppmid) {
        *ppmid = NULL;
    }

    if ((err = mid_ary_t::get_exist_t_unit(inter_node_index, pmid)) != 0 || NULL == pmid) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "mid_ary_t::get_exist_t_unit() returns 0x%x", err);
        DF_WRITE_LOG_US(log_level,
                        "inter_node_index=0x%lx, final_node_index=0x%lx, pmid=0x%p",
                        inter_node_index, final_node_index, pmid);
        err = DF_BT_SET_NORMAL_ERR(ERRNO_BT_INVALID_INDEX);
    } else {
        if (NULL != ppmid) {
            *ppmid = pmid;
        }
    }

    return err;
};


//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_get_leaf_node(const uint64_t final_node_index, const leaf_node_t ** ppleaf) const
{
    leaf_node_t * pleaf = NULL;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if ((err = _get_leaf_node_for_mutate(final_node_index, &pleaf)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "_get_leaf_node_for_mutate() returns 0x%x", err);
        DF_WRITE_LOG_US(log_level, "final_node_index=0x%lx", final_node_index);
        if (NULL != ppleaf) {
            *ppleaf = NULL;
        }
    } else {
        if (NULL != ppleaf) {
            *ppleaf = pleaf;
        }
    }

    return err;
};
//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_get_mid_node(const uint64_t final_node_index, const mid_node_t ** ppmid) const
{
    mid_node_t * pmid = NULL;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if ((err = _get_mid_node_for_mutate(final_node_index, &pmid)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "_get_mid_node_for_mutate() returns 0x%x", err);
        DF_WRITE_LOG_US(log_level, "final_node_index=0x%lx", final_node_index);
        if (NULL != ppmid) {
            *ppmid = NULL;
        }
    } else {
        if (NULL != ppmid) {
            *ppmid = pmid;
        }
    }

    return err;
};
//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_get_node(const uint64_t final_node_index, const node_base_t ** ppbase) const
{
    const leaf_node_t * pleaf = NULL;
    const mid_node_t * pmid = NULL;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL != ppbase) {
        *ppbase = NULL;
    }

    if (_is_mid_node(final_node_index)) {
        if ((err = _get_mid_node(final_node_index, &pmid)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_get_mid_node() returns 0x%x", err);
            DF_WRITE_LOG_US(log_level, "final_node_index=0x%lx", final_node_index);
        } else if (NULL != ppbase) {
            *ppbase = (node_base_t *)pmid;
        }
    } else {
        if ((err = _get_leaf_node(final_node_index, &pleaf)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_get_leaf_node() returns 0x%x", err);
            DF_WRITE_LOG_US(log_level, "final_node_index=0x%lx", final_node_index);
        } else if (NULL != ppbase) {
            *ppbase = (node_base_t *)pleaf;
        }
    }

    return err;
};

//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_valid_leaf_node_index_verify(const uint64_t final_node_index) const
{
    const uint64_t inter_node_index = _remove_node_flag_bit(final_node_index);
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (!_is_leaf_node(final_node_index)) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "Invalid leaf node index");
    } else if ((err = leaf_ary_t::valid_t_index_verify(inter_node_index)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "leaf_ary_t::valid_t_index_verify() returns 0x%x", err);
    }

    if (DF_UL_LOG_NONE != log_level) {
        DF_WRITE_LOG_US(log_level,
                        "final_node_index=0x%lx, inter_node_index=0x%lx",
                        final_node_index, inter_node_index);
    }

    return err;
};
//return: 0 for success(be a valid index), other value for error
inline int dfs_btree_t::_valid_mid_node_index_verify(const uint64_t final_node_index) const
{
    const uint64_t inter_node_index = _remove_node_flag_bit(final_node_index);
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (!_is_mid_node(final_node_index)) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "Invalid leaf node index");
    } else if ((err = mid_ary_t::valid_t_index_verify(inter_node_index)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "mid_ary_t::valid_t_index_verify() returns 0x%x", err);
    }

    if (DF_UL_LOG_NONE != log_level) {
        DF_WRITE_LOG_US(log_level,
                        "final_node_index=0x%lx, inter_node_index=0x%lx",
                        final_node_index, inter_node_index);
    }

    return err;
};

//���ܣ����node(leaf or mid)��pos����key_index
//Return: 0 for success, other value for error
inline int dfs_btree_t::_get_key_of_node(const uint64_t node_index, const uint32_t pos, uint64_t & key_index) const
{
    const node_base_t * pnode = NULL;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    key_index = UNDEF_INDEX;
    if ((err = _get_node(node_index, &pnode)) != 0 || NULL == pnode) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "_get_node() returns 0x%x", err);
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_NULL_POINTER);
    } else if (pos >= pnode->get_subkey_num()) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pos >= pnode->get_subkey_num()");
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
    } else {
        key_index = pnode->get_subkey_index(pos);
    }

    if (DF_UL_LOG_NONE != log_level) {
        DF_WRITE_LOG_US(log_level,
                        "node_index=0x%lx, pos=%d",
                        node_index, pos);
    }

    return err;
};
//���ܣ����mid node��pos����subnode
//Return: 0 for success, other value for error
inline int dfs_btree_t::_get_subnode_of_mid_node(const uint64_t node_index, const uint32_t pos, uint64_t & subnode_index) const
{
    const mid_node_t * pmid_node = NULL;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    subnode_index = UNDEF_INDEX;
    if ((err = _get_mid_node(node_index, &pmid_node)) != 0 || NULL == pmid_node) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "_get_mid_node() returns 0x%x", err);
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_MID_NODE_NULL_POINTER);
    } else if (pos > pmid_node->get_subkey_num()) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pos > pmid_node->get_subkey_num()");
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_NODE_POS);
    } else {
        subnode_index = pmid_node->get_subnode_index(pos);
    }

    if (DF_UL_LOG_NONE != log_level) {
        DF_WRITE_LOG_US(log_level,
                        "node_index=0x%lx, pos=%d",
                        node_index, pos);
    }

    return err;
};



//inline uint64_t dfs_btree_t::_bt_inc_mutation_counter(void)
//{
//    return atomic_inc(&_mutation_counter);
//};

inline uint64_t dfs_btree_t::_bt_update_mutation_counter(const bool is_batch_mode)
{
    atomic_inc(&_mutation_counter);
    if (!is_batch_mode) {
        //TODO..???
        atomic_exchange(&_max_cow_mutation_counter, _mutation_counter);
    }

    return _mutation_counter;
}

inline uint64_t dfs_btree_t::_bt_get_mutation_counter(void) const
{
    return _mutation_counter;
};

inline uint64_t dfs_btree_t::_bt_get_max_cow_mutation_counter(void) const
{
    return _max_cow_mutation_counter;
};

inline void dfs_btree_t::_bt_update_max_cow_mutation_counter(void) const
{
    atomic_exchange(&(((dfs_btree_t *)this)->_max_cow_mutation_counter), _mutation_counter);
    return;
};

inline bool dfs_btree_t::_bt_is_cancel_checkpointing(void) const
{
    return (_is_cancel_checkpointing != 0);
};

//return previous state
inline bool dfs_btree_t::_bt_set_cancel_checkpointing(const bool is_cancel)
{
    uint32_t pre_state = atomic_exchange(&_is_cancel_checkpointing, (is_cancel) ? 1 : 0);

    return (pre_state != 0);
};


//not necessary in locked state
//_root_node_index might be UNDEF_INDEX
inline int dfs_btree_t::_bt_inc_root_ref_counter(const dfs_bt_root_t & bt_root) const
{
    const dfs_sbt_root_t & sbt_root = _bt_get_sbt_root(bt_root);
    const uint64_t root_index = sbt_root.get_root_node_index();
    uint64_t ref_counter = UNDEF_REF_COUNTER;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (UNDEF_INDEX != root_index) {
        if ((err = ((dfs_btree_t *)this)->_inc_node_ref_counter(root_index, ref_counter)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_inc_node_ref_counter() returns 0x%x", err);
        }
    }

    return err;
};
inline int dfs_btree_t::_bt_dec_root_ref_counter(const dfs_bt_root_t & bt_root) const
{
    const dfs_sbt_root_t & sbt_root = _bt_get_sbt_root(bt_root);
    const uint64_t root_index = sbt_root.get_root_node_index();
    uint64_t ref_counter = UNDEF_REF_COUNTER;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (UNDEF_INDEX != root_index) {
        if ((err = ((dfs_btree_t *)this)->_dec_node_ref_counter(root_index, ref_counter)) != 0) {
            log_level = DF_UL_LOG_FATAL;
            DF_WRITE_LOG_US(log_level, "_dec_node_ref_counter() returns 0x%x", err);
        }
    }

    return err;
};


////for mutation operation
//inline int dfs_btree_t::_bt_copy_root(dfs_cell_root_t & bt_root) const
//{
//    _bt_set_own_root_info(bt_root, _root_info);
//    return 0;
//};
//inline int dfs_btree_t::_bt_update_root(
//            dfs_cell_root_t & cur_root_info_ary,
//            const dfs_cell_root_t & new_root_info_ary)
//{
//    const dfs_root_info_t & new_root_info = _bt_get_own_root_info(new_root_info_ary);
//    const uint64_t new_root_index = new_root_info.get_root_node_index();
//    uint64_t old_root_index = _root_info.get_root_node_index();
//    uint64_t tmp_root_index = UNDEF_INDEX;
//    int log_level = DF_UL_LOG_NONE;
//    int err = 0;
//
//    if (UNDEF_INDEX == new_root_index)
//    {
//        log_level = DF_UL_LOG_FATAL;
//        DF_WRITE_LOG_US(log_level, "UNDEF_INDEX == new_root_info.get_root_node_index()");
//        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_INVALID_INDEX);
//    }
//    else
//    {
//        _root_info.set_next_allocate_id(new_root_info.get_next_allocate_id());
//        _root_info.set_root_node_index(old_root_index, new_root_index);
//    }
//    _bt_set_own_root_node_index(cur_root_info_ary, tmp_root_index, old_root_index);
//
//    if (DF_UL_LOG_NONE != log_level)
//    {
//        DF_WRITE_LOG_US(log_level,
//                "old_root_index=0x%lx, new_root_index=0x%lx",
//                old_root_index, new_root_index);
//    }
//
//    return err;
//};



//����֮ǰ�Ķ���...
template<uint32_t FANOUT>
inline int dfs_btree_leaf_node_t<FANOUT>::action_before_gc(void * pgc_info, const uint64_t node_index)
{
    dfs_btree_t * pbtree = (dfs_btree_t *)pgc_info;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL == pbtree) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "NULL == pbtree");
        err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_SET_GC_INFO);
    } else if((err = pbtree->action_before_leaf_node_gc(node_index)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pbtree->action_before_leaf_node_gc() returns 0x%x", err);
    }

    return err;
};

template<uint32_t FANOUT>
inline int dfs_btree_mid_node_t<FANOUT>::action_before_gc(void * pgc_info, const uint64_t node_index)
{
    dfs_btree_t * pbtree = (dfs_btree_t *)pgc_info;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL == pbtree) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "NULL == pbtree");
        err = DF_BT_SET_INTERNAL_ERR(dfs_bt_const_t::ERRNO_BT_SET_GC_INFO);
    } else if((err = pbtree->action_before_mid_node_gc(node_index)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pbtree->action_before_mid_node_gc() returns 0x%x", err);
    }

    return err;
};

//����ǰ�����Ĳ���
inline int dfs_bt_root_t::action_while_added(void * pgc_info, const uint64_t node_index)
{
    dfs_btree_t * pbtree = (dfs_btree_t *)pgc_info;
    //uint64_t ref_counter = 0;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL == pbtree) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "NULL == pbtree");
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_SET_GC_INFO);
        //���Ӹ��Ĳ���.....
    } else if ((err = pbtree->vir_inc_root_ref_counter(*this)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pbtree->vir_inc_root_ref_counter() returns 0x%x", err);
    }

    return err;
};
//ɾ��ǰ�����Ĳ���
//���ڵ�ɾ��֮ǰ...
inline int dfs_bt_root_t::action_before_gc(void * pgc_info, const uint64_t node_index)
{
    dfs_btree_t * pbtree = (dfs_btree_t *)pgc_info;
    //uint64_t ref_counter = 0;
    int log_level = DF_UL_LOG_NONE;
    int err = 0;

    if (NULL == pbtree) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "NULL == pbtree");
        err = DF_BT_SET_INTERNAL_ERR(ERRNO_BT_SET_GC_INFO);
    } else if ((err = pbtree->vir_dec_root_ref_counter(*this)) != 0) {
        log_level = DF_UL_LOG_FATAL;
        DF_WRITE_LOG_US(log_level, "pbtree->vir_dec_root_ref_counter() returns 0x%x", err);
    }

    return err;
};



class dfs_btree_lock_hold_t : public dfs_mutex_hold_t
{
};

class dfs_btree_lock_t : public dfs_mutex_lock_t
{
public:
    int init(void) const {
        return 0;
    };
//protected:
//    inline int _acquire_mutate_lock(
//        dfs_btree_lock_hold_t & lock_hold,
//        const int64_t wait_microsecond = 0) const;
//    inline int _release_mutate_lock(dfs_btree_lock_hold_t & lock_hold) const;
//    inline int _verify_mutate_locker(const dfs_btree_lock_hold_t & lock_hold) const;
public:
    inline int acquire_mutate_lock(
        dfs_btree_lock_hold_t & lock_hold,
        const int64_t wait_microsecond = 0) const {
        return dfs_mutex_lock_t::_acquire_lock(lock_hold, wait_microsecond);
    };
    inline int release_mutate_lock(dfs_btree_lock_hold_t & lock_hold) const {
        return dfs_mutex_lock_t::_release_lock(lock_hold);
    };
    inline bool verify_lock_hold(const dfs_btree_lock_hold_t & lock_hold) const {
        //???
        return _verify_hold(lock_hold);
    };
////Begin: Obsolete interface:
//protected:
//    dfs_btree_lock_hold_t _lock_hold;
//    inline int _acquire_mutate_lock(const int64_t wait_microsecond = 0) const
//    {
//        return acquire_mutate_lock((dfs_btree_lock_hold_t &)_lock_hold, wait_microsecond);
//    };
//    inline int _release_mutate_lock(void) const
//    {
//        return release_mutate_lock((dfs_btree_lock_hold_t &)_lock_hold);
//    };
////End: Obsolete interface:
};


#define BT_MUTATE_LOCK_ACQUIRE(hold_ptr, hold_inst, max_time_in_us)  \
    ((NULL == hold_ptr) ? this->acquire_mutate_lock(hold_inst, max_time_in_us) : \
        this->_acquire_hold(*hold_ptr))

#define BT_MUTATE_LOCK_RELEASE(hold_ptr, hold_inst)   \
    ((NULL == hold_ptr) ? this->release_mutate_lock(hold_inst) : \
        this->_release_hold(*hold_ptr))

#define BT_MUTATE_LOCK_ACQUIRE_ERR_INFO  "BT_MUTATE_LOCK_ACQUIRE() returns error"

#define BT_MUTATE_LOCK_RELEASE_ERR_INFO  "BT_MUTATE_LOCK_RELEASE() returns error"

#endif //__DFS_BTREE_INCLUDE_H_

