/***************************************************************************
 * 
 * 
 **************************************************************************/
 
 
 
/**
 * @file CmdOption.h
 * @brief 
 *  
 **/


#ifndef  __CMDOPTION_H_
#define  __CMDOPTION_H_

#include "ConfigGroup.h"
#include "utils/cc_utils.h"

namespace comcfg{
	/**
	 * @brief ��getopt��Ϊ��һ�µĵط���
	 *         ���optstring = "a:b:"
	 *         ����./run -a -b c d
	 *         optstring��� -b��Ϊ-a�Ĳ���
	 *         ��CmdOption���c, d��ջ������c��-b����d��-a
	 *
	 */
	class CmdOption : public ConfigGroup{
		public:
			/**
			 * @brief �����в�����ʼ�����������ظ����ã�
			 *
			 * @param [in] argc   : int ��������
			 * @param [in] argv   : char*  const* ����
			 * @param [in] optstring   : const char* ѡ����Ϣ����getopt��ͬ
			 * @return  int 0Ϊ�ɹ�������Ϊʧ��
			 * @retval   
			 * @see 
			**/
			int init(int argc, char * const * argv, const char * optstring);
			/**
			 * @brief �����в������Ƿ����ĳ��ѡ��
			 *
			 * @return  bool 
			 * @retval   
			 * @see 
			**/
			bool hasOption(const char *) const;
			/**
			 * @brief ĩβ����������ע�⣺�����ܵĲ���������
			 * 		�磺argc = 5, argv = {"./run", "-a", "xxx", "-h", "yyy", "zzz"}, optstring = "a:h"
			 * 		��ô�����argSize = 3 (�ֱ���./run, yyy��zzz)������xxx��-a�Ĳ���
			 *
			 * @return  size_t ��������
			 * @retval   
			 * @see 
			**/
			size_t argSize() const;
			/**
			 * @brief ��ȡ��n������
			 *
			 * @param [in] n   : int ����λ�ã�0 ~ argSize-1��
			 * @return  const char* 
			 * @retval   
			 * @see 
			**/
			const char * getArg(int n) const;

			typedef int (*hook_t)(CmdOption * );
			/**
			 * @brief ע��������ӣ��������option������runHookʱ��ִ����Ӧ�Ĺ���
			 * 			һ��option��ע��������
			 *
			 * @param [in] option   : const char*
			 * @param [in] hook   : hook_t
			 * @return  int 
			 * @retval   
			 * @see 
			**/
			int registHook(const char * option, hook_t hook);
			/**
			 * @brief ���й���
			 *
			 * @return  int �����壨runHook���ܹ��ӵ����н����
			 * 				����Ĺ��ӵ����н�������ڹ��������쳣�׳������д���
			 * @retval   
			 * @see 
			**/
			int runHook();
			virtual  ConfigUnit & operator= (ConfigUnit & unit) {
				return ConfigUnit::operator=(unit);
			}
			CmdOption();
		protected:
			static const int CHAR_TABLE_SIZE = 256;
			char ch[CHAR_TABLE_SIZE];
			static const char CMD_NONE = 0;
			static const char CMD_OPT0 = 1;
			static const char CMD_OPT1 = 2;
			static const char CMD_OPT2 = 3;
			std::vector <str_t> arg;
			std::multimap <str_t, hook_t> hkmap;
			typedef std::pair<str_t, hook_t> pair_t;
			typedef std::multimap<str_t, hook_t> :: iterator hkitr_t;
			typedef std::pair<hkitr_t, hkitr_t> hk_pair_t;

			int setOptString(const char *);
	};
}












#endif  //__CMDOPTION_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
