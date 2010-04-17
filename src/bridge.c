/*
Copyright (c) 2009,2010 Roger Light <roger@atchoo.org>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of mosquitto nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>

#include <config.h>
#include <mqtt3.h>

int mqtt3_bridge_new(mqtt3_context **contexts, int *context_count, struct _mqtt3_bridge *bridge)
{
	int i;
	int new_sock = -1;
	mqtt3_context *new_context = NULL;
	mqtt3_context **tmp_contexts;

	if(!contexts || !bridge) return 1;

	new_sock = mqtt3_socket_connect(bridge->address, bridge->port);
	if(new_sock == -1){
		mqtt3_log_printf(MQTT3_LOG_ERR, "Error creating bridge.");
		return 1;
	}

	new_context = mqtt3_context_init(new_sock);
	if(!new_context){
		return 1;
	}
	new_context->bridge = bridge;
	for(i=0; i<(*context_count); i++){
		if(contexts[i] == NULL){
			contexts[i] = new_context;
			break;
		}
	}
	if(i==(*context_count)){
		(*context_count)++;
		tmp_contexts = mqtt3_realloc(contexts, sizeof(mqtt3_context*)*(*context_count));
		if(tmp_contexts){
			contexts = tmp_contexts;
			contexts[(*context_count)-1] = new_context;
		}
	}

	new_context->id = mqtt3_strdup(bridge->name);
	mqtt3_raw_connect(new_context, new_context->id,
			/*will*/ false, /*will qos*/ 0, /*will retain*/ false, /*will topic*/ NULL, /*will msg*/ NULL,
			60/*keepalive*/, /*cleanstart*/true);

	return 0;
}
