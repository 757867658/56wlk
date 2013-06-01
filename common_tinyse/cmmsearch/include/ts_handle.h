/***************************************************************************
 * 
 * Copyright (c) 2008 Baidu.com, Inc. All Rights Reserved
 * $Id: ts_handle.h,v 1.4 2009/11/18 20:27:50 yanghy2 Exp $ 
 * 
 **************************************************************************/



/**
 * @file tinyse.h
 * @author yanghy(yanghuaiyuan@baidu.com)
 * @date 2008/04/08 11:38:52
 * @version $Revision: 1.4 $ 
 * @brief 
 *  
 **/


#ifndef  __TS_HANDLE_H_
#define  __TS_HANDLE_H_

class ts_query_handle_t
{
  public:
    /**
	 * @brief ������ѯ����
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [out] term_list   : vector<ts_terminfo_t>&
	 * @return  int 
	 * @retval   
	**/
    virtual int parse_query(const pointer_t query_cmd,
                            vector < ts_terminfo_t > &term_list) = 0;

    /** 
     * @brief �����ϲ��㷨
     *
     * @param [in] query_cmd   : const pointer_t
     * @param [in] term_list   : vector < ts_terminfo_t >&
     * @param [in] ind_reader   : ts_ind_reader_t&
     * @param [out] merged_list   : vector<ts_index_t>*
     * @return  int 
     * @retval   
    **/
    virtual int merge_ind_list(const pointer_t query_cmd,
                                vector < ts_terminfo_t > &term_list,
                                ts_ind_reader_t &ind_reader,
                                vector<ts_index_t> *merged_list) = 0;


    /**
	 * @brief ��������
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [in] merged_list   : vector<ts_index_t>*
	 * @param [out] filted_list   : vector<ts_index_t>*
	 * @return  int 
	 * @retval   
	**/
    virtual int index_filt(const pointer_t query_cmd,
                           vector < ts_index_t > *merged_list,
                           vector < ts_index_t > *filted_list) = 0;


    /**
	 * @brief ��Ȩ
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [in] term_list   : const vector<ts_terminfo_t>&
	 * @param [in/out] filted_list   : vector<ts_index_t>*
	 * @return  int 
	 * @retval   
	**/
    virtual int adjust_weight(const pointer_t query_cmd,
                              const vector < ts_terminfo_t > &term_list,
                              vector < ts_index_t > *filted_list) = 0;


	/**
	 * @brief ��ʼ�����buffer
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [in/out] result   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
	virtual int init_abs(const pointer_t query_cmd,ts_buffer_t &result) = 0;
    /**
	 * @brief ���㲢���ժҪ�����buffer�� 
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [in] i   : int
	 * @param [in] fts   : const ts_buffer_t&
	 * @param [in] brs   : const ts_buffer_t&
	 * @param [out] result   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
    virtual int add_abs(const pointer_t query_cmd,
                        int i,
                        const ts_buffer_t & fts,
                        const ts_buffer_t & brs, ts_buffer_t & result) = 0;

	/**
	 * @brief finish���buffer
	 *
	 * @param [in] query_cmd   : const pointer_t
	 * @param [in/out] result   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
	virtual int fini_abs(const pointer_t query_cmd,ts_buffer_t &result) = 0;

  private:
};

class ts_update_handle_t
{
  public:

    /**
	 * @brief �����������
	 *
	 * @param [in] update_cmd   : pointer_t
	 * @param [out] id   : uint32&
	 * @param [out] termlist   : vector<ts_terminfo_t>&
	 * @param [out] brief   : ts_buffer_t&
	 * @param [out] fulltext   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
    virtual int parse_add(pointer_t update_cmd,
                          uint32 & id,
                          vector < ts_terminfo_t > &termlist,
                          ts_buffer_t & brief, ts_buffer_t & fulltext) = 0;


    /**
	 * @brief �����޸�����
	 *
	 * @param [in] update_cmd   : pointer_t
	 * @param [out] id   : uint32&
	 * @param [out] termlist   : vector<ts_terminfo_t>&
	 * @param [out] brief   : ts_buffer_t&
	 * @param [out] fulltext   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
    virtual int parse_mod(pointer_t update_cmd,
                          uint32 & id,
                          vector < ts_terminfo_t > &termlist,
                          ts_buffer_t & brief, ts_buffer_t & fulltext) = 0;

    /**
	 * @brief 
	 *
	 * @param [in] update_cmd   : pointer_t
	 * @param [out] id   : uint32&
	 * @param [out] brief   : ts_buffer_t&
	 * @return  int 
	 * @retval   
	**/
    virtual int parse_modbasic(pointer_t update_cmd,
                               uint32 & id, ts_buffer_t & brief) = 0;

    /**
	 * @brief ����ɾ������
	 *
	 * @param [in] update_cmd   : pointer_t 
	 * @param [out] id_list   : vector<uint32>&
	 * @return  int 
	 * @retval   =0 �ɹ� ; <0 ʧ��
	**/
    virtual int parse_del(pointer_t update_cmd,
                          vector < uint32 > &id_list) = 0;


    /**
	 * @brief ������ɾ������
	 *
	 * @param [in] update_cmd   : pointer_t
	 * @param [out] id_list   : vector<uint32>&
	 * @return  int 
	 * @retval  =0 �ɹ� ; <0 ʧ��
	**/
    virtual int parse_undel(pointer_t update_cmd,
                            vector < uint32 > &id_list) = 0;

  private:
};




/**
* @brief ���ο������
*/
typedef struct ts_handle_t
{
    ts_init_func_t global_init;               /**< ȫ�ֳ�ʼ������       */
    ts_init_func_t query_thread_init;         /**< ��ѯ�̳߳�ʼ������       */
    ts_init_func_t update_thread_init;        /**< �����̳߳�ʼ������       */
    ts_init_func_t ontime_session_proc;       /**< ��ʱ�̴߳�����       */

    ts_query_handle_t *query_handle;          /**< ��ѯ���       */
    ts_update_handle_t *update_handle;        /**< ���¾��       */
} ts_handle_t;


#endif //__TS_HANDLE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
