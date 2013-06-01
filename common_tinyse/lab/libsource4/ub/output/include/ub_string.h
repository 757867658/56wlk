/////////////////////////////////////////////////////////////////////////////

#ifndef __UB_STRING_H__
#define __UB_STRING_H__
#include <string.h>
//#ifndef size_t
//#define size_t unsigned int
//#endif

//string related function
#ifndef HAVE_STRLCPY
/**
 * @brief ��ȫ���ַ�����������������Ŀ��λ�������0
 *
 * @param [out] t   : �ִ�������Ŀ��λ��
 * @param [in] s   : ��Ҫ�������ַ���
 * @param [in] n   : Ŀ�괮λ�õ����ռ䳤�ȣ��������Ҫ�������ַ�������С��ضϣ����λ���ã�
 * @return  ��Ҫ�������ַ������ȣ����ǽ���ĳ��� 
**/
extern size_t strlcpy(register char* t, register const char* s, register size_t n);
#endif

/**
 * @brief �����ַ���
 *
 * @param [out] dest   : Ŀ���ַ���
 * @param [in] src   : ��Ҫ���ӵ��ַ���
 * @param [in] n   : Ŀ�괮λ�õ����ռ䳤��,n��С��ض� 
 * @return  0û�нضϡ�-1�нض�
**/
extern int ub_strcat(register char *dest, register const char *src, register size_t n);

/**
 * @brief ��ȡ�ַ���str�ĳ��ȣ��������maxlen-1
 *
 * @param [in] str   : ��Ҫȡ����str
 * @param [in] maxlen   : ���ĳ���
 * @return  �ַ���str�ĳ��ȣ�����maxlen-1������maxlen-1
**/
extern int ub_strnlen(register char *str, register size_t maxlen);

/**
 * @brief ת���ַ�����16���Ƶ���ʽ 
 *
 * @param [out] dst   : Ŀ���ַ���
 * @param [in] dlen   : Ŀ���ַ�������󳤶�
 * @param [in] src   : ��Ҫת�����ַ���
 * @param [in] slen   : ��Ҫת�����ַ����ĳ���
 * @return 0��ʾ�ɹ�����0ʧ��  
**/
extern int ub_strtohex(char *dst, const size_t dlen, char *src, const size_t slen);
#endif

/* vim: set ts=4 sw=4 tw=0 noet: */
