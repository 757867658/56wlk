#ifndef FELT_RULE_H_
#define FELT_RULE_H_

#include <string.h>
#include <math.h>

#define FWORD_LEN 100
#define QLEN 80
#define RULE_WORD_LEN 200
#define RULE_NUM      1000

// {{{ MACRO Definition
#define	MING_CI		1		//����
#define FANG_WEI_CI	3		//nd	��λ��  ��  ��
#define REN_MING	5		//����
#define DI_MING		6		//����
#define DONG_CI		9		//����
#define XING_RONG_CI	10		//���ݴ�
#define FU_CI		12		//����
#define SHU_CI		13		//����
#define LIANG_CI	14		//����
#define	DAI_CI		15		//����
#define JIE_CI		16		//���
#define LIAN_CI		17		//����
#define ZHU_CI		20		//����
#define HOU_JIE_CHENG_FEN		22		//��ӳɷ�
#define YU_SU		25		//����
#define YU_QI_CI	30		//������
// }}}

// {{{data structure definition
typedef struct WP_RULENODE_T
{
  char word[RULE_WORD_LEN];
  unsigned int head;
  unsigned int current;
  unsigned int tail;
  WP_RULENODE_T *pnext;
}wp_rulenode_t;

typedef struct WP_RULEDICT_T
{
  //prefix
  wp_rulenode_t prefixarr[RULE_NUM];

  //suffix
  wp_rulenode_t suffixarr[RULE_NUM];
}wp_ruledict_t;
// }}}


// {{{function definition

//if failed, return -1;
int fr_load_rule_dict(wp_ruledict_t *pruledict,char* path);

int fr_free_rule_dict(wp_ruledict_t *pruledict);


//if not find, return -1; else return the idx in rulearr.
int fr_findrule(char *buf, wp_rulenode_t rulearr[], int len);

//if tag not defined, return 0, else return predefined code.
int fr_gettagcode(const char* buf);

// }}}

#endif
