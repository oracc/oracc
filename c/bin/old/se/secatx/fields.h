
#ifndef _FIELDS_H
#define _FIELDS_H

#define field_names \
  C(accession_no) \
  C(accounting_period) \
  C(acquisition_history) \
  C(aka_name) \
  C(alternative_years) \
  C(atf_source) \
  C(atf_up) \
  C(author) \
  C(author_remarks) \
  C(cdli_collation) \
  C(citation) \
  C(composite_id) \
  C(condition_description) \
  C(corpus) \
  C(date_entered) \
  C(date_of_origin) \
  C(date_remarks) \
  C(dates_referenced) \
  C(designation) \
  C(display_size) \
  C(electronic_publication) \
  C(elevation) \
  C(envelope) \
  C(excavation_no) \
  C(external_id) \
  C(findspot_remarks) \
  C(findspot_square) \
  C(genre) \
  C(height) \
  C(id_text) \
  C(join_information) \
  C(language) \
  C(lineart_up) \
  C(material) \
  C(museum_no) \
  C(name) \
  C(object_preservation) \
  C(object_remarks) \
  C(object_type) \
  C(owner) \
  C(period) \
  C(period_remarks) \
  C(photo_up) \
  C(primary_publication) \
  C(provenience) \
  C(provenience_remarks) \
  C(public) \
  C(public_atf) \
  C(public_images) \
  C(publication_date) \
  C(publication_history) \
  C(published_collation) \
  C(seal_id) \
  C(seal_information) \
  C(stratigraphic_level) \
  C(subcorpus) \
  C(subgenre) \
  C(subgenre_remarks) \
  C(supergenre) \
  C(surface_preservation) \
  C(text_remarks) \
  C(thickness) \
  C(tradition) \
  C(width) \
  C(writing_phase) \
  C(xproject)

#define C(x) x,
enum fields 
{ 
  field_names
  nofield,
  not_in_use,
  next_uid
};

struct sn_tab
{
  const char *name;
  enum fields uid;
};

struct sn_tab *statnames (register const char *str, register unsigned int len);

#endif /*_FIELDS_H*/
