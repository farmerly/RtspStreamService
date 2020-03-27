// =====================================================================================
//  Copyright (C) 2020 by Jiaxing Shao. All rights reserved
//
//  文 件 名:  sdp_codec.cpp
//  作    者:  Jiaxing Shao, 13315567369@163.com
//  版 本 号:  1.0
//  创建时间:  2020年03月25日
//  Compiler:  g++
//  描    述:  
// =====================================================================================

#include "sdp_codec.h"

struct sdp_payload *sdp_parser(const char *payload)
{
	sdp_payload *sdp = (sdp_payload*)malloc(sizeof(sdp_payload));
	if(sdp == NULL){
		goto fail;
	}

	return sdp;

fail:
	sdp_destroy(sdp);
	return NULL;
}

std::string str_format(const char *fmt, ...)
{
	char str_bytes[1024] = { 0 };
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(str_bytes, 1024, fmt, ap);
	va_end(ap);
	std::string str_fmt = str_bytes;
	return str_fmt;
}

std::string sdp_format(const struct sdp_payload *sdp)
{
	if(sdp == NULL){
		return NULL;
	}

	std::string sdp_data;
	sdp_data += str_format("v=%d\r\n", sdp->proto_version);
	sdp_data += str_format("o=%s %ld %ld %s %s %s\r\n", sdp->origin.username, sdp->origin.sess_id, sdp->origin.sess_version, 
			sdp->origin.nettype, sdp->origin.addrtype, sdp->origin.addr);
	sdp_data += str_format("s=%s\r\n", sdp->session_name);
	if(sdp->session_info){
		sdp_data += str_format("i=%s\r\n", sdp->session_info);
	}
	if(sdp->uri){
		sdp_data += str_format("u=%s\r\n", sdp->uri);
	}
	for(int i = 0; i < sdp->emails_count; i++){
		sdp_data += str_format("e=%s\r\n", sdp->emails[i]);
	}
	for(int i = 0; i < sdp->phones_count; i++){
		sdp_data += str_format("p=%s\r\n", sdp->phones[i]);
	}
	if(sdp->conn.nettype || sdp->conn.addrtype || sdp->conn.address){
		sdp_data += str_format("c=%s %s %s\r\n", sdp->conn.nettype, sdp->conn.addrtype, sdp->conn.address);
	}
	for(int i = 0; i < sdp->bw_count; i++){
		sdp_data += str_format("b=%s %s\r\n", sdp->bw[i].bwtype, sdp->bw[i].bandwidth);
	}
	for(int i = 0; i < sdp->times_count; i++){
		sdp_data += str_format("t=%ld %ld\r\n", sdp->times[i].starttime, sdp->times[i].stoptime);
		for(int j = 0; j < sdp->times[i].repeat_count; j++){
			sdp_data += str_format("r=%ld %ld");
			for(int k = 0; k < sdp->tims[i].repeat[j].offsets_count; k++){
				sdp_data += str_format(" %d", sdp->times[i].repeat[j].offsets[k]);
			}
			sdp_data += "\r\n";
		}
	}
	if(sdp->timezone_adj){
		sdp_data += "z=";
		for(int i = 0; i < sdp->timezone_adj_count; i++){
			sdp_data += str_format("%d %d ", sdp->timezone_adj[i].adject, sdp->timezone_adj[i].offset);
		}
		sdp_data += "\r\n";
	}
	if(sdp->encrypt.method){
		if(sdp->encrypt.key){
			sdp_data += str_format("k=%s:%s\r\n", sdp->encrypt.method, sdp->encrypt.key);
		}else{
			sdp_data += str_format("k=%s\r\n", sdp->encrypt.method);
		}
	}
	for(int i = 0; i < sdp->attributes_count; i++){
		sdp_data += str_format("a=%s\r\n", sdp->attributes[i]);
	}
	for(int i = 0; i < sdp->medias_count; i++){
		if(sdp->medias[i].info.port_n){
			sdp_data += str_format("m=%s %d/%d %s", sdp->medias[i].info.type, sdp->medias[i].info.port, sdp->medias[i].info.port_n, sdp->medias[i].info.proto);
		}else{
			sdp_data += str_format("m=%s %d %s", sdp->medias[i].info.type, sdp->medias[i].info.port, sdp->medias[i].info.proto);
		}
		for(int j = 0; j < sdp->medias[i].info.fmt_count; j++){
			sdp_data += str_format(" %d", sdp->medias[i].info.fmt[j]);
		}
		sdp_data += "\r\n";
		if(sdp->medias[i].title){
			sdp_data += str_format("i=%s\r\n", sdp->medias[i].title);
		}
		if(sdp->medias[i].conn.nettype || sdp->medias[i].conn.addrtype || sdp->medias[i].conn.address){
			sdp_data += str_format("c=%s %s %s\r\n", sdp->medias[i].conn.nettype, sdp->medias[i].conn.addrtype, sdp->medias[i].conn.address);
		}
		for(int j = 0; j < sdp->medias[i]->bw_count; j++){
			sdp_data += str_format("b=%s %s\r\n", sdp->medias[i]->bw[j].bwtype, sdp->medias[i]->bw[j].bandwidth);
		}
		if(sdp->medias[i]->encrypt.method){
			if(sdp->medias[i]->encrypt.key){
				sdp_data += str_format("k=%s:%s\r\n", sdp->medias[i]->encrypt.method, sdp->medias[i]->encrypt.key);
			}else{
				sdp_data += str_format("k=%s\r\n", sdp->medias[i]->encrypt.method);
			}
		}
		for(int j = 0; j < sdp->medisa[i]->attributes_count; j++){
			sdp_data += str_format("a=%s\r\n", sdp->medias[i]->attributes[j]);
		}
	}
	return str_data;
}

void sdp_destroy(struct sdp_payload *sdp)
{
	if(sdp == NULL){
		free(sdp->origin.username);
		free(sdp->origin.nettype);
		free(sdp->origin.addrtype);
		free(sdp->origin.addr);
		free(sdp->session_name);
		free(sdp->session_info);
		free(sdp->uri);
		free(sdp->emails);
		free(sdp->phones);
		free(sdp->conn.nettype);
		free(sdp->conn.addrtype);
		free(sdp->conn.address);
		for(int i = 0; i < sdp->bw_count; i++){
			free(sdp->bw[i].bwtype);
			free(sdp->bw[i].bandwidth);
		}
		free(sdp->bw);
		for(int i = 0; i < sdp->times_count; i++){
			for(int j = 0; j < sdp->times[i].repeat_count; j++){
				free(sdp->times[i].repeat[j].offsets);
			}
			free(sdp->times[i].repeat);
		}
		free(sdp->times);
		free(sdp->timezone_adj);
		free(sdp->encrypt->method);
		free(sdp->encrypt->key);
		free(sdp->attributes);
		for(int i = 0; i < medias_count; i++){
			free(sdp->medias[i].titles);
			for(int j = 0; j < sdp->medias[i].bw_count; j++){
				free(sdp->medias[i].bw[j].bwtype);
				free(sdp->medias[i].bw[j].bandwidth);
			}
			free(sdp->medias[i].encrypt->method);
			free(sdp->medias[i].encrypt->key);
			free(sdp->medias[i].attributes);
		}
		free(sdp->medias);
	}
	free(sdp);
}
