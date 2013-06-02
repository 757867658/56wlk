/**
 * @file
 * @brief �ַ���ӳ��ʵ�
 */
/**
 * @brief �ַ���ӳ��ʵ��ǽ��ַ�������ʵ�ʱ�᷵��һ����֮��Ӧ����������ʵ���ַ����Ͷ�Ӧ������֮����໥���ҡ�
 */

#ifndef STRDICT_H
#define STRDICT_H

#include <ul_dict.h>


typedef struct UL_STRDICT_T
{
	unsigned int m_bufsize;
	unsigned int m_curpos;
	unsigned int m_count;
	char* m_strbuf;
	Sdict_build * m_dict;
}ul_strdict_t;

/**
 *  �õ��ֵ䵱ǰ�洢�Ľڵ�����
 *  
 *  @param[in]  pstrdict  �ֵ��ָ��
 *  @param[out] ��
 *  @return �ڵ�����
 */

int ul_strd_size(const ul_strdict_t * pstrdict);

/**
 *  �����ַ���ӳ��ʵ�
 *  
 *  @param[in]  hashnum  �ʵ�Ĺ�ϣͰ�Ĵ�С
 *  @param[in]  bufsize  �ʵ仺�����ĳ�ʼ��С��ÿ�����ӵ��ֽ���
 *  @param[out] ��
 *  @return �ֵ�ָ��
 * - ��NULL   �ɹ�
 * - NULL     ʧ��
 */ 
ul_strdict_t * ul_strd_create(int hashnum,int bufsize);


/**
 *  �����ַ���ӳ��ʵ�
 *  
 *  @param[in]  pstrdict  �ֵ��ָ��
 *  @param[out] ��
 *  @return ��
 */ 
void ul_strd_destroy(ul_strdict_t *pstrdict);

/**
 *  ���ַ���ӳ��ʵ��в���һ���ַ���
 *  
 *  @param[in]  pstrdict  ָ��ʵ��ָ��
 *  @param[in]  str       ��������ַ���
 *  @param[in]  pcode     ���ڴ洢���ַ�����Ӧ�����Ŀռ�
 *  @param[out] pcode     ��������ַ�����Ӧ������
 *  @return ��������ֵ
 * - 1   �ɹ�
 * - 0   �ַ����Ѵ���
 * - -1  ʧ��
 */    
int ul_strd_insert(ul_strdict_t* pstrdict,const char * const str,int* pcode);

 
/**
 *  ����ַ���ӳ��ʵ�
 *  
 *  @param[in]  pstrdict  ָ��ʵ��ָ��
 *  @param[out] ��
 *  @return ��
 */ 
void ul_strd_reset(ul_strdict_t *pstrdict);

 
/**
 *  ͨ���ַ������������Ӧ��code
 *  
 *  @param[in]  pstrdict  ָ��ʵ��ָ��
 *  @param[in]  str       �������������ַ���
 *  @param[out] ��
 *  @return code
 * - >=0   ���ҵ������ַ�����Ӧ��code
 * - -1  ʧ��
 */ 
int ul_strd_getcode(const ul_strdict_t*pstrdict,const char* str);


 
/**
 *  ͨ��code���������Ӧ���ַ���
 *  
 *  @param[in]  pstrdict  ָ��ʵ��ָ��
 *  @param[in]  code      �������ַ�����code
 *  @param[out] ��
 *  @return ָ����ҵ����ַ�����ָ��
 * - ��NULL   �ɹ�
 * - NULL     ʧ��
 *  @note the string will be change when new string is insert the dict.BE SURE NOT to keep this string after a new string had been added to the strdict.
 */
const char* ul_strd_getstr(const ul_strdict_t* pstrdict,const int code);

#endif
