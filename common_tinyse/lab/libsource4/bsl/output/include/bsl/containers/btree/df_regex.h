/**======================================================================
 *
 *  df_regex.h - Pyramid / df-lib
 *
 *
 *
 *  -------------------------------------------------------------------
 *
 *  Description
 *
 *     ������ʽƥ����
 *
 *  -------------------------------------------------------------------
 *
 *  Change Log
 *
 *
 =====================================================================**/
#ifndef _DF_REGEX_H_
#define _DF_REGEX_H_

#include <regex.h>

class df_regex_t
{
public:
    /**
     * @brief
     *
     * @see
     * @note
    **/
    df_regex_t();
    /**
     * @brief
     *
     * @see
     * @note
    **/
    ~df_regex_t();

public:
    // ģʽ��ʼ��
    // @param(in): pattern - ģʽ
    // @param(in): flags - ƥ���option��ͬregcomp
    // @return: true - �ɹ�
    //          false - ʧ��
    bool init(const char* pattern, int flags);

    // ����ַ����Ƿ�ƥ��
    // @param(in): text - ��ƥ����ַ���
    // @param(in): flags - ƥ���option��ͬregexec
    // @return: true - ƥ��
    //          false - ��ƥ��
    bool match(const char* text, int flags);

    // �����һ��ģʽ
    void destroy(void);

private:
    bool _init;
    regmatch_t* _match;
    regex_t _reg;
    size_t _nmatch;
};

#endif
