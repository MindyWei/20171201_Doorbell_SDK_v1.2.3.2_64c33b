#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ite_avienc.h"

#define ADD_IN_BUFFER_FIRST 1
#if (ADD_IN_BUFFER_FIRST)
#define BUF_FRAMES_COUNT 20
#define MAX_BUF_FRAMES_SIZE 256*1024*20 //30
static char *buf_frames;
static int  buf_frames_size = 0;
#endif
static void write_short(FILE *out, unsigned int num)
{
  char lilend_num[2];

  lilend_num[0] = num&0xff;
  lilend_num[1] = (num>>8)&0xff;

  fwrite(lilend_num, 2, 1, out);
}

static void write_int(FILE *out, unsigned int num)
{
  char lilend_num[4];

  lilend_num[0] = num&0xff;
  lilend_num[1] = (num>>8)&0xff;
  lilend_num[2] = (num>>16)&0xff;
  lilend_num[3] = (num>>24)&0xff;

  fwrite(lilend_num, 4, 1, out);
}

static int write_avi_header(FILE *out, struct ite_avi_header_t *avi_header)
{
  int marker,t;

  fwrite("avih", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);

  write_int(out,avi_header->time_delay);
  write_int(out,avi_header->data_rate);
  write_int(out,avi_header->reserved);
  write_int(out,avi_header->flags);
  write_int(out,avi_header->number_of_frames);
  write_int(out,avi_header->initial_frames);
  write_int(out,avi_header->data_streams);
  write_int(out,avi_header->buffer_size);
  write_int(out,avi_header->width);
  write_int(out,avi_header->height);
  write_int(out,avi_header->time_scale);
  write_int(out,avi_header->playback_data_rate);
  write_int(out,avi_header->starting_time);
  write_int(out,avi_header->data_length);

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}

static int write_stream_header(FILE *out, struct ite_stream_header_t *stream_header)
{
  int marker,t;

  fwrite("strh", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);

  fwrite(stream_header->data_type, 4, 1, out);
  fwrite(stream_header->codec, 4, 1, out);
  write_int(out,stream_header->flags);
  write_int(out,stream_header->priority);
  write_int(out,stream_header->initial_frames);
  write_int(out,stream_header->time_scale);
  write_int(out,stream_header->data_rate);
  write_int(out,stream_header->start_time);
  write_int(out,stream_header->data_length);
  write_int(out,stream_header->buffer_size);
  write_int(out,stream_header->quality);
  write_int(out,stream_header->sample_size);
  write_int(out,0);
  write_int(out,0);

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}

static int write_stream_format_v(FILE *out, struct ite_stream_format_v_t *stream_format_v)
{
  int marker,t;

  fwrite("strf", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);

  write_int(out,stream_format_v->header_size);
  write_int(out,stream_format_v->width);
  write_int(out,stream_format_v->height);
  write_int(out,stream_format_v->num_planes + stream_format_v->bits_per_pixel*256*256);
  write_int(out,stream_format_v->compression_type);
  write_int(out,stream_format_v->image_size);
  write_int(out,stream_format_v->x_pels_per_meter);
  write_int(out,stream_format_v->y_pels_per_meter);
  write_int(out,stream_format_v->colors_used);
  write_int(out,stream_format_v->colors_important);

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}

static int write_stream_format_a(FILE *out, struct ite_stream_format_a_t *stream_format_a)
{
  int marker,t;

  fwrite("strf", 4, 1, out);
  marker=ftell(out);
  write_int(out,0);

  write_short(out,stream_format_a->format_type);
  write_short(out,stream_format_a->channels);
  write_int(out,stream_format_a->sample_rate);
  write_int(out,stream_format_a->bytes_per_second);
  write_short(out,stream_format_a->block_align);
  write_short(out,stream_format_a->bits_per_sample);
  write_short(out,stream_format_a->size);

  t=ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}


static int write_junk_chunk(FILE *out)
{
  int marker,t;
  int r,l;
  char *junk={ "JUNK IN THE CHUNK! " };

  fwrite("JUNK", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);

  r = 4096 - ftell(out);
  l = strlen(junk);
  t = 0;
  while(t < r)
  {
    if(t + l <= r)
        fwrite(junk, l, 1, out);
    else
        fwrite(junk, r-t, 1, out);
    t = t + l;
  }

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}

static int write_avi_header_chunk(struct ite_avi_t *avi)
{
  long marker,t;
  long sub_marker;
  FILE *out = avi->out;

  fwrite("LIST", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);
  fwrite("hdrl", 4, 1, out);
  write_avi_header(out,&avi->avi_header);

  fwrite("LIST", 4, 1, out);
  sub_marker = ftell(out);
  write_int(out,0);
  fwrite("strl", 4, 1, out);
  write_stream_header(out,&avi->stream_header_v);
  write_stream_format_v(out,&avi->stream_format_v);

  t = ftell(out);
  fseek(out,sub_marker,SEEK_SET);
  write_int(out,t-sub_marker-4);
  fseek(out,t,SEEK_SET);

  if (avi->avi_header.data_streams==2)
  {
    fwrite("LIST", 4, 1, out);//write_chars_bin(out,"LIST",4);
    sub_marker=ftell(out);
    write_int(out,0);
    fwrite("strl", 4, 1, out);//write_chars_bin(out,"strl",4);
    write_stream_header(out,&avi->stream_header_a);
    write_stream_format_a(out,&avi->stream_format_a);

    t = ftell(out);
    fseek(out,sub_marker,SEEK_SET);
    write_int(out,t-sub_marker-4);
    fseek(out,t,SEEK_SET);
  }

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  write_junk_chunk(out);

  return 0;
}

static int write_index(FILE *out, int count, unsigned int *offsets)
{
  int marker,t;
  int offset=4;

  if (offsets==0) return -1;

  fwrite("idx1", 4, 1, out);
  marker = ftell(out);
  write_int(out,0);

  for(t=0; t<count; t++)
  {
    if ((offsets[t]&0x80000000)==0)
    {
      fwrite("00dc", 4, 1, out);
    }
    else
    {
      fwrite("01wb", 4, 1, out);
      offsets[t]&=0x7fffffff;
    }  
    write_int(out,0x10);
    write_int(out,offset);
    write_int(out,offsets[t]);
    offset = offset + offsets[t] + 8;
  }

  t = ftell(out);
  fseek(out,marker,SEEK_SET);
  write_int(out,t-marker-4);
  fseek(out,t,SEEK_SET);

  return 0;
}

struct ite_avi_t *ite_avi_open(char *filename, int width, int height, char *fourcc, int fps, struct ite_avi_audio_t *audio)
{
  struct ite_avi_t *avi;
  FILE *out;

  out=fopen(filename,"wb");
  if (out==0)
  {
	printf("ite_avi_open: fopen(%s) error! \r\n", filename);
    return 0;
  }

#if (ADD_IN_BUFFER_FIRST)
  buf_frames = (char *)malloc(MAX_BUF_FRAMES_SIZE);
#endif
  avi=(struct ite_avi_t *)malloc(sizeof(struct ite_avi_t));
  memset(avi,0,sizeof(struct ite_avi_t));

  avi->out=out;

  /* set avi header */
  avi->avi_header.time_delay=1000000/fps;
  avi->avi_header.data_rate=0;
  avi->avi_header.flags=0x10;
  if (audio==NULL)
  {
    avi->avi_header.data_streams=1;
  }
  else
  {
    avi->avi_header.data_streams=2;
  }
  avi->avi_header.number_of_frames=0; //FIXME - this needs to be reset
  avi->avi_header.width=width;
  avi->avi_header.height=height;
  avi->avi_header.buffer_size=0;

  /* set stream header */
  strcpy(avi->stream_header_v.data_type,"vids");
  memcpy(avi->stream_header_v.codec,fourcc,4);
  avi->stream_header_v.time_scale=1;
  avi->stream_header_v.data_rate=fps;
  avi->stream_header_v.buffer_size=0;
  avi->stream_header_v.data_length=0;

  /* set stream format */
  avi->stream_format_v.header_size=40;
  avi->stream_format_v.width=width;
  avi->stream_format_v.height=height;
  avi->stream_format_v.num_planes=1;
  avi->stream_format_v.bits_per_pixel=24;
  avi->stream_format_v.compression_type=((unsigned int)fourcc[3]<<24)+
                                       ((unsigned int)fourcc[2]<<16)+
                                       ((unsigned int)fourcc[1]<<8)+
                                       ((unsigned int)fourcc[0]);
  avi->stream_format_v.image_size=width*height*3;
  avi->stream_format_v.colors_used=0;
  avi->stream_format_v.colors_important=0;

  avi->stream_format_v.palette=0;
  avi->stream_format_v.palette_count=0;

  if (avi->avi_header.data_streams==2)
  {
    /* set stream header */
    strcpy(avi->stream_header_a.data_type,"auds");
    avi->stream_header_a.codec[0]=1;
    avi->stream_header_a.codec[1]=0;
    avi->stream_header_a.codec[2]=0;
    avi->stream_header_a.codec[3]=0;
    avi->stream_header_a.time_scale=1;
    avi->stream_header_a.data_rate=audio->samples_per_second;
    avi->stream_header_a.buffer_size=audio->channels*(audio->bits/8)*audio->samples_per_second;
    avi->stream_header_a.quality=-1;
    avi->stream_header_a.sample_size=(audio->bits/8)*audio->channels;

    /* set stream format */
    avi->stream_format_a.format_type=1;
    avi->stream_format_a.channels=audio->channels;
    avi->stream_format_a.sample_rate=audio->samples_per_second;
    avi->stream_format_a.bytes_per_second=audio->channels*(audio->bits/8)*audio->samples_per_second;
    avi->stream_format_a.block_align=audio->channels*(audio->bits/8);;
    avi->stream_format_a.bits_per_sample=audio->bits;
    avi->stream_format_a.size=0;
  }

  fwrite("RIFF", 4, 1, out); 
  write_int(out,0);
  fwrite("AVI ", 4, 1, out);
  write_avi_header_chunk(avi);

  fwrite("LIST", 4, 1, out);
  avi->marker=ftell(out);
  write_int(out,0);
  fwrite("movi", 4, 1, out);

  avi->offsets_len=1024;
  avi->offsets=malloc(avi->offsets_len*sizeof(int));
  avi->offsets_ptr=0;

  return avi;
}

#if (ADD_IN_BUFFER_FIRST)
void ite_avi_add_frame(struct ite_avi_t *avi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* if your frame is raggin, give it some paddin' */
  int t;
  char *pad_buffer;

  avi->offset_count++;
  avi->stream_header_v.data_length++;

  if(avi->stream_header_v.data_length % BUF_FRAMES_COUNT == 1 && avi->stream_header_v.data_length != 1)
  {
    //printf(">>>>%d\n", buf_frames_size);
    fwrite(buf_frames, buf_frames_size, 1, avi->out);
    memset(buf_frames, 0, MAX_BUF_FRAMES_SIZE);
    buf_frames_size = 0;
  }

  maxi_pad=len%4;
  if (maxi_pad>0) maxi_pad=4-maxi_pad;

  if (avi->offset_count >= avi->offsets_len)
  {
    avi->offsets_len+=1024;
    avi->offsets=realloc(avi->offsets,avi->offsets_len*sizeof(int));
  }

  avi->offsets[avi->offsets_ptr++]=len+maxi_pad;

  memcpy(buf_frames + buf_frames_size, "00dc", 4);
  buf_frames_size += 4;
  {
    char lilend_num[4];
    int num = len+maxi_pad;

    lilend_num[0] = num&0xff;
    lilend_num[1] = (num>>8)&0xff;
    lilend_num[2] = (num>>16)&0xff;
    lilend_num[3] = (num>>24)&0xff;

    memcpy(buf_frames + buf_frames_size, lilend_num, 4);
    buf_frames_size += 4;
  }  
  memcpy(buf_frames + buf_frames_size, buffer, len);
  buf_frames_size += len;
  
  pad_buffer = calloc(maxi_pad, sizeof(char));
  memcpy(buf_frames + buf_frames_size, pad_buffer, maxi_pad);
  buf_frames_size += maxi_pad;

  if(pad_buffer)
    free(pad_buffer);
}
#else
void ite_avi_add_frame(struct ite_avi_t *avi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* if your frame is raggin, give it some paddin' */
  int t;
  char *pad_buffer;

  avi->offset_count++;
  avi->stream_header_v.data_length++;

  maxi_pad=len%4;
  if (maxi_pad>0) maxi_pad=4-maxi_pad;

  if (avi->offset_count >= avi->offsets_len)
  {
    avi->offsets_len+=1024;
    avi->offsets=realloc(avi->offsets,avi->offsets_len*sizeof(int));
  }

  avi->offsets[avi->offsets_ptr++]=len+maxi_pad;
  fwrite("00dc", 4, 1, avi->out);
  write_int(avi->out,len+maxi_pad);

  fwrite(buffer, 1, len, avi->out);
  
  pad_buffer = calloc(maxi_pad, sizeof(char));
  fwrite(pad_buffer, 1, maxi_pad, avi->out);

  if(pad_buffer)
    free(pad_buffer);
}
#endif

#if (ADD_IN_BUFFER_FIRST)
void ite_avi_add_audio(struct ite_avi_t *avi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* incase audio bleeds over the 4 byte boundary  */
  int t;
  char *pad_buffer;

  avi->offset_count++;

  maxi_pad=len%4;
  if (maxi_pad>0) maxi_pad=4-maxi_pad;

  if (avi->offset_count >= avi->offsets_len)
  {
    avi->offsets_len+=1024;
    avi->offsets=realloc(avi->offsets,avi->offsets_len*sizeof(int));
  }

  avi->offsets[avi->offsets_ptr++]=(len+maxi_pad)|0x80000000;

  memcpy(buf_frames + buf_frames_size, "01wb", 4);
  buf_frames_size += 4;
  {
    char lilend_num[4];
    int num = len+maxi_pad;

    lilend_num[0] = num&0xff;
    lilend_num[1] = (num>>8)&0xff;
    lilend_num[2] = (num>>16)&0xff;
    lilend_num[3] = (num>>24)&0xff;

    memcpy(buf_frames + buf_frames_size, lilend_num, 4);
    buf_frames_size += 4;
  }
  memcpy(buf_frames + buf_frames_size, buffer, len);
  buf_frames_size += len;
  
  pad_buffer = calloc(maxi_pad, sizeof(char));
  memcpy(buf_frames + buf_frames_size, pad_buffer, maxi_pad);
  buf_frames_size += maxi_pad;
    
  avi->stream_header_a.data_length+=len+maxi_pad;

  if(pad_buffer)
    free(pad_buffer);
}
#else
void ite_avi_add_audio(struct ite_avi_t *avi, unsigned char *buffer, int len)
{
  int maxi_pad;  /* incase audio bleeds over the 4 byte boundary  */
  int t;
  char *pad_buffer;

  avi->offset_count++;

  maxi_pad=len%4;
  if (maxi_pad>0) maxi_pad=4-maxi_pad;

  if (avi->offset_count >= avi->offsets_len)
  {
    avi->offsets_len+=1024;
    avi->offsets=realloc(avi->offsets,avi->offsets_len*sizeof(int));
  }

  avi->offsets[avi->offsets_ptr++]=(len+maxi_pad)|0x80000000;

  fwrite("01wb", 4, 1, avi->out);
  write_int(avi->out,len+maxi_pad);

  fwrite(buffer, 1, len, avi->out);

  //for (t=0; t<maxi_pad; t++) putc(0,kavi->out);
  pad_buffer = calloc(maxi_pad, sizeof(char));
  fwrite(pad_buffer, 1, maxi_pad, avi->out);

  avi->stream_header_a.data_length+=len+maxi_pad;

  if(pad_buffer)
    free(pad_buffer);
}
#endif

void ite_avi_close(struct ite_avi_t *avi)
{
  long t;

#if (ADD_IN_BUFFER_FIRST)
  if(buf_frames_size)
  {
    fwrite(buf_frames, buf_frames_size, 1, avi->out);
    buf_frames_size = 0;
  }
#endif

  t=ftell(avi->out);
  fseek(avi->out,avi->marker,SEEK_SET);
  write_int(avi->out,t-avi->marker-4);
  fseek(avi->out,t,SEEK_SET);
  printf("avi->offset_count = %d\n", avi->offset_count);
  write_index(avi->out, avi->offset_count, avi->offsets);

  free(avi->offsets);

  /* reset some avi header fields */
  avi->avi_header.number_of_frames=avi->stream_header_v.data_length;

  t=ftell(avi->out);
  fseek(avi->out,12,SEEK_SET);
  write_avi_header_chunk(avi);
  fseek(avi->out,t,SEEK_SET);

  t=ftell(avi->out);
  fseek(avi->out,4,SEEK_SET);
  write_int(avi->out,t-8);
  fseek(avi->out,t,SEEK_SET);

  fclose(avi->out);
  free(avi);
#if (ADD_IN_BUFFER_FIRST)  
  free(buf_frames);
#endif
}



