void
tree_xml_rnv(FILE *fp, Tree *tp, struct xnn_data *xdp, const char *rncbase)
{
  Xmlhelper *xhp = xmlh_init(fp ? fp : stdout);
  rnvxml_init(tp, xdp, rncbase);
  tree_iterator(tp, xhp, tree_xml_node, tree_xml_post);
  free(xhp);
}
