/////////////////////////////////////////////////////////////////////////////

#ifndef __UB_LOG_H__
#define __UB_LOG_H__

////////////////////////////////////////////////////////////////
//////// Module:  ub_log, log related module of general framework
///////  Comment: standlization to log format will benefit us all
///////
////////////////////////////////////////////////////////////////

#include <pthread.h>
#include "ul_log.h"


/**********************************************************************/
/***************general framework notice function suggestment**********/
//ͨ�ÿ�ܹ涨��notice��־�е�һ����ʽ��
//[logid:834798723][proctime:23ms][reqip:127.0.32.123][reqserver:nstrans]
//[cmdno:3(update)][svrname:di][errno:-3(readiheadfaild)][ϵͳһ��Ԥ����Ϣ]
//[userinfo:{push����Ϣ}][�û�һ����Ϣ]
/**********************************************************************/
/**********************************************************************/


//����userinfo����
#define NOTICE_INFO_MAXLEN LOG_BUFF_SIZE_EX

//���ⲻ�ɼ��ķָ���
#define UB_SPACE  '\10'

//��һ����Ϣ������
typedef enum ub_notice_type{
    UB_LOG_ZERO = 0,		/**< this one is not used, and should be the first*/
    UB_LOG_LOGID,		/**< ����logid��Ӧ������*/
    UB_LOG_PROCTIME,	/**< ���ô���ʱ���Ӧ������*/
    UB_LOG_REQIP,		/**< ��������IP��Ӧ������*/
    UB_LOG_REQSVR,		/**< ����������������ƶ�Ӧ������*/
    UB_LOG_SVRNAME,		/**< ���õ�ǰ��������*/
    UB_LOG_CMDNO,		/**< �������������Ӧ������*/
    UB_LOG_ERRNO,		/**< ���ô�������Ӧ������*/
    UB_LOG_END		    /**<this one should be the last*/
};


/**
 * @brief ��ʼ������
 *     Ӧ���ڴ�LOG֮ǰ����,��ul_log��û��open�޹�,�����ڳ����ʼ����
 *
 * @param[in] log_file ��־�ļ����ļ���
 *
 * @param[in] log_path ��־���ڵ�Ŀ¼(���������Ŀ¼)
 *
 * @param[in] max_size ��־�ļ�����󳤶�
 *
 * @param[in] log_level��־�ĵȼ���(��Ϊ0,1,2,4,8,16�����ȼ�)
 *
 * @param[in] to_syslog  interesting events to send to syslog
 *
 * @param[in] assorted switches
 *
 * @return
 *       0��ʾ�ɹ�
 *		-1������־ʧ��
**/
int ub_log_init(const char* log_path = "./log", const char* log_file = "ub_log.",
	int max_size = 1000, int log_level = 16, int to_syslog = 0, int spec = 0);


/**
 * @brief �߳��г�ʼ������ʱ���ã������̴߳�ӡ��־
 *       �����̺߳����
 *
 * @param[in] thread_name �̵߳���Ϣ
 *
 * @return
 *		0��ʾ�ɹ�
 *		-1������־ʧ��
 */
int ub_log_initthread(const char* thread_name);


/**
 * @brief �������ʱ���ã������ر���־����
 */
void ub_log_close();


/**
 * @brief �������ʱ���ã������ر���־����
 */
void ub_log_closethread();


/**
 * @brief �����߳�notice��־ջ��Ϣ, ��󳤶�Ϊ NOTICE_INFO_MAXLEN
 * ����ͨ�ÿ�ܹ淶����һ������־ΪRD��ʹ�ã���ΪPEͳ��ʹ��
 *
 * @param[in] key key info of key:value
 *
 * @param[in] valuefmt format of info
 *
 * @return
 *	  length of truely added info
 */
unsigned int ub_log_pushnotice(const char* key, const char* valuefmt, ...);


/**
 * @brief ���û���notice ��Ϣ����Щ����ʹ��ub_log�������ı�����Ŀ
 *
 * @param[in] type basic infomation type
 *
 * @param[in] valuefmt format of info
 *
 * @return	number of chars been added
 */
unsigned int ub_log_setbasic(ub_notice_type type, const char* valuefmt, ...);


/**
 * @brief ȡ�û���notice����Ϣ
 *
 * @param type serven type basic infomation type
 *
 * @return
 *		string info
 *
 * @comment for ub_log_use
 */
char* ub_log_getbasic(ub_notice_type type);


/**
 * @brief set logid
 *
 * @param logid logid of network
 *
 * @return
 *		logid that set
 */
unsigned int ub_log_setlogid(unsigned int logid);


/**
 * @brief get logid
 *
 * @param logid logid of network
 *
 * @return
 *		logid that get
 */
unsigned int ub_log_getlogid();


/**
 * @brief clear logid
 *
 * @param logid logid of network
 *
 * @return
 *		logid that clear
 */
unsigned int ub_log_clearlogid();


/**
 * @brief ����push�������Ϣ
 *
 * @return
 *		string of user puted
 *
 * @comment for ub_log_use
 */
char* ub_log_popnotice();


/**
 * @brief ����߳���־�������:��ÿ�δ�notice�󣬿���Լ�����
 *
 * @return
 *        0��ʾ�ɹ�
 *	  ������ʾʧ��
 *
 * @comment for ub_log_use
 **/
int ub_log_clearnotice();


/**
 * @brief �õ��̵߳Ĵ���ʱ��, ������UB_LOG_DEBUG�д�ӡ�����ϴδ�ӡ��ʱ��
 *
 * @return
 *	�����Դ��ϴε�������ִ�е�ʱ�䣨΢��)
 *
 * @comment for ub_log_use
 */
unsigned int ub_log_getussecond();


/**
 * @brief �õ��û��������Ϣ, ��ӡnoticeʱub_log����
 *
 * @return
 *        point to the string in the stack
 *
 * @comment for ub_log_use
 */
//char* ub_log_getnoticeadd();


#define UB_LOG_MONITOR(fmt, arg...)  \
    ul_writelog(UL_LOG_WARNING, "---LOG_MONITOR--- [ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_REQIP), UB_SPACE,\
			__FILE__, __LINE__, ## arg)

#define UB_LOG_FATAL(fmt, arg...) do { \
    ul_writelog(UL_LOG_WARNING, "---LOG_MONITOR--- [ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_REQIP), UB_SPACE,\
			__FILE__, __LINE__, ## arg); \
    ul_writelog(UL_LOG_FATAL, "[ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_REQIP), UB_SPACE,\
			__FILE__, __LINE__, ## arg); \
} while (0)

#define UB_LOG_WARNING(fmt, arg...)  \
    ul_writelog(UL_LOG_WARNING, "[ %clogid:%s %c][ %creqip:%s %c][%s:%d]" fmt,\
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_REQIP), UB_SPACE,\
			__FILE__, __LINE__, ## arg)

#define UB_LOG_NOTICE(fmt, arg...) do { \
    ul_writelog(UL_LOG_NOTICE, "[ %clogid:%s %c]" "[ %cproctime:%s %c]" "[ %creqip:%s %c]" "[ %creqsvr:%s %c]"\
	    "[ %ccmdno:%s %c]" "[ %csvrname:%s %c]" "[ %cerrno:%s %c]" "[ %c%s %c]" "[ %c%s %c]" "[ %c" fmt " %c]", \
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_PROCTIME), UB_SPACE, \
	    UB_SPACE, ub_log_getbasic(UB_LOG_REQIP), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_REQSVR), UB_SPACE, \
	    UB_SPACE, ub_log_getbasic(UB_LOG_CMDNO), UB_SPACE, UB_SPACE, ub_log_getbasic(UB_LOG_SVRNAME), UB_SPACE, \
		UB_SPACE, ub_log_getbasic(UB_LOG_ERRNO), UB_SPACE, UB_SPACE, "", UB_SPACE, \
		UB_SPACE, ub_log_popnotice(), UB_SPACE, UB_SPACE, ## arg, UB_SPACE);\
	    ub_log_clearnotice(); \
} while (0)

#define UB_LOG_TRACE(fmt, arg...)  \
    ul_writelog(UL_LOG_TRACE, "[ %clogid:%s %c]" fmt,\
	   UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, ## arg)

#ifndef NO_UB_DEBUG
#define UB_LOG_DEBUG(fmt, arg...) \
    ul_writelog(UL_LOG_DEBUG, "[ %clogid:%s %c][%s:%d][time:%uus]" fmt,\
	    UB_SPACE, ub_log_getbasic(UB_LOG_LOGID), UB_SPACE, __FILE__, __LINE__,\
	    ub_log_getussecond(), ## arg)
#else
#define UB_LOG_DEBUG(fmt, arg...) \
    do {} while(0)
#endif


#endif //__UB_LOG_H__

/* vim: set ts=4 sw=4 tw=0 noet: */


