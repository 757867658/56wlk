/*
 */


#ifndef _MYNET_H
#define _MYNET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ul_log.h"

/**
 *@file
 *@brief ���紦������װ
 */

/**
 * @brief ����ʱ��΢�뼶����tcp���Ӻ��� 
 * @param host : ����������������ΪNULL
 * @param port : �������������˿�
 * @param timeout : ���ʱʱ�䣨΢���ָ�룬����ΪNULL�����ӳɹ�timeout��������Ϊʣ���ʱ��
 * @param name : ����ģ����
 * @return -1 ����ʧ�ܻ�ʱ
 * @return socket ���ӳɹ���socket
 */
int MyConnectO(const char *host, int port, int *timeout, const char *name);

/**
 * @brief ����ʱ��΢�뼶����socket������ 
 * @param sock : �����ӵ�socket
 * @param buf  : ����Ŀռ�
 * @param len  : �������󳤶�
 * @param timeout  : ���ʱʱ�䣨΢���ָ�룬����ΪNULL���������غ�timeout��������Ϊʣ���ʱ��
 * @param name : ����ģ����
 * @return -1 ��ʧ��
 * @return ʵ�ʶ������ַ���
 */
int MyReadO(int sock, void *buf, ssize_t len, int *timeout, const char *name);

/**
* @brief ����ʱ��΢�뼶����socket��������ֱ�������������г�����stop�������߶���len���ȣ���ʱ�� 
* @param sock : �����ӵ�socket
* @param buf  : ����Ŀռ�
* @param len  : �������󳤶�
* @param timeout  : ���ʱʱ�䣨΢���ָ�룬����ΪNULL���������غ�timeout��������Ϊʣ���ʱ��
* @param name : ����ģ����
* @param stop : �����������г���stop�ִ�ʱ��ֹͣ��
* @return -1 ��ʧ��
* @return ʵ�ʶ������ַ���
*/
int MyReadOS(int sock, void *buf, ssize_t len, int *timeout, const char *name,
	     const char *stop);

/**
* @brief ����ʱ��΢�뼶����socketд���� 
* @param sock : �����ӵ�socket
* @param buf  : Ҫд���Ŀռ�
* @param len  : Ҫд������󳤶�
* @param timeout  : ���ʱʱ�䣨΢���ָ�룬����ΪNULL���������غ�timeout��������Ϊʣ���ʱ��
* @param name : ����ģ����
* @return -1 ��ʧ��
* @return ʵ��д�����ַ���
*/
int MyWriteO(int sock, void *buf, ssize_t len, int *timeout,
	     const char *name);

/**
* @brief �ر�socket 
* @param fd : �����ӵ�socket
* @return -1 �ر�ʧ��
* @return 0 �ɹ��ر�
*/
int MyClose(int fd);

/**
* @brief ����tcp�����˿� 
* @param port : �˿�
* @param queue  : listen�ȴ����г���
* @return -1 ʧ��
* @return �ɹ����ؿ���������fd
*/
int MyTcpListen(int port, int queue);

#endif // _MYNET_H
/* vim: set ts=8 sw=4 sts=4 tw=78 noet: */
