/*
 * @brief ip��֤ϵͳ ֧��ͳ�����ʽ
 * ipʶ���ʽ����:
 * 		127.*.*.*
 * 		127.0.0.1
 * 		127.0.0.0/24
 * 		10.0.10.1-10.1.11.1
 * 		127.123.42.1/13-128.111.1.1/13
 *
 * �ڵ���ub_auth_compile��֧�ֶ��ֲ��Һ�ip����ϲ�,�ʺ���֤ip���Ƚ϶�����
 * û�е���ub_auth_complie�������Բ��ҷ�ʽ
 * @reversion auth1.0.0.0
 */

#ifndef _UB_AUTH_H
#define _UB_AUTH_H

typedef struct _ub_auth_t ub_auth_t;

/*
 * @brief ����ip��֤ϵͳ���
 * @param[in] unsigned int sup_size ��ʾĬ�ϵ�֧��ip�������Զ�̬��չ��������Ĭ��ֵ
 * @return ���ش�����ip��֤�����ʧ�ܷ���NULL
 */
ub_auth_t * ub_auth_create(unsigned int sup_size=64);

/*
 * @brief ����ip��֤ϵͳ
 * @param[in] ub_auth_t * ip��֤���
 */
void ub_auth_destroy(ub_auth_t *);

/*
 * @breif 	���ļ�������ip��Ϣ
 * 			�ļ���������
 * 				#ÿ������һ�У���ͷ��#��ʾע����
 * 				127.*.*.*
 * 				127.0.0.1
 * 				127.0.0.0/24
 * 				10.0.10.1-10.1.11.1
 * 				127.123.42.1/13-128.111.1.1/13
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * @param[in]	const char *file	ip�ļ�·��
 * @return		�ɹ�����0������ʧ��
 */
int ub_auth_load_ip(ub_auth_t *auth, const char *file);

/*
 * @breif		�ֶ���ip����ӵ���֤ϵͳ��
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * @param[in]	const char *ip ip�ε��ַ�������
 * @return		�ɹ�����0������ʧ��
 */
int ub_auth_push_ip(ub_auth_t *auth, const char *ip);

/*
 * @brief		���뵼���ip��Ϣ���ϲ��ظ����䣬�������ֲ���������
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * return		����0�ɹ�������ʧ��
 */
int ub_auth_compile(ub_auth_t *auth);

/*
 * @brief		�ж�ip�Ƿ�����֤ϵͳ��
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * @param[in]	ip	�жϵ�ip *.*.*.* ��ʽ
 * @return		1 ��ʾ���ڣ� 0��ʾ������
 */
int ub_auth_ip_str(ub_auth_t *auth, const char *ip);

/*
 * @brief		�ж�ip�Ƿ�����֤ϵͳ��
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * @param[in]	ip	���罻���л�ȡ��32λint��ip
 * @return		1 ��ʾ���ڣ� 0��ʾ������
 */
int ub_auth_ip_int(ub_auth_t *auth, const unsigned int ip);

/*
 * @brief		�ж�ip�Ƿ�����֤ϵͳ��
 * @param[in]	ub_auth_t *auth ��֤ϵͳ���
 * @param[in]	fd	���罻��accept����fd��������ݾ������ip��ַ
 * @return		1 ��ʾ���ڣ� 0��ʾ������
 */
int ub_auth_ip_fd(ub_auth_t *auth, const int fd);

#endif

