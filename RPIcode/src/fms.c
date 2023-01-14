#define _GNU_SOURCE
#define PCRE2_CODE_UNIT_WIDTH 8

//verbose import
#include "verbose_print.h"

//struct imports
#include "messagestruct.h"

//imports
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <inttypes.h>

#include "crc_table.h"

#define RED 0
#define BLUE 1

CURL *curl;
char* IP;

char* red_q[5] = { "", "", "", "", "" };
char* blue_q[5] = { "", "", "", "", "" };
uint32_t last_crc = 0;
int question_changed = 0;

uint32_t crc32(const uint8_t data[], size_t data_length) {
	uint32_t crc = 0xFFFFFFFFu;

	for (size_t i = 0; i < data_length; i++) {
		const uint32_t lookupIndex = (crc ^ data[i]) & 0xff;
		crc = (crc >> 8) ^ crc_table[lookupIndex];  // crc_table is an array of 256 32-bit constants
	}

	// Finalize the CRC-32 value by inverting all the bits
	crc ^= 0xFFFFFFFFu;
	return crc;
}

size_t data_process(char* buff, size_t item_size, size_t items, void* ignore)
{
    int team = RED;
    int line = 0;
    int state = 0; //0 = control, 1 = read, 2 = start

    size_t bytes = item_size * items;
    int offset = 0;

    for (int i = 0; i < bytes; i++)
    {
        if (state == 1)
        {
            if (buff[i] == '<' || buff[i] == '>' || buff[i] == '\n') {
                switch (team)
                {
                    case RED: asprintf(&red_q[line], "%.*s", i-offset, buff+offset);
                        break;
                    case BLUE: asprintf(&blue_q[line], "%.*s", i-offset, buff+offset);
                        break;
                }
                line++;
            }
        }

        if (buff[i] == '<' || buff[i] == '>' || buff[i] == '\n') { state = buff[i] == '<' ? 0 : 2; continue; }

        if (state == 2)
        {
            if (buff[i] == ']')
            {
                team = RED;
                line = 0;
            }
            else if (buff[i] == '|')
            {
                team = BLUE;
                line = 0;
            }
            else if (buff[i] > ' ')
            {
                state = 1;
                offset = i;
            }
        }
    }

    uint32_t crc = crc32(buff, bytes);
    if (crc != last_crc) { question_changed = 1; }
    last_crc = crc;

    return bytes;
}

void network_setup(char* ip)
{
    curl = curl_easy_init();
    IP = ip;
}

void network_get()
{
    char* ip_buff;
    asprintf(&ip_buff, "http://%s/Matches/Display", IP);
    //asprintf(&ip_buff, "http://%s/FMS/stemq.html", IP);
    curl_easy_setopt(curl, CURLOPT_URL, ip_buff);
    curl_easy_setopt(curl,CURLOPT_HTTPGET , (long) 0);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_process);
    curl_easy_perform(curl);
}

void network_post(message msg)
{
    char* ip_buff;
    char* post_buff;
    if (msg.cmd == cmd_beacon)
    {
        asprintf(&ip_buff, "http://%s/Beacons/Update", IP);
        asprintf(&post_buff, "Id=%d&state=%c", msg.id, msg.team);
    }
    else if (msg.cmd == cmd_stemq)
    {
        asprintf(&ip_buff, "http://%s/Matches/Reset", IP);
        asprintf(&post_buff, "reset=%c&Id=", msg.team);
    }
    curl_easy_setopt(curl, CURLOPT_URL, ip_buff);
    curl_easy_setopt(curl,CURLOPT_POST, (long) 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_buff);
    curl_easy_perform(curl);
}

void network_close()
{
    curl_easy_cleanup(curl);
}
