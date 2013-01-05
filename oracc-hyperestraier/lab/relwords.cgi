#! /usr/bin/ruby

#================================================================
# relwords.cgi
# CGI script to dump related words
#================================================================


# library requirement
require "cgi"
require "villa"


# constants
DBNAME = "relwords.qdb"


# get parameters
scriptname = ENV["SCRIPT_NAME"]
scriptname = $0 unless scriptname
scriptname = scriptname.gsub(/.*\//, "")
cgi = CGI::new
word = cgi["word"]
word.downcase!
word.strip!


# show HTTP headers
printf("Content-Type: text/html; charset=UTF-8\r\n\r\n")


# show page header
printf("%s", <<__EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<meta http-equiv="Content-Language" content="en" />
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<meta http-equiv="Content-Style-Type" content="text/css" />
<link rel="contents" href="./" />
<title>Related Word Extractor</title>
<style type="text/css">html { margin: 0em 0em; padding 0em 0em; background: #eeeeee none; }
body { margin: 1em 2em; padding 0em 0em;
  background: #eeeeee none; color: #444444;
  font-style: normal; font-weight: normal; }
h1 { margin-top: 1.8em; margin-bottom: 1.3em; font-weight: bold; font-size: larger; }
div.myform { margin: 1em 1em; }
p { margin: 1em 1em; }
a { color: #0022aa; text-decoration: none; font-weight: bold; }
a:hover,a:focus { color: #0033ee; text-decoration: underline; }
a.head { color: #111111; text-decoration: none; }
strong { color: #000000; }
ul { line-height: 150%; }
</style>
</head>
<body>
<hr />
__EOF
);


# show input form
escword = CGI::escapeHTML(word)
printf("%s", <<__EOF
<form method="get" action="#{scriptname}">
<div class="myform">Word:
<input type="text" name="word" value="#{escword}" size="32" />
<input type="submit" value="Extract" />
</div>
</form>
<hr />
__EOF
);


# show result
if word.length > 0
  sum = 0
  allkwd = {}
  begin
    db = Villa::new(DBNAME)
    terms = word.split(/[ \t]/)
    terms.each do |term|
      next if term.length < 1
      begin
        line = db.get(term)
      rescue
        line = "0"
      end
      fields = line.split(/\t/)
      num = fields.shift.to_i
      sum = sum + num
      diam = 32 / ((num + 8) ** 0.6)
      for i in 0...fields.length
        next if i % 2 > 0
        key = fields[i]
        val = fields[i+1].to_i * diam
        cur = allkwd[key]
        allkwd[key] = cur ? cur + val : val
      end
    end
  rescue
  ensure
    db.close if db
  end
  if sum > 0 && allkwd.size > 0
    escword = CGI::escapeHTML(word)
    printf("<p>Related words of \"<strong id=\"origword\">%s</strong>\"" + \
           " (<span id=\"orignum\">%d</span>).</p>\n", escword, sum)
    scores = []
    allkwd.each do |key, val|
      scores.push([key, val])
    end
    scores.sort! do |a, b|
      b[1] - a[1]
    end
    printf("<ul>\n")
    i = 0
    scores.each do |elem|
      key = elem[0]
      val = elem[1]
      printf("<li><a href=\"%s?word=%s\" id=\"relword%04d\">%s</a>" + \
             " (<span id=\"relnum%04d\">%d</span>)</li>\n", scriptname, CGI::escape(key),
             i / 2 + 1, CGI::escapeHTML(key), i / 2 + 1, val)
      i += 1
      break if i >= 32
    end
    printf("</ul>\n")
  else
    printf("<p>There is no related word for \"<strong id=\"origword\">%s</strong>\".</p>\n",
           escword)
    printf("<p>\"%s\" could not be opened.</p>", DBNAME) unless db
  end
else
  printf("<p>Input a source word of relation.</p>\n")
end
printf("<hr />\n");


# show page footer
printf("%s", <<__EOF
</body>
</html>
__EOF
)


# exit normally
exit(0)



# END OF FILE
