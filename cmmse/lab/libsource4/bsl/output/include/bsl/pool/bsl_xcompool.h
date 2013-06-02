/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file bsl_xcompool.h
 * @brief 
 *  
 **/


#ifndef  __BSL_XCOMPOOL_H_
#define  __BSL_XCOMPOOL_H_


#include "bsl_xmempool.h"
#include <set>

namespace bsl
{
class xcompool : public mempool
{
	size_t _maxblksiz;
	void * _buffer;
	size_t _bufsiz;
	xmempool _pool;

	typedef std::set<void *> SET;
	SET _set;
public:
	xcompool() {
		_maxblksiz = 0;
		_buffer = NULL;
	}
	~xcompool() {
		destroy();
	}
	/**
	 * @brief ��ʼ���ڴ��
	 *
	 * @param [in/out] maxblk   : size_t ÿ��slab������ڴ��С
	 * @param [in/out] bmin   : size_t	slab�������С�ڴ浥Ԫ
	 * @param [in/out] bmax   : size_t	slab���������ڴ浥Ԫ
	 * @param [in/out] rate   : float	slab�ĵ�������
	 * @return  int  �ɹ�����0, ����ʧ��
	 * @retval   
	 * @see 
	 * @note 
	**/
	int create (size_t maxblk = 1<<20, 
			size_t bmin = sizeof(void *), size_t bmax = (1<<16), 
			float rate = 2.0f);

	/**
	 * @brief �����ڴ��
	 *
	 * @return  void 
	 * @retval   
	 * @see 
	 * @note 
	**/
	void destroy();

	void * malloc (size_t size);

	void free (void *ptr, size_t size);

	/**
	 * @brief ����pool������ڴ�
	 *
	 * @return  void 
	 * @retval   
	 * @see 
	 * @note 
	**/
	void clear ();

};
};



#endif  //__BSL_XCOMPOOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
