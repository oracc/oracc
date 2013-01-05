/* These are just included, optimizable statics for several modules */
static int
XidVal(long int id)
{
  if (idVal(id) >= X_FACTOR)
    return idVal(id)-X_FACTOR;
  else
    return idVal(id);
}

static char
id_prefix(long int id)
{
  extern const char *return_index;
  if (l2 && (!return_index || strncmp(return_index,"cbd",3)))
    {
      return 'v';
    }
  else
    {
      if (isQ(id))
	return ret_type_rules->neg_id_prefix;
      else
	{
	  if (idVal(id) > X_FACTOR)
	    return 'X';
	  else
	return ret_type_rules->pos_id_prefix;
	}
    }
}

static void
selemx_print_fields(FILE *fp, int f)
{
  switch (f)
    {
    case 0x1000:
      fputs("; gw/sense", fp);
      break;
    case 0x2000:
      fputs("; cf/norm", fp);
      break;
    case 0x4000:
      fputs("; m1", fp);
      break;
    case 0x8000:
      fputs("; m2", fp);
      break;
    }
}
