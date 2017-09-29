/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "StackTrace.h"
#include "MQTTPacket.h"

#include <string.h>


const char* MQTTPacket_names[] =
{
	"RESERVED", "CONNECT", "CONNACK", "PUBLISH", "PUBACK", "PUBREC", "PUBREL",
	"PUBCOMP", "SUBSCRIBE", "SUBACK", "UNSUBSCRIBE", "UNSUBACK",
	"PINGREQ", "PINGRESP", "DISCONNECT"
};


const char* MQTTPacket_getName(unsigned short packetid)
{
	return MQTTPacket_names[packetid];
}


int MQTTStringFormat_connack(char* strbuf, int strbuflen, unsigned char connack_rc, unsigned char sessionPresent)
{
	int strindex = snprintf(strbuf, strbuflen, "CONNACK session present %d, rc %d", sessionPresent, connack_rc);
	return strindex;
}


int MQTTStringFormat_ack(char* strbuf, int strbuflen, unsigned char packettype, unsigned char dup, unsigned short packetid)
{
	int strindex = snprintf(strbuf, strbuflen, "%s, packet id %d", MQTTPacket_names[packettype], packetid);
	if (dup)
		strindex += snprintf(strbuf + strindex, strbuflen - strindex, ", dup %d", dup);
	return strindex;
}


int MQTTStringFormat_suback(char* strbuf, int strbuflen, unsigned short packetid, int count, int* grantedQoSs)
{
	return snprintf(strbuf, strbuflen,
		"SUBACK packet id %d count %d granted qos %d", packetid, count, grantedQoSs[0]);
}


#if defined(MQTT_CLIENT)
char* MQTTFormat_toClientString(char* strbuf, int strbuflen, unsigned char* buf, int buflen)
{
	int index = 0;
	int rem_length = 0;
	MQTTHeader header = {0};
	int strindex = 0;

	header.byte = buf[index++];
	index += MQTTPacket_decodeBuf(&buf[index], &rem_length);

	switch (header.bits.type)
	{

	case CONNACK:
	{
		unsigned char sessionPresent, connack_rc;
		if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, buf, buflen) == 1)
			strindex = MQTTStringFormat_connack(strbuf, strbuflen, connack_rc, sessionPresent);
	}
	break;
	case PUBLISH:
	{
		unsigned char dup, retained, *payload;
		unsigned short packetid;
		int qos, payloadlen;
		MQTTString topicName = MQTTString_initializer;
		if (MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName,
				&payload, &payloadlen, buf, buflen) == 1)
			strindex = MQTTStringFormat_publish(strbuf, strbuflen, dup, qos, retained, packetid,
					topicName, payload, payloadlen);
	}
	break;
	case PUBACK:
	case PUBREC:
	case PUBREL:
	case PUBCOMP:
	{
		unsigned char packettype, dup;
		unsigned short packetid;
		if (MQTTDeserialize_ack(&packettype, &dup, &packetid, buf, buflen) == 1)
			strindex = MQTTStringFormat_ack(strbuf, strbuflen, packettype, dup, packetid);
	}
	break;
	case SUBACK:
	{
		unsigned short packetid;
		int maxcount = 1, count = 0;
		int grantedQoSs[1];
		if (MQTTDeserialize_suback(&packetid, maxcount, &count, grantedQoSs, buf, buflen) == 1)
			strindex = MQTTStringFormat_suback(strbuf, strbuflen, packetid, count, grantedQoSs);
	}
	break;
	case UNSUBACK:
	{
		unsigned short packetid;
		if (MQTTDeserialize_unsuback(&packetid, buf, buflen) == 1)
			strindex = MQTTStringFormat_ack(strbuf, strbuflen, UNSUBACK, 0, packetid);
	}
	break;
	case PINGREQ:
	case PINGRESP:
	case DISCONNECT:
		strindex = snprintf(strbuf, strbuflen, "%s", MQTTPacket_names[header.bits.type]);
		break;
	}
	return strbuf;
}
#endif

#if defined(MQTT_SERVER)
char* MQTTFormat_toServerString(char* strbuf, int strbuflen, unsigned char* buf, int buflen)
{
	int index = 0;
	int rem_length = 0;
	MQTTHeader header = {0};
	int strindex = 0;

	header.byte = buf[index++];
	index += MQTTPacket_decodeBuf(&buf[index], &rem_length);

	switch (header.bits.type)
	{
	case CONNECT:
	{
		MQTTPacket_connectData data;
		int rc;
		if ((rc = MQTTDeserialize_connect(&data, buf, buflen)) == 1)
			strindex = MQTTStringFormat_connect(strbuf, strbuflen, &data);
	}
	break;
	case PUBLISH:
	{
		unsigned char dup, retained, *payload;
		unsigned short packetid;
		int qos, payloadlen;
		MQTTString topicName = MQTTString_initializer;
		if (MQTTDeserialize_publish(&dup, &qos, &retained, &packetid, &topicName,
				&payload, &payloadlen, buf, buflen) == 1)
			strindex = MQTTStringFormat_publish(strbuf, strbuflen, dup, qos, retained, packetid,
					topicName, payload, payloadlen);
	}
	break;
	case PUBACK:
	case PUBREC:
	case PUBREL:
	case PUBCOMP:
	{
		unsigned char packettype, dup;
		unsigned short packetid;
		if (MQTTDeserialize_ack(&packettype, &dup, &packetid, buf, buflen) == 1)
			strindex = MQTTStringFormat_ack(strbuf, strbuflen, packettype, dup, packetid);
	}
	break;
	case SUBSCRIBE:
	{
		unsigned char dup;
		unsigned short packetid;
		int maxcount = 1, count = 0;
		MQTTString topicFilters[1];
		int requestedQoSs[1];
		if (MQTTDeserialize_subscribe(&dup, &packetid, maxcount, &count,
				topicFilters, requestedQoSs, buf, buflen) == 1)
			strindex = MQTTStringFormat_subscribe(strbuf, strbuflen, dup, packetid, count, topicFilters, requestedQoSs);;
	}
	break;
	case UNSUBSCRIBE:
	{
		unsigned char dup;
		unsigned short packetid;
		int maxcount = 1, count = 0;
		MQTTString topicFilters[1];
		if (MQTTDeserialize_unsubscribe(&dup, &packetid, maxcount, &count, topicFilters, buf, buflen) == 1)
			strindex =  MQTTStringFormat_unsubscribe(strbuf, strbuflen, dup, packetid, count, topicFilters);
	}
	break;
	case PINGREQ:
	case PINGRESP:
	case DISCONNECT:
		strindex = snprintf(strbuf, strbuflen, "%s", MQTTPacket_names[header.bits.type]);
		break;
	}
	strbuf[strbuflen] = '\0';
	return strbuf;
}
#endif
