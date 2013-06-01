/********************************************************************
 ********************************************************************/

#ifndef WP_FELT_H__
#define WP_FELT_H__

// {{{ Header Files
#include <scwdef.h>
#include <postagger.h>

#include "feltrule.h"
#include "wp_proper_noun.h"
#include "replace_index.h"
#include "identify_index.h"

#include "wp_nomeans.h"
#include "wp_place_parent.h"
#include "ul_thr.h"
//#include "mid.h"
 // }}}

// {{{ Macro Definition
/*ͨ��ǰ��׺*/
#define FELT_SET_APPFIX(property,value)		((property) | value)
#define FELT_GET_APPFIX(property)		((property) & 0x00000003)

/*��ͨǰ��׺*/
#define FELT_SET_COMFIX(property,value)		((property) | ((value) << 2))
#define FELT_GET_COMFIX(property)		(((property) & 0x0000001C) >> 2)

/*������Ϣ*/
#define FELT_SET_AREA(property,value)		((property) | (value << 5))
#define FELT_GET_AREA(property)			(((property) & 0x000000E0) >> 5)

/*��Ҫ��*/
#define FELT_SET_IMPT(property,value)		((property) | ((value) << 8))
#define FELT_GET_IMPT(property)			(((property) & 0x00000700) >> 8)

/*�Ƿ��Ƶ--ͨ��diff����*/
#define FELT_SET_DIFFLAG(property)		((property) | 0x00000800)
#define FELT_GET_DIFFLAG(property)		((property) & 0x00000800)

/*dropword property*/
#define FELT_SET_DROP(property,value)		((property) | (value << 12))
#define FELT_GET_DROP(property)			(((property) & 0x00003000) >> 12)

/*splitword property*/
#define FELT_SET_SPLIT(property,value)		((property) | (value << 14))
#define FELT_GET_SPLIT(property)		(((property) & 0x0000C000) >> 14)

/*diff merge reweight*/
#define FELT_SET_REQ_DIFF(property, value)	((property) |( value) << 16)
#define FELT_GET_REQ_DIFF(property)		(((property) & 0x00010000) >> 16)

#define SEARCH_IN_AT 0
#define SEARCH_IN_CT 1

#define MAX_QUERY_LEN 80 

/*
 * Term ������
 */
#define WEIGHT_OFFSET_EXACT		0	//����Ҫ��Offset������Ȩ��
#define WEIGHT_OFFSET_NORMAL	1	//��Diff Term����Bitmap Term������������
#define WEIGHT_OFFSET_DEFAULT	2	//Offset���ƿ�ѡ, Ȩ��Ҫ��  Ĭ��ֵ
#define WEIGHT_ONLY   3				//ֻҪ��Ȩ��
#define OFFSET_ONLY   4				//ֻҪ��Offset, Ŀǰδʹ��
#define APPEAR_ONLY   5				//ֻ����
#define NO_USE        6				//�ɶ����Ĵ�

/*
 * Offset ���Ƶ�����
 */
#define OFFSET_LOOSE		0	//����Term������
#define OFFSET_NORMAL		1	//��ֻ������Term && OFFSET_LOOSE, �� OFFSET_LOOSE ��Ϊ��ֵ 
								//Ĭ��ֵ
#define OFFSET_TIGHT_HALF	2	//����Term֮���ϵ����� ��OFFSET_TIGHT_WHOLE��Ϊ��ֵ
#define OFFSET_TIGHT_WHOLE  3	//����Term��ȫ����
// }}} 

//���Ա��
#define OMIT_JUNK_INDEX 0		//ʡ�������ʲ�����Ч	��Query����
// }}}

// }}} 

// {{{ phraseoffreq
#define WHOLEOFFREQ 1
#define HALFOFFREQ 2
// }}}

#define FELT_MAX_BUF_SIZE 65536
#define FELT_MIN_BUF_SIZE 4
// }}}

/* {{{ constant defined here */
const char FELT_PREFIX[] = "W_";
const int FELT_PREFIX_LEN = 2;
const int FELT_QR_FREQ_LB = 100;
const int FELT_QU_FREQ_LB = 10;
const int FELT_QS_FREQ_LB = 0;
/* }}} */

/* {{{ data structure defined here */
typedef struct WP_FELT_T
{
	char* m_buf;
	int m_size;
	int m_curpos;
	int m_termcount;
	int* m_termoffsets;
	int* m_termpos;
}
wp_felt_t;

typedef struct WP_SCOPE_T
{
	int m_valid;
	int m_ftype;
	int m_ptype;
	int m_felted;
	int m_difflag;
	int m_impt;
	int m_area;
	int m_comfix;
	int m_appfix;
	int m_drop;
    int m_phraselen;
}
wp_scope_t;

/*
 * temporaray backup store for scw_out_t structure
 * ֻ�ܱ���scw_out_t ���BUFFER֮һ
 */
typedef struct WP_PackBuf
{
	int termcount;
	char buf[MAX_QUERY_LEN * 2 + 40];
	int pos[MAX_QUERY_LEN * 2 + 40];
	int offsets[MAX_QUERY_LEN * 2 + 40];
}
wp_packbuf_t;

/*
 * Toolkit Class
 * Query data with its length
 * Move From IndSearch
 * @auther
 */
typedef struct _in_info
{
	char*	inbuf;
	int		slen;
//	int	omitBasicCount;		//��ʡ�ԵĻ����ʵĸ��� �����չ�ʡ������FELT
//	int	omitBasicLen;		//��ʡ�ԵĻ����ʵ��ܳ��� �����չ�ʡ������FELT
}
info_in_t;

/*
 * Toolkit Class
 * All input data for IndSearch and Felt
 * @auther
 */
typedef struct SEARCH_SEG_DATA
{
	info_in_t m_in;
	
	scw_out_t* m_out;
	
	wp_felt_t m_felt;
	wp_felt_t m_felt_merge;
	
	char* m_outbuf[8];				//ǰ�������д�buffer,��һ����ճ��buffer
	unsigned int* m_poss[8];
	unsigned int* m_offsets[8];
	unsigned int* m_count[8];
	
	char* m_tmpbuf;
	
	pos_pack_t* m_pos_pack;
	scw_item_t m_pitem;
	dm_pack_t* m_pDmPack;
	
	char m_tags[MAX_QUERY_LEN];
	char m_term_type[MAX_QUERY_LEN];
	char m_off_type[MAX_QUERY_LEN];
	char m_ispronoun[MAX_QUERY_LEN];
	char m_isorgname[MAX_QUERY_LEN];
	bool m_hasproper;
}
search_seg_data_t;


/*
 * �ʵ��
 * ����ճ��ģ��ʹ�õĸ��ִʵ�
 */
typedef struct
{
	scw_worddict_t* pScwDict;		//ָ��ִʴʵ�
	pos_dict_t*   pPosDict;			//ָ����Ա�ע�ʵ�
	Sdict_search* pFeltDict; 		//ָ��ճ�Ӵʵ�,�������������ʼ��
	Sdict_search* pAtFeltDict;		//ָ��ATճ�Ӵʵ�
	Sdict_search* pDropDict;		//ָ���ز�ʡ�Դʴʵ�,�ڽ����ز�ճ��ʱʹ��
	wp_ruledict_t* pRuleDict;		//����ǰ��׺ճ�ӹ���ʵ�
	Sdict_search* pRequire;			//����ʴʵ�
//	dm_dict_t* pEmbracedict;		//����λ��ϵ�ʵ䣬��ʱȡ��

	Sdict_search* pPlaceMap;		//�������¼���ϵӳ��ʵ� 
	char* placeParentBuffer;		//�����ϼ������� �й�#�й�-����#�й�-�ӱ�-ʯ��ׯ# (WJY)
	Sdict_search* pOmitDict;		//ʡ�Դʴʵ�	(WJY)
	Sdict_search* phrasedict;		//��ִʴʵ�
	dm_dict_t* pProperNounDict;		//����ר���ʵ�
	dm_dict_t* pOrgnameDict;		//���ػ������ʵ�
	dm_dict_t* pSemanticDict;		//��һ���ʵ�

	Sdict_search* pDiffDict;		//diff term �ʵ�
	Sdict_search* pHiLiIdfDict;		//������IDF�ʵ�
	Sdict_search* pStopWordDict;		//ͣ�ôʱ�
	Sdict_search* pOffTypeDict;		//Term�������Դʵ�
	Sdict_search* yibian1_dict;     	//�ױ�����ҳ����ʵ�
	Sdict_search* yibian2_dict;
	Sdict_search*vd_nomeans_dict;
	Sdict_search* m_pSitefix;		//Ѱַ���׺�ʵ�
	trans_dict_t * m_err_transdict;		//����ʵ�
	trans_dict_t * m_syn_transdict;		//ģ��ͬ���滻�ʵ�
	ident_dict_t * m_site_identdict;	//������ʶ��ʵ�
	
	bool policySwitch[10];			//������Ч��������	������ �պ��޸ĵ�
	unsigned short sema_num_map[65536]; //����ת����
}
wp_dict_pack_t;
/* }}} */

/**************************************************
 * type		  : public 
 * function	  : init the segData
 * in-argu	  : bufsize	 : the segData's buf size (by byte) 
 * out-argu	  : psegData : the data pir
 *			  : phraseoffreq : atFelt use
 * return	  : >=0 if success
 *			  : <0  if failed
 * other	  : psegData��phraseoffreqֻ��Ҫ���ⲿ����������Ҫ
 *			  : ����ռ䣬bufsize��СΪ4�����Ϊ65536,�������
 *			  : ִ�й�����ʧ�ܣ����ӡʧ����Ϣ���ͷ��Ѿ������
 *			  : �ڴ棬Ȼ�󷵻�-1
 * *************************************************/
int init_segData(search_seg_data_t *&psegData, char *&phraseoffreq, int bufsize);

/****************************************************
 * type		  : public
 * function	  : init the dict pack
 * in-argu	  : worddict : the worddict dir path
 *            : bsdict   : the bsdict dir path
 * out-argu	  : pdictPack: the dict pack
 *			  : atFelt : atFelt flag
 * return	  : >=0 if sucess
 *			  : <0  if failed 
 * other	  : worddict��bsdictΪ�ļ���·�����ڲ���ŵĴʵ��
 *			  : ����ĵ���atFelt��ps�û�at����ģ�ns��Ʒ�ò���
 *			  : ʹ��Ĭ��ֵ����
 * **************************************************/
int init_dictPack(char* worddict,char* bsdict, wp_dict_pack_t *&pdictPack, bool atFelt = false);


/******************************************************************
 * type     :   pulblic
 * function :   ns_get_seg_pos
 * in-argu  :   instring 
 *          :   segData : �дʴ��Ա�עճ�ӽṹ��
 *          :   dictPack: �ֵ��
 *          :   level : ճ�����ȣ�һ��Ϊ0
 * out-argu :
 * return   :   >= 0 if succeed
 *          :   <0   if failed
 * other    :   �����������дʴ��Ա�עճ���Լ�����֮���һЩ��
 *			:	������ת����instring��ԭʼ������ָ�룬
 *			:	instring_len����Ҫ����ĳ��ȡ�ns_get_seg_pos��
 *			:	��ı�ԭʼ���������д�ģ���һ��ȱ�ݣ����
 *			:	instring��������ĳ��term�ĳ��ȳ�����
 *			:	instring_len����ô�дʺ���������instring_len��
 *			:	����ȫ��instring�����Խ������ⲿ�����м��һ��
 * ****************************************************************/
int ns_get_seg_pos(
         char* instring, int instring_len,  search_seg_data_t *&segData, wp_dict_pack_t *&dictPack);

extern int g_phraseseplen;

/* {{{ wp_felt_hfword_p */
/********************************************************************
 * type        : public
 * function    : felt the high frequency into two-word.This function
 *             : will be used in parser. 
 * in-argu     : pack_outbuf * pout : the out buffer of word segment
 *             : Sdict_search * pdict : the hfword dict
 *             :     if a term in dict, other==0 indicate the term is
 *             :     a stopword, other==1 indicate the term is a hfword and
 *             :     needs to felt
 * out-argu    : wp_felt_t * pres : the output result for felt hfword
 * return      : 1 if success
 *             : <0 if failed
 * notice      : ����������ڽ���ճ�ӣ��Ը�Ƶ�ʽ���ճ�ӣ�������Բο���Ӧ�ĵ�
 *			   : �ܹ�֧�ֵ�������봮����ȡ���ڷ����buf�Ĵ�С
 ********************************************************************/
int wp_felt_hfword_p (wp_felt_t* pres, scw_out_t* pout,
					  Sdict_search* pdict);
/* }}} */

/* {{{ wp_get_query */
/****************************************************************************
 * ճ��ģ������Ҫ����ں���
 * ���ݷִʽ����ճ�Ӽ���, ����ճ�ӽ��
 * ֮����������ճ�ӽṹָ��, ��ȫ��Ϊ�洢�ռ��Լ����
 * ����: ����level, �ֱ���ò�ͬ��ճ�Ӳ���, ��ȡճ�ӽ��
 *
 * type		   : public
 * in-argu	   : pDictPack : �ǵ������װ��ԭ�е��дʴǵ䡢ճ�Ӵǵ䣬
 *			   :             ����������������ڿ���ճ�Ӻ����ԵĴʵ䡣
 *			   :			 ��Щ�ʵ�ļ��ط�ʽ���Բο��°�testfelt��
 *			   : search_seg_data_t : ��װ���дʡ����Ա�ע��ճ�ӽ����
 *			   :			 ����buffer
 *			   : level :     ��ѯ��Σ�֧��-1(��ȷƥ��),0(��ͨ��ѯ),1,12,2,3,4(�������)
 *			   : searchPos : AT��ѯor CT��ѯ
 *			   : phraseoffreq : at phrase��ֵ����Ʊ�ǣ�������ڱ���
 *			   :			 ������ݣ�����ָ��offset���㣬���Ժ����������
 *			   : query_is_org_name : �������query�Ƿ�ΪѰַ��query�������
 *			   :			 ������ճ�����ȡ��������Ϊ�ʴ�ʽquery������Ѱַ
 *			   :			 ��query����ֱ�Ӵ���0��ȥ���ɡ���ϣ�����Դ�����
 *			   :			 ȷ�Ĳ������Բο��°�testfelt����������ļ��㷽ʽ��
 *
 * ***************************************************************************/
int wp_get_query(
		const wp_dict_pack_t* pDictPack,	//�ʵ��
		search_seg_data_t* pSegData,	//������ݰ�
		int level,						//�������, ָ��ճ�Ӳ���
		int searchPos,						//��AT / CT
		char phraseoffreq[],					//��at_phrase offset���Ʊ��
		int query_is_orig_name,				//query���ͣ��Ƿ�ΪѰַ��
		bool omit_junk_word = false,		//�Ƿ��ճ�ӽ��ȥ��
		bool omit_pc_word = false);			//�Ƿ�����ʽ��ȥ��
/* }}} */

/* {{{ wp_init_nomeans_dict  */
/****************************************************************************
 * function  : load no-means phrase configure file and init the internal dict
 * argu-in   : fname: the path and filename of no-means configure file, usually worddict/no_means.txt
 * return    : number of phrases loaded, or -1 if there is any error
 ***************************************************************************/
//int wp_init_nomeans_dict (const char* fname);
/* }}} */

int wp_backup_segdata(search_seg_data_t *des, const search_seg_data_t *src);

int wp_append_buf (wp_felt_t * pFelt, const char *buf, int len, int offset);

int get_subphrase_ind (const scw_out_t* pOut, int boff, int eoff);

/*************************************************                                              
 * Function    : clear_segData()
 * Description : �����дʺ�ճ�Ӵʵ�,�ͷ���������ṹ��ռ�ڴ�                                    
 * Input  : psegData		 �дʴ��Ա�עճ�Ӱ�ָ��                                             
 *        : phraseoffreq   ��¼at phrase��ֵ�bufferָ��                                      
 * Output :         
 * Return : 
 * Others : �ú����Ƿ��̰߳�ȫ�ģ���Ҫ����ʹ��                                                  
 **************************************************/                                            
void clear_segData(search_seg_data_t *psegData, char* phraseoffreq);
/*************************************************                                              
 * Function    : clear_dictPack()
 * Description : ���ٴʵ䣬�ͷŴʵ���ռ�ڴ�
 * Input  : pdictPack                                      
 * Output :         
 * Return : 
 * Others : �ú����Ƿ��̰߳�ȫ�ģ���Ҫ����ʹ��                                                  
 **************************************************/                                            
void clear_dictPack(wp_dict_pack_t *pdictPack);

#endif
