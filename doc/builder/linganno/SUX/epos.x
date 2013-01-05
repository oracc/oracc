<h1>EPOS</h1>

<p>The lemmatizer actually uses a double POS: the
<strong>default</strong> POS and the <strong>effective</strong> POS
(EPOS).</p>

<p>Every instance of a lemma has a part-of-speech associated with
it which is tied to the current syntactic context: this is the
<strong>effective</strong> POS.  Every lemma has a part-of-speech which
is assumed to be the effective part-of-speech unless the lemmatizer is
informed otherwise: this is the <strong>default</strong> POS.  It is
not necessarily the case that the default POS (or, since it is the
unmarked case, simply POS) is the statistically most frequent EPOS
(though this usually will be true).  The nomination of a POS is primarily
a matter of practical convenience.</p>

<p>In most lemmatization, even the existence of the EPOS can be
ignored.  Some words, however, have more than one POS and it is
necessary to annotate those cases in which the EPOS is not the POS.
Another common case is that some classes of words may be used in
certain contexts with a unusual EPOS--verbs may function as nouns, for
example.  Here, too, the EPOS must be annotated explicitly.</p>

<p><strong>The EPOS is never inherited; every instance in which the EPOS is
different from the POS must be annotated explicitly.</strong></p>

<p>The EPOS is signified in ATF by use of the ASCII prime symbol, or
right quote (<code>'</code>) immediately before the POS.  Annotation
may give both POS and EPOS or EPOS alone:</p>

<pre class="example">
1. sag₉-ga-zu bi₂-in-dug₄
#lem: sag[good]V'N dug[speak]
</pre>

<p>or:</p>

<pre class="example">
1. sag₉-ga-zu bi₂-in-dug₄
#lem: sag[good]'N dug[speak]
</pre>

<p>(Other analyses of the construct above are possible; this is just
an example).</p>


