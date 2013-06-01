/**
 * @file
 * @brief ʵ���߼����ļ������ȡ�ķ�װ
 **/
#ifndef _FILEBLOCK_HEAD_FILE_
#define _FILEBLOCK_HEAD_FILE_
#include "mfiles.h"

#define FB_FD_MAXNUM  2048          ///֧������Ӳ���ļ�: 2048��
#define FB_BLOCK_MAXID  (int)(((unsigned int)1024*1024*1024*2)-1)      //2G-1, 20��-1
#define FB_BLOCK_MAXSIZE (1024*1024*10)        ///һ�����Ϊ10M

///fileblock�� ����ֵ�Ķ���, >=0��ʾ�ɹ�, <0��ʾʧ��. ʧ������ĸ��ִ���������:
enum fileblock_error_code_enum{ 
	FB_ERR_OTHER  = -1,      ///��ͨ����
	FB_ERR_EMPTY   = -2,      ///����Ϊ��, �����ļ��д��ڿ�������
	FB_ERR_NOEXIST = -3,      ///���ݿ鲻����, ��ζ���������Ŀ�ID
	FB_ERR_DATA    = -4,      ///���ݿ鲻����: ֻ����һ����, ��ֻдһ����, �����ݳ��ȺͿ鳤������

	FB_ERR_PARAM   = -6,      ///�ӿڴ����������
	FB_ERR_FILE    = -7,      ///�ļ�����: �ļ�i/o, �ļ���
	FB_ERR_SYS     = -8       ///ϵͳ����: mallocʧ��, �������ID��
};

typedef struct fblock_t
{
	mfiles_t  mfiles; ///���ļ����߼��ļ�����
	int  max_curbid;  //��ǰ��ŵ�����, reloadʱ�������ļ����ȼ�������. �������ʱ,�����.

	int  blocksize;   //���С.
	char module[32];
}fblock_t;

/**     
 *  ��ʼ���ṹ��. ��������Ƿ���ȷ
 *      
 *  @param[in]  pfb          block�ļ�ָ��
 *  @param[in]  modulename   ģ����    
 *  @param[in]  filename     �ļ���   
 *  @param[in]  blocksize    �趨�Ŀ��С
 *  @param[in]  iswritesync  �Ƿ����дͬ��
 *  @param[in]  isreadonly   �Ƿ�ֻ��
 *  @param[out] pfb          block�ļ�ָ�룬��װ�ļ����ȸ���������Ϣ
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_FILE   �ļ��򿪴���
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_SYS    blockϵͳ����
 *  - �ɹ�����0
 */  
int fblock_init( fblock_t* pfb, const char *modulename, const char *filename, 
		int blocksize, bool iswritesync, bool isreadonly );

/**     
 *  �ر�fblock. ���clear_data��λ����������������ļ�
 *      
 *  @param[in]  pfb          block�ļ�ָ��
 *  @param[in]  clear_data   �Ƿ�����ļ�(Ĭ�ϲ����)
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_FILE   �ļ��򿪴���
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_SYS    blockϵͳ����
 *  - 0 �ɹ�
 */
int fblock_fini( fblock_t* pfb, bool clear_data = false );

/**     
 *  ����block id ��ȡ����
 *      
 *  @param[in]  pfb       block�ļ�ָ��
 *  @param[in]  blockid   ��ȡ�Ŀ�id
 *  @param[in]  preadbuf  ��ȡ��buf��ַ
 *  @param[in]  bufsize   ȡ���ĳ���
 *  @param[out] preadbuf  ȡ��������
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_NOEXIST id������
 *  - FB_ERR_EMPTY   �������ݿ�
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_FILE   �ļ��򿪴���
 *  - 0 �ɹ�
 */
int fblock_read( fblock_t* pfb,  int blockid, void* preadbuf, int bufsize );

/**     
 *  ����block id��ȡ�����е�һ���֣������ɿ���ȡ
 *      
 *  @param[in]  pfb       block�ļ�ָ��
 *  @param[in]  blockid   ��ȡ�Ŀ�id
 *  @param[in]  preadbuf  ��ȡ��buf��ַ
 *  @param[in]  bufsize   ȡ���ĳ���
 *  @param[in]  off       block�ڲ�ƫ����
 *  @param[out] preadbuf  ȡ��������
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_NOEXIST id������
 *  - FB_ERR_EMPTY   �������ݿ�
 *  - FB_ERR_FILE   �ļ��򿪴���
 *  - 0 �ɹ�
 */
int fblock_read_partial( fblock_t* pfb,  int blockid, void* preadbuf, int bufsize, int off );


/**     
 *  ����blockid�������ݿ�д����Ӧλ��
 *      
 *  @param[in]  pfb       block�ļ�ָ��
 *  @param[in]  blockid   ׼��д��Ŀ�id
 *  @param[in]  data      д�������
 *  @param[in]  bufsize   д��ĳ���
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_DATA   ���ݴ���
 *  - 0 �ɹ�
 */
int fblock_write( fblock_t* pfb, int blockid, const void* data, int datasize );

/**     
 *  ����blockid���޸Ŀ��ڲ�������
 *      
 *  @param[in]  pfb       block�ļ�ָ��
 *  @param[in]  blockid   ׼��д��Ŀ�id
 *  @param[in]  data      д�������
 *  @param[in]  bufsize   д��ĳ���
 *  @param[in]  off       д��Ŀ���ƫ��
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_DATA   ���ݴ���
 *  - 0 �ɹ�
 */
int fblock_write_partial( fblock_t* pfb, int blockid, const void* data, int datasize, int off );


/**     
 *  ����ͬ��
 *      
 *  @param[in]  pfb       block�ļ�ָ��
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - 0 �ɹ�
 */
int fblock_sync( fblock_t* pfb );

typedef int (*fileblock_handler)(unsigned int id, void* pitem,void* pex);

/**     
 *  ���ر��� (����File��ʽ���ܴ����߱������ٶ� ) 
 *      
 *  @param[in]  pfb          block�ļ�ָ��
 *  @param[in]  pitembuf     Ϊ��������׼���Ļ���
 *  @param[in]  itembufsize  ÿ�ζ�ȡ�ĳ���
 *  @param[in]  handler      ���������ľ��,���¶��壺typedef int (*fileblock_handler)(unsigned int id, void* pitem,void* pex);
 *  @param[in]  pex          ���������Ĳ���ָ��
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_SYS    ϵͳ����
 *  - >=0 �ɹ������Ŀ�ĸ���
 */
int fblock_traverse_load(fblock_t* pfb,void*pitembuf, int itembufsize, fileblock_handler handler,void* pex);


typedef int (*fileblock_handler_ex)(unsigned int id, bool *stop_flag, void* pitem,void* pex);

/**     
 *  ���ر��� (����File��ʽ���ܴ����߱������ٶ� ) 
 *      
 *  @param[in]  pfb          block�ļ�ָ��
 *  @param[in]  pitembuf     Ϊ��������׼���Ļ���
 *  @param[in]  itembufsize  ÿ�ζ�ȡ�ĳ���
 *  @param[in]  handler      ���������ľ��,���¶��壺typedef int (*fileblock_handler_ex)(unsigned int id, bool *stop_flag, void* pitem,void* pex); *stop_flag ��Ϊtrue��ʱ�����ʹfblock_traverse_load����;�˳�������������
 *  @param[in]  pex          ���������Ĳ���ָ��
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_SYS    ϵͳ����
 *  - >=0 �ɹ������Ŀ�ĸ���
 */
int fblock_traverse_load_ex(fblock_t* pfb,void*pitembuf, int itembufsize, fileblock_handler_ex handler,void* pex);


/**     
 *  ��д������
 *      
 *  @param[in]  pfb           block�ļ�ָ��
 *  @param[in]  start_bid     д�����ʼ���id
 *  @param[in]  block_num     д��Ŀ�ĸ���
 *  @param[in]  data          д�������
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_EMPTY  д�����ݳ���0
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_FILE   �ļ�����
 *  - >=0 д�ɹ��Ŀ���
 */
int fblock_multiwrite( fblock_t* pfb, int start_bid, int block_num, const void* data );

 
/**     
 *  һ��д����, ��ӵ��ļ�ĩβ
 *      
 *  @param[in]  pfb           block�ļ�ָ��
 *  @param[in]  block_num     д��Ŀ�ĸ���
 *  @param[in]  data          д�������
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_EMPTY  д�����ݳ���0
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_FILE   �ļ�����
 *  - >=0 д�ɹ��Ŀ���
 */
int fblock_multiappend( fblock_t* pfb, int block_num, const void* data );

 
/**     
 *  һ�ζ�����
 *      
 *  @param[in]  pfb           block�ļ�ָ��
 *  @param[in]  start_bid     ��ȡ����ʼ���id
 *  @param[in]  block_num     ��ȡ�Ŀ�ĸ���
 *  @param[in]  pbuf          ��ȡ������ָ��
 *  @return 
 *  - FB_ERR_PARAM  ��������
 *  - FB_ERR_EMPTY  ��ȡ���ݳ���0
 *  - FB_ERR_DATA   ���ݴ���
 *  - FB_ERR_FILE   �ļ�����
 *  - >=0 ���ɹ��Ŀ���
 */
int fblock_multiread( fblock_t* pfb, int start_bid, int block_num, void* pbuf );

 
/**     
 *  ����Ƿ���׷�ӵ�������; �����������,��max_curbid���µ��ļ�������Ӧ��id��.
 *      
 *  @param[in]  pfb           block�ļ�ָ��
 *  @return 
 *  - >0 ��ʾ�и���
 *  - <0 ��ʾ�޸���
 *  - =0 ��ʱδ����
 *  @note ��Ҫ���ڶ���̹����ļ�, �ܼ��������ӡ����µĻ���,Ŀǰ��֧�ַ���������ӵ�׷�Ӽ��
 */ 
int fblock_chk_newdata( fblock_t* pfb );


#endif
