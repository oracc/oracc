#ifndef _VID_H_
#define _VID_H_
enum vid_proj { vid_proj_xmd , vid_proj_xtf };
struct vid_data
{
  Hash_table *vidh;
  char **ids;
  char *padded_ids;
  char *vpool;
  char *file;
  int ids_alloced;
  int ids_used;
  int max_len;
  struct npool *pool;
};

struct vid_data *vid_init(void);
void vid_term(struct vid_data *);
const char *vid_map_id(struct vid_data *vp, const char *xid);
void vid_new_id(struct vid_data *vp, const char *xid);
const char *vid_get_id(struct vid_data *vp, int vid, enum vid_proj which_proj);
const char *vid_get_PQ(struct vid_data *vp, int vid);
void vid_dump_data(struct vid_data *vp, const char *fname);
void vid_finish(struct vid_data *vp);
struct vid_data *vid_load_data(const char *fname);
void vid_free(struct vid_data*vp);

#endif /*_VID_H*/
