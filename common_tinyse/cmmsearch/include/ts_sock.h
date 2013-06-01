/***************************************************************************
 * 
 * Copyright (c) 2008 Baidu.com, Inc. All Rights Reserved
 * $Id: ts_sock.h,v 1.1 2008/08/22 04:04:06 yanghy2 Exp $ 
 * 
 **************************************************************************/



/**
 * @file tinyse_interface.h
 * @author yanghy(yanghuaiyuan@baidu.com)
 * @date 2008/06/20 09:56:28
 * @version $Revision: 1.1 $ 
 * @brief socket�ӿ�
 *  
 **/


#ifndef  __TS_SOCK_H_
#define  __TS_SOCK_H_



/**
* @brief ��ѯ�������
*/
enum _ts_query_cmd_t
{
    qcAll = 100001,     ///< ����ȫ������
    qcDel = 100002,     ///< ������ɾ������
    qcUndel = 100003,   ///< ����δɾ������

};
/**
* @brief �ύ�������
*/
enum _ts_commit_cmd_t
{
    cmDataAdd = 10001,      ///< �������
    cmDataMod = 10002,      ///< �޸�����
    cmDataModBasic = 10003, ///< ���޸Ļ������ݣ���Ӱ��������di
    cmDataDel = 10004,      ///< ����ɾ��
    cmDataUndel = 10005,    ///< ������ɾ��
    cmDataRebuild = 10006   ///< ��������طŵ��ؽ�����������
};

/**
* @brief �����
*/
enum _ts_errinfo_t
{
    SUCCESS = 0,        ///< �ɹ�
    errNet = 10,        ///< ����io����
    errCmdno = 11,      ///< ����Ŵ���
    errIllegalReq = 12, ///< ����Ľṹ������������
    errConnect = 13,    ///< ���ӷ������
    errRWTimeout = 14,  ///< ��д���ݳ�ʱ
    errNoPower = 15,    ///< ��Ȩ��
    errNoExist = 16,    ///< ���ݲ����� 
    errDataExist = 17,  ///< �������Ѵ���
    errDataType = 18,   ///< ���������Ͳ���ȷ
    errDataWrong = 19,  ///< �������ݴ�������: �ظ�/ɾ��/������/����/��Χ����..
    errInternal = 20    ///< �ڲ��������ļ�io����.(��/д)
};




/**
* @brief tinyseͳһ������ӿ�
*   
*   +-------+------------------------+-------------------+
*   |nshead | �������� ts_head_t     |  �䳤����mcpack   |
*   +-------+------------------------+-------------------+
*/
typedef struct ts_head_t
{
    uint32 cmd_no;                         /**< �����     */
    uint32 err_no;                         /**< �����     */
    char data_type[TS_TYPENAME_MAXLEN];    /**< ��������      */

    uint32 page_no;                        /**< ҳ��      */
    uint32 num_per_page;                   /**< ÿҳ��ʾ����       */

    uint32 ret_num;                        /**< ���ظ���      */
    uint32 disp_num;                       /**< �ܹ���ѯ���ĸ���      */


} ts_head_t;










#endif //__TS_SOCK_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
