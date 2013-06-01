/***************************************************************************
 * 
 * Copyright (c) 2008 Baidu.com, Inc. All Rights Reserved
 * $Id: default.cpp,v 1.6 2009/11/18 20:27:50 yanghy2 Exp $ 
 * 
 **************************************************************************/



/**
 * @file default.cpp
 * @author yanghy(yanghuaiyuan@baidu.com)
 * @date 2008/07/24 14:53:04
 * @version $Revision: 1.6 $ 
 * @brief 默认实现
 *  
 **/
#include "nshead.h"
#include "mc_pack.h"
#include "mc_pack_rp.h"
#include "ul_sign.h"
#include <ub_log.h>
#include <algorithm>
#include "tinyse.h"



int DEFAULT_GLOBAL_INIT()
{
    return 0;
}

int DEFAULT_THREAD_INIT()
{
    return 0;
}

int DEFAULT_SESSION_INIT()
{
    return 0;
};

int DEFAULT_QUERY_HANDLE::parse_query(const pointer_t query_cmd,
                                      vector < ts_terminfo_t > &term_list)
{
    nshead_t *req_nshead = (nshead_t *) query_cmd;
    if(req_nshead->body_len < sizeof(ts_head_t))
    {
        return -1;
    }
    //ts_head_t *req_head = (ts_head_t *) & req_nshead[1];
    //mc_pack_t *mc = (mc_pack_t *) & req_head[1];

    //req
	bsl::ResourcePool rp;
	mc_pack_t *mc = mc_pack_open_r_rp((char *)(req_nshead+1), req_nshead->body_len, &rp);
    if(MC_PACK_PTR_ERR(mc)){
        UB_LOG_WARNING("[function:parse_query>>mc_pack_open_r_rp][ret:][desc:mc_pack_open_r_rp error][intput:]");
        return -2;
    }
    //print input
    char in_query[1024*1024];
    mc_pack_pack2json_gbk(mc, in_query, 1024*1024);
    ub_log_pushnotice("[SE INPUT:", " %s]", in_query );
    
    //term_list
    const char *keyword = mc_pack_get_str(mc, "query");
    if(MC_PACK_PTR_ERR(keyword))
    {
        return -2;
    }

    const char delimiters[] = " .,;:!-";
    char *running;
    char *token;
    running = strdup(keyword);
    ts_terminfo_t terminfo;
    token = strsep(&running, delimiters);
    while(NULL != token)
    {
        creat_sign_fs64(token, strlen(token), &terminfo.term.sign1,
                        &terminfo.term.sign2);
        term_list.push_back(terminfo);
        UB_LOG_DEBUG("qterm[%s] [%u:%u]",token, terminfo.term.sign1,
                     terminfo.term.sign2);
        token = strsep(&running, delimiters);
    }
	if (running)
		free(running);
        
    mc_pack_close(mc);
    return 0;
};

class sort_less_t
{
  public:
    bool operator() (const ts_terminfo_t & a, const ts_terminfo_t & b)const
    {   
        return a.ind_num < b.ind_num;
    }   
};
int DEFAULT_QUERY_HANDLE::merge_ind_list(const pointer_t query_cmd,
                                vector < ts_terminfo_t > &term_list,
                                ts_ind_reader_t &ind_reader,
                                vector<ts_index_t> *merged_list) 
{

    //get ind_num
    for(vector < ts_terminfo_t >::iterator iter = term_list.begin();
        iter != term_list.end(); ++iter)
    {
        iter->ind_num = ind_reader.read_ind_num(iter->term);
		if ((int)iter->ind_num<0)
		{
			UB_LOG_WARNING("ind_reader.read_ind_num [%u:%u] failed.",iter->term.sign1,iter->term.sign2);
		}      

        if(0 == iter->ind_num)
        {
            UB_LOG_DEBUG("no result.term[%u:%u]", iter->term.sign1,
                         iter->term.sign2);
            return 0;
        } 

    }
	//找出最短term
    std::sort(term_list.begin(), term_list.end(), sort_less_t());
	
	vector < ts_index_t > *lst1 = ((vector < ts_index_t > *)get_data("lst1"));
	vector < ts_index_t > *lst3 = ((vector < ts_index_t > *)get_data("lst3"));
	lst1->clear();
    lst3->clear();
	
	int k = 0;	
	for (vector<ts_terminfo_t>::iterator  term_it = term_list.begin();
		term_it != term_list.end();++term_it)
	{
	
		if (0 == k) {
			if (1 == term_list.size())
			{
				ts_ind_reader_t::iterator ind_it2 = 
						ind_reader.begin(term_it->term,TS_INDEX_SHORTEST_NUM);
				lst3->resize(ind_it2.size());
				std::copy(ind_it2,ind_reader.end(),lst3->begin());
			}

		} else if (1 == k) {

			ts_ind_reader_t::iterator ind_it1 = 
							ind_reader.begin(term_list.begin()->term,TS_INDEX_SHORTEST_NUM);
			ts_ind_reader_t::iterator ind_it2 = ind_reader.begin(term_it->term,TS_INDEX_MAXNUM);;

			while (ind_it1 != ind_reader.end() && ind_it2 != ind_reader.end())
			{
				ts_index_t& ind1 = *ind_it1;
				ts_index_t& ind2 = *ind_it2;

				if (ind1.id > ind2.id)
				{
					++ind_it2;
				} else if (ind1.id < ind2.id){
					++ind_it1;
				} else {
					ts_index_t tmp ;
					tmp.id = ind1.id;
					tmp.sweight = ind1.sweight + ind2.sweight;
					lst3->push_back(tmp);			
					++ind_it1;
					++ind_it2;
				}

			}
		} else {
			vector < ts_index_t >::iterator ind_it1 = lst1->begin();
			ts_ind_reader_t::iterator ind_it2 = ind_reader.begin(term_it->term,TS_INDEX_MAXNUM);;
			while (ind_it1 != lst1->end() && ind_it2 != ind_reader.end())
			{
				ts_index_t& ind1 = *ind_it1;
				ts_index_t& ind2 = *ind_it2;
				if (ind1.id > ind2.id)
				{
					++ind_it2;
				} else if (ind1.id < ind2.id){
					++ind_it1;
				} else {
					ts_index_t tmp ;
					tmp.id = ind1.id;
					tmp.sweight = ind1.sweight + ind2.sweight;
					lst3->push_back(tmp);			
					++ind_it1;
					++ind_it2;
				}

			}
		}
		{///swap
			vector < ts_index_t > *tmp = lst1;
			lst1 = lst3;
			lst3 = tmp;
			lst3->clear();
		}
		k++;
	}
	int old_size = merged_list->size();
	merged_list->resize(old_size+lst1->size());
	std::copy(lst1->begin(),lst1->end(),merged_list->begin()+old_size);

	return 0;
}


int DEFAULT_QUERY_HANDLE::index_filt(const pointer_t query_cmd,
                                     vector < ts_index_t > *merged_list,
                                     vector < ts_index_t > *filted_list)
{
    int opret = 0;
    nshead_t *req_nshead = (nshead_t *) query_cmd;
    if(req_nshead->body_len < sizeof(mc_pack_t) + sizeof(ts_head_t))
    {
        return -1;
    }
    ts_head_t *req_head = (ts_head_t *) & req_nshead[1];
    mc_pack_t *mc = (mc_pack_t *) & req_head[1];

    uint32 itmp = 0;
    opret = mc_pack_get_uint32(mc, "itmp", &itmp);
    if(opret < 0)
    {
        return -1;
    }
    if(itmp != 0 && itmp != 1)
    {
        return -2;
    }

    char brief[5];
    for(vector < ts_index_t >::iterator iter = merged_list->begin();
        iter != merged_list->end(); ++iter)
    {
        get_brief(iter->id, brief);
        if(brief[0] == (int)itmp)
        {
            //std::copy(iter,iter,filted_list.end());
            filted_list->push_back(*iter);
        }
    };

    return 0;
}


int DEFAULT_QUERY_HANDLE::adjust_weight(const pointer_t query_cmd,
                                        const vector < ts_terminfo_t >
                                        &term_list, 
                                        vector < ts_index_t > *filted_list)
{
    return 0;
}




int DEFAULT_QUERY_HANDLE::add_abs(const pointer_t query_cmd, int i,
                                  const ts_buffer_t & fts,
                                  const ts_buffer_t & brs, ts_buffer_t & abs)
{
    char name[16];
    snprintf(name, sizeof(name), "result%d", i);

    //mc_pack_t *mc;
    //if(0 == i)
    //{
        //mc = mc_pack_create(abs._buf, abs._size);
    //} else
    //{
        //mc = (mc_pack_t *) abs._buf;
		//mc_pack_reopen 	(mc,abs._size);  
	//}
    //TODO:1、输入i没作用了；2、每次都open mcpack性能可以能有问题
    bsl::ResourcePool rp;
    mc_pack_t *mc = mc_pack_open_rw_rp(abs._buf, abs._size, &rp);
    if(MC_PACK_PTR_ERR(mc)){
        UB_LOG_WARNING("[function:parse_query>>mc_pack_open_r_rp][ret:][desc:mc_pack_open_r_rp error][intput:]");
        return -2;
    }

    mc_pack_t *tmp = mc_pack_put_object(mc, name);
    if(MC_PACK_PTR_ERR(tmp) < 0)
    {
        return MC_PACK_PTR_ERR(tmp);
    }
    mc_pack_copy_item((mc_pack_t *) fts._buf, tmp, NULL);
    mc_pack_close(mc);
    abs._used = mc_pack_get_size(mc);

    return 0;
}


int DEFAULT_QUERY_HANDLE::init_abs(const pointer_t query_cmd,ts_buffer_t &result) 
{
	return 0;
}


int DEFAULT_QUERY_HANDLE::fini_abs(const pointer_t query_cmd,ts_buffer_t &result) 
{
	return 0;
}



int DEFAULT_UPDATE_HANDLE::parse_add(pointer_t update_cmd,
                                     uint32 & id,
                                     vector < ts_terminfo_t > &term_list,
                                     ts_buffer_t & brief,
                                     ts_buffer_t & fulltext)
{

    int opret = 0;

    nshead_t *req_nshead = (nshead_t *) update_cmd;
    if(req_nshead->body_len < sizeof(ts_head_t))
    {
        return -1;
    }
    ts_head_t *req_head = (ts_head_t *) & req_nshead[1];
    mc_pack_t *mc = (mc_pack_t *) & req_head[1];




    //id
    opret = mc_pack_get_uint32(mc, "id", &id);
    if(opret < 0)
    {
        return opret;
    }
    //brief
    uint32 itmp = 0;
    opret = mc_pack_get_uint32(mc, "brief", &itmp);
    if(opret < 0)
    {
        return opret;
    }
    if(itmp != 0 && itmp != 1)
    {
        return -4;
    }
    brief._buf[0] = itmp;
	brief._used = 1;

    //fulltext
	if (mc_pack_get_size(mc) > (int)fulltext._size)
	{
		return -5;
	}
    memcpy(fulltext._buf, mc, mc_pack_get_size(mc));
    fulltext._used = mc_pack_get_size(mc);

    //term_list
    const char *keyword = mc_pack_get_str(mc, "keyword");
    if(MC_PACK_PTR_ERR(keyword))
    {
        return -6;
    }

    const char delimiters[] = " .,;:!-";
    char *running;
    char *token;
    running = strdup(keyword);
    ts_terminfo_t terminfo;
    token = strsep(&running, delimiters);
    while(NULL != token)
    {
        creat_sign_fs64(token, strlen(token), &terminfo.term.sign1,
                        &terminfo.term.sign2);
        term_list.push_back(terminfo);
        token = strsep(&running, delimiters);
        UB_LOG_DEBUG("uterm [%u:%u]", terminfo.term.sign1,
                     terminfo.term.sign2);
    }
	if (running)
		free(running);
    return 0;
}


int DEFAULT_UPDATE_HANDLE::parse_mod(pointer_t update_cmd,
                                     uint32 & id,
                                     vector < ts_terminfo_t > &termlist,
                                     ts_buffer_t & brief,
                                     ts_buffer_t & fulltext)
{
    return parse_add(update_cmd,id,termlist,brief,fulltext);
}

int DEFAULT_UPDATE_HANDLE::parse_modbasic(pointer_t update_cmd,
                                          uint32 & id, ts_buffer_t & brief)
{

	int opret = 0;

	nshead_t *req_nshead = (nshead_t *) update_cmd;
	if(req_nshead->body_len < sizeof(ts_head_t))
	{
		return -1;
	}
	ts_head_t *req_head = (ts_head_t *) & req_nshead[1];
	mc_pack_t *mc = (mc_pack_t *) & req_head[1];




	//id
	opret = mc_pack_get_uint32(mc, "id", &id);
	if(opret < 0)
	{
		return opret;
	}
	//brief
	uint32 itmp = 0;
	opret = mc_pack_get_uint32(mc, "brief", &itmp);
	if(opret < 0)
	{
		return opret;
	}
	if(itmp != 0 && itmp != 1)
	{
		return -4;
	}
	brief._buf[0] = itmp;
	brief._used = 1;

	return 0;
}

int DEFAULT_UPDATE_HANDLE::parse_del(pointer_t update_cmd,
		vector < uint32 > &id_list)
{
	nshead_t *req_nshead = (nshead_t *) update_cmd;
    if(req_nshead->body_len < sizeof(ts_head_t))
    {
        return -1;
    }
    ts_head_t *req_head = (ts_head_t *) & req_nshead[1];
    uint32 *ids = (uint32 *) & req_head[1];
    uint32 num = (req_nshead->body_len - sizeof(ts_head_t)) / sizeof(uint32);
    id_list.resize(num);
    std::copy(ids, ids + num, id_list.begin());

    return 0;
}


int DEFAULT_UPDATE_HANDLE::parse_undel(pointer_t update_cmd,
                                       vector < uint32 > &id_list)
{
    return parse_del(update_cmd, id_list);
}




/* vim: set ts=4 sw=4 sts=4 tw=100 */
