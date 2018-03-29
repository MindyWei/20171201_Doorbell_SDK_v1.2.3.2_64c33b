
/*

This code is copyright 2008 by Michael Kohn
This code falls under the LGPL license

http://www.mikekohn.net/
Michael Kohn <mike@mikekohn.net>

*/

struct ite_avi_audio_t
{
  int channels;
  int bits;
  int samples_per_second;
};

struct ite_avi_header_t
{
  int time_delay;
  int data_rate;
  int reserved;
  int flags;
  int number_of_frames;
  int initial_frames;
  int data_streams;
  int buffer_size;
  int width;
  int height;
  int time_scale;
  int playback_data_rate;
  int starting_time;
  int data_length;
};

struct ite_stream_header_t
{
  char data_type[5];
  char codec[5];
  int flags;
  int priority;
  int initial_frames;
  int time_scale;
  int data_rate;
  int start_time;
  int data_length;
  int buffer_size;
  int quality;
  int sample_size;
};

struct ite_stream_format_v_t
{
  unsigned int header_size;
  unsigned int width;
  unsigned int height;
  unsigned short int num_planes;
  unsigned short int bits_per_pixel;
  unsigned int compression_type;
  unsigned int image_size;
  unsigned int x_pels_per_meter;
  unsigned int y_pels_per_meter;
  unsigned int colors_used;  
  unsigned int colors_important;  
  unsigned int *palette;
  unsigned int palette_count;
};

struct ite_stream_format_a_t
{
  unsigned short format_type;
  unsigned short channels;
  unsigned int sample_rate;
  unsigned int bytes_per_second;
  unsigned short block_align;
  unsigned short bits_per_sample;
  unsigned short size;
};

struct ite_avi_index_t
{
  int id;
  int flags;
  int offset;
  int length;
};

struct ite_avi_t
{
  FILE *out;
  struct ite_avi_header_t avi_header;
  struct ite_stream_header_t stream_header_v;
  struct ite_stream_format_v_t stream_format_v;
  struct ite_stream_header_t stream_header_a;
  struct ite_stream_format_a_t stream_format_a;
  long marker;   /* movi marker */
  int offsets_ptr;
  int offsets_len;
  long offsets_start;
  unsigned int *offsets;
  int offset_count;
};

struct ite_avi_t *ite_avi_open(char *filename, int width, int height, char *fourcc, int fps, struct ite_avi_audio_t *audio);
void ite_avi_add_frame(struct ite_avi_t *kavi, unsigned char *buffer, int len);
void ite_avi_add_audio(struct ite_avi_t *avi, unsigned char *buffer, int len);
void ite_avi_close(struct ite_avi_t *kavi);



