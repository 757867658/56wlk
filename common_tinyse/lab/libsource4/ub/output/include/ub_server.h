/*
 * breif ͨ�÷�����ģ�ͣ�֧�ֳ�������
 */

#ifndef _UB_SERVER_H
#define _UB_SERVER_H

#include <ul_log.h>
#include <ub_log.h>
#include <ul_net.h>
#include <ub_conf.h>
#include <ub_auth.h>
#include <ul_def.h>

//���÷������ĳ�������
enum {
	UBSVR_SHORT_CONNECT = 0,	//������
	UBSVR_LONG_CONNECT
};

//���÷������������
enum {
	UBSVR_LINGER_CLOSE = 0x1,
	UBSVR_NODELAY = 0x2,
};

//����ѡ�õ�pool����
enum {
	UBSVR_XPOOL = 0,	//�������ڶ��̶߳�����
	UBSVR_CPOOL,		//�������ڶ��̳߳�����
	UBSVR_EPPOOL,		//����epool��pendding pool���������cpool(2.6�����ں�)
	UBSVR_POOL_NUM,	//��ǰ�ж��ٸ�pool
};
enum{
	UBPOOL_SOCKET_CLOSE_DEFAULT,
	UBPOOL_SOCKET_CLOSE_TRUST
};
typedef struct _ub_server_t ub_server_t;

typedef int (* callback_proc)();
typedef int (* callback_check)(void *req, int reqsiz);

/*
 * @brief 	����ub��������
 * @const char *ub������������
 * @return 	����NULL����ʾ����ʧ��
 * 			�ɹ����ش������������
 */
ub_server_t *ub_server_create(const char *svrname = NULL);

/*
 * @brief  �������ļ�����������
 * @param[in] ub_server_t *sev ���������
 * @param[in] ub_svr_t *svrconf ������������Ϣ
 * @return �����뷵��0��ʧ�ܷ��ط�0
 */
int ub_server_load(ub_server_t *sev, ub_svr_t *svrconf);

/*
 * @brief		����ub������
 * @param[in]	ub���������
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_run(ub_server_t *);

typedef int	(*ub_handle_fun_t)();
/**
 * @brief ����ÿ���߳�����ʱ��Ļص�����
 *
 * @param [in/out] sev   : ub_server_t*
 * @param [in/out] op   : ub_handle_fun_t
 * @return  int ���óɹ�����0������ʧ��
 * @retval   
 * @see 
**/
int ub_server_set_threadstartfun(ub_server_t *sev, ub_handle_fun_t op);

/**
 * @brief �����߳��˳�ʱ��Ĵ�����
 *
 * @param [in/out] sev   : ub_server_t*
 * @param [in/out] op   : ub_handle_fun_t
 * @return  int ���óɹ�����0������ʧ��
 * @retval   
 * @see 
**/
int ub_server_set_threadstopfun(ub_server_t *sev, ub_handle_fun_t op);

/**
 * @brief ֹͣubserver
 *
 * @return  int �ɹ�����0������ʧ��
 * @retval   
 * @see 
**/
int ub_server_stop(ub_server_t *);

/*
 * @brief	join ub������
 * @param[in]	ub���������
 * @return	�ɹ�����0��ʧ������
 */
int ub_server_join(ub_server_t *);


/*
 * @brief		�ݻ�ub������
 * @param[in]	ub���������
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_destroy(ub_server_t *);

//�������ú���

/* @brief		����ip��֤ϵͳ���
 * @param[in]	sev ub���������
 * @param[in]	ip��֤ϵͳ���
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_ip_auth(ub_server_t *sev, ub_auth_t *auth);

/* @brief		���ü����˿�
 * @param[in]	sev ub���������
 * @param[in]	port�����Ķ˿�
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_listen_port(ub_server_t *sev, int port);

/*
 * @brief 		���������лص�����
 * @param[in] 	sev ub�������ľ��
 * @param[in] 	cb �����д���ص�����
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_native_callback(ub_server_t *sev, callback_proc cb);

/*
 * @brief	 	���ü����лص�����
 * @param[in] 	sev ub���������
 * @param[in] 	cb_deal�����д����߼�����
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_callback(ub_server_t *sev, callback_proc cb_deal);
int ub_server_set_callback(ub_server_t *sev, callback_proc cb_deal, callback_check cb_check);

/*
 * @brief 		���ö�д������
 * @param[in] 	sev ub���������
 * @param[in] 	rddata ����������ַ��N���߳�����Ҫ�ռ��ָ������
 * 				���data����ΪNULL��ub�������������ã��Լ�����һ�λ�����
 * @param[in] 	read_siz ����������С
 * @param[in] 	write_siz д��������С
 * @param[in] 	wtdata ����������ַ��N���߳�����Ҫ�ռ��ָ������
 * 				���data����ΪNULL��ub�������������ã��Լ�����һ�λ�����
 * @return		�ɹ�����0�� ����ʧ��
 * @ע��	 
 * */
int ub_server_set_buffer(ub_server_t *sev, void **rdbuf, int read_siz, void **wtbuf, int write_siz);
int ub_server_set_buffer(ub_server_t *sev, int threadnum, void **rdbuf, int read_siz, void **wtbuf, int write_siz);

/*
 * @brief 		�����û�ָ�����߳�����
 * @param[in] 	sev ub���������
 * @param[in] 	usersefbuf �û������߳����ݣ�N���߳�����Ҫ�ռ��ָ������, ���Ϊ���Զ������ö���С������
 * @param[in] 	usersefsiz �û������߳����ݵĴ�С
 * @return		�ɹ�����0�� ����ʧ��	
 * @ע��		
 */
int ub_server_set_user_data(ub_server_t *sev, void **usersefbuf, u_int usersefsiz);
int ub_server_set_user_data(ub_server_t *sev, int threadnum, void **, u_int);

/*
 * @brief		����ubserver�������߳���
 * @param[in]	sev ub���������
 * @param[in]	pnum�������߳���
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_pthread_num(ub_server_t *sev, int pnum);

/*
 * @brief		����ubserver��д�����ӳ�ʱ, ��λ����
 * @param[in]	sev ub���������
 * @param[in]	connect_timeout	���ӳ�ʱ
 * @param[in]	read_timeout ����ʱ
 * @param[in]	write_timeout д��ʱ
 * @return		�ɹ�����0��ʧ������
 */
int ub_server_set_timeout(ub_server_t *sev, int connect_timeout, int read_timeout, int write_timeout);

/*
 * @brief	���÷������������
 * @param	sev ���������
 * @param	type �����������������UBSVR_NODELAY | UBSVR_LINGER_CLOSE
 * @return	0��ʾ�ɹ�������ʧ��
 */
int ub_server_setoptsock(ub_server_t *sev, int type);


/*
 * @brief 		���÷���������
 * @param[in] 	ub������
 * @param[in]	 ����������
 * 					XPOOL	��������Ӹ��Ӹ�Ч��ģ��
 * 					CPOOL	�������Ӹ��Ӹ�Ч��ģ��
 * 					EPPOOL	�������Ӹ��Ӹ�Ч��ģ��(��2.6�ں�����ʹ�ã���CPOOLҪ��)
 * @return		����0��ʾ�ɹ�������ʧ��
 */
int ub_server_set_server_type(ub_server_t *sev, int type);

/**
 * @brief ���÷���������
 *
 * @param [in/out] sev   : ub_server_t*
 * @param [in/out] name   : const char* ��ǰ������������
 * @return  int 
 * @retval   
 * @see 
**/
int ub_server_set_server_name(ub_server_t *sev, const char *name);

/*
 * @brief 		 ������������
 * @param[in]	 ub������
 * @param[in]	 ��������
 * 					SHORT_CONNECT	//������
 * 					LONG_CONNECT	//������
 * @return		����0��ʾ�ɹ�������ʧ��
 */
int ub_server_set_connect_type(ub_server_t *sev, int type);


//���û�ȡ����
/*
 * @brief		��ȡ�������ļ����˿�
 * @param[in]	���������
 * @return		���ؼ����˿�
 */
int ub_server_get_listen_port(ub_server_t *sev);

/*
 * @brief		��ȡ��ǰ�߳��ڲ����ӵ�socket���
 * @return		�ɹ�����socket�����ʧ��-1
 */
int ub_server_get_socket();

/*
 * @brief		���õ�ǰ�߳��ڲ����ӵ�socket���
 */
int ub_server_set_socket_flags(int flag = UBPOOL_SOCKET_CLOSE_DEFAULT);

/*
 * @brief 	��ȡ��������
 * @return 	�õ���������ַ, ʧ��NULL
 */
void *ub_server_get_read_buf();

/*
 * @brief	��ȡ�û��Զ�������
 * @return	�õ��Զ������ݵ�ַ��ʧ�� NULL
 */
void *ub_server_get_user_data();

/*
 * @brief 	��ȡ����������С
 * @return 	��������С, ʧ��0
 */
u_int ub_server_get_read_size();

/*
 * @brief 	��ȡ�û��Զ������ݴ�С
 * @return 	���ݴ�С, ʧ��0
 */
u_int ub_server_get_userdata_size();

/*
 * @brief 	��ȡд������
 * @return 	�õ���������ַ, ʧ��NULL
 */
void *ub_server_get_write_buf();

/*
 * @brief 	��ȡд��������С
 * @return 	��������С, ʧ��0
 */
u_int ub_server_get_write_size();

/*
 * @brief	�õ����ӳ�ʱ ��λ����
 * @return	�ɹ��������ӳ�ʱʱ�䣬ʧ�ܷ���0
 */
int ub_server_get_connect_timeout();

/*
 * @brief	�õ�����ʱ ��λ����
 * @return	�ɹ����ض���ʱʱ�䣬ʧ�ܷ���0
 */
int ub_server_get_read_timeout();

/*
 * @brief	�õ�д��ʱ ��λ����
 * @return	�ɹ�����д��ʱʱ�䣬ʧ�ܷ���0
 */
int ub_server_get_write_timeout();

/*
 * @brief	�õ����Ӷ�ip
 * @return  ip��ַ
 */
in_addr_t ub_server_get_ip();

//pendding pool(cpool/eppool)��ר������
//��ʷԭ���������cpool�Ĳ������ú�eppool�Ĳ������ü���
/*
 * @breif 		����cpool/eppool�ľ������г��ȣ�Ĭ��Ϊ100
 * @param[in] 	sev ���������
 * @param[in] 	siz ���д�С
 * @return		����0��ʾ�ɹ�������ʧ��
 */
int ub_server_set_cpool_queue_size(ub_server_t *sev, int siz);

/*
 * @brief	��ȡcpool/eppool�������д�С
 * @return	cpool/eppool�������д�С
 */
int ub_server_get_cpool_queue_size(ub_server_t *sev);

/*
 * @brief		����cpool/eppool socket������еĴ�С
 * @param[in]	sev���������
 * @param[in]	siz socket������д�С
 * @return		����0��ʾ�ɹ�������ʧ��
 */
int ub_server_set_cpool_socket_size(ub_server_t *sev, int siz);

/*
 * @brief		��ȡcpool/eppool socket������д�С
 * @return		����cpool/eppool socket������еĴ�С
 */
int ub_server_get_cpool_socket_size(ub_server_t *sev);

/*
 * @brief		���þ���ھ�����������ĳ�ʱʱ�� ��λ��
 * @param[in]	sev ���������
 * @param[in]	timeout ��ʱʱ��
 * @return		����0��ʾ�ɹ�������ʧ��
 */
int ub_server_set_cpool_timeout(ub_server_t *sev, int timeout);
/*
 * @brief ��ȡ����ڻ����������ĳ�ʱʱ�� ��λ��
 * @return ����cpool/eppool�����������ĳ�ʱʱ��
 */
int ub_server_get_cpool_timeout(ub_server_t *sev);

//�߼����ú���
/*
 * @brief	�õ����������
 * @return	�õ���ǰʹ�õ�ub�����������ʧ��NULL
 */
ub_server_t *ub_server_get_server();

#endif

/* vim: set ts=4 sw=4 noet: */
